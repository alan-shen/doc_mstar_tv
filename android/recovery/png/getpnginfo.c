/*
 * gcc png_example.c -Iinclude -lpng
 *
 * The original code is based android_4.4.2_r2 branch.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <png.h>
#include "pixelflinger/pixelflinger.h" // android/system/core/include/pixelfligner/pixelflinger.h

// copy from minui.h
typedef struct {
    int width;
    int height;
    int row_bytes;
    int pixel_bytes;
    unsigned char* data;
} GRSurface;

typedef GRSurface* gr_surface;

#define SURFACE_DATA_ALIGNMENT 8

static gr_surface malloc_surface(size_t data_size) {
    unsigned char* temp = malloc(sizeof(GRSurface) + data_size + SURFACE_DATA_ALIGNMENT);
    if (temp == NULL) return NULL;
    gr_surface surface = (gr_surface) temp;
    surface->data = temp + sizeof(GRSurface) +
        (SURFACE_DATA_ALIGNMENT - (sizeof(GRSurface) % SURFACE_DATA_ALIGNMENT));
    return surface;
}


static int open_png(const char* file_name, png_structp* png_ptr, png_infop* info_ptr,
                    png_uint_32* width, png_uint_32* height, png_byte* channels) {
    unsigned char header[8];
    int result = 0;

    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        result = -1;
        goto exit;
    }

    size_t bytesRead = fread(header, 1, sizeof(header), fp);
    if (bytesRead != sizeof(header)) {
        result = -2;
        goto exit;
    }

    if (png_sig_cmp(header, 0, sizeof(header))) {
        result = -3;
        goto exit;
    }

    *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!*png_ptr) {
        result = -4;
        goto exit;
    }

    *info_ptr = png_create_info_struct(*png_ptr);
    if (!*info_ptr) {
        result = -5;
        goto exit;
    }

    if (setjmp(png_jmpbuf(*png_ptr))) {
        result = -6;
        goto exit;
    }

    png_init_io(*png_ptr, fp);
    png_set_sig_bytes(*png_ptr, sizeof(header));
    png_read_info(*png_ptr, *info_ptr);

    int color_type, bit_depth;
    png_get_IHDR(*png_ptr, *info_ptr, width, height, &bit_depth,
            &color_type, NULL, NULL, NULL);

    *channels = png_get_channels(*png_ptr, *info_ptr);

    if (bit_depth == 8 && *channels == 3 && color_type == PNG_COLOR_TYPE_RGB) {
        // 8-bit RGB images: great, nothing to do.
    } else if (bit_depth <= 8 && *channels == 1 && color_type == PNG_COLOR_TYPE_GRAY) {
        // 1-, 2-, 4-, or 8-bit gray images: expand to 8-bit gray.
        png_set_expand_gray_1_2_4_to_8(*png_ptr);
    } else if (bit_depth <= 8 && *channels == 1 && color_type == PNG_COLOR_TYPE_PALETTE) {
        // paletted images: expand to 8-bit RGB.  Note that we DON'T
        // currently expand the tRNS chunk (if any) to an alpha
        // channel, because minui doesn't support alpha channels in
        // general.
        png_set_palette_to_rgb(*png_ptr);
        *channels = 3;
    } else {
        fprintf(stderr, "minui doesn't support PNG depth %d channels %d color_type %d\n",
                bit_depth, *channels, color_type);
        result = -7;
        goto exit;
    }

    return result;

  exit:
    if (result < 0) {
        png_destroy_read_struct(png_ptr, info_ptr, NULL);
    }
    if (fp != NULL) {
        fclose(fp);
    }

    return result;
}

static int matches_locale(const char* loc, const char* locale) {
    if (locale == NULL) return 0;

    if (strcmp(loc, locale) == 0) return 1;

    // if loc does *not* have an underscore, and it matches the start
    // of locale, and the next character in locale *is* an underscore,
    // that's a match.  For instance, loc == "en" matches locale ==
    // "en_US".

    int i;
    for (i = 0; loc[i] != 0 && loc[i] != '_'; ++i);
    if (loc[i] == '_') return 0;

    return (strncmp(locale, loc, i) == 0 && locale[i] == '_');
}


int read_png(const char* file_name, const char* locale/*,
                                    gr_surface* pSurface*/) {
    gr_surface surface = NULL;
    int result = 0;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_uint_32 width, height;
    png_byte channels;

    //*pSurface = NULL;

    if (locale == NULL) {
        surface = malloc_surface(0);
        surface->width = 0;
        surface->height = 0;
        surface->row_bytes = 0;
        surface->pixel_bytes = 1;
        goto exit;
    }

    result = open_png(file_name, &png_ptr, &info_ptr, &width, &height, &channels);
    if (result < 0) return result;

    if (channels != 1) {
        result = -7;
        goto exit;
    }

    unsigned char* row = malloc(width);
    png_uint_32 y;
    for (y = 0; y < height; ++y) {
        png_read_row(png_ptr, row, NULL);
        int w = (row[1] << 8) | row[0];
        int h = (row[3] << 8) | row[2];
        int len = row[4];
        char* loc = (char*)row+5;

        printf("  %20s: %s (%d x %d @ %lu)\n", file_name, loc, w, h, y);
        if (y+1+h >= height || matches_locale(loc, locale)) {
            surface = malloc_surface(w*h);
            if (surface == NULL) {
                result = -8;
                goto exit;
            }
            surface->width = w;
            surface->height = h;
            surface->row_bytes = w;
            surface->pixel_bytes = 1;

            int i;
            for (i = 0; i < h; ++i, ++y) {
                png_read_row(png_ptr, row, NULL);
                memcpy(surface->data + i*w, row, w);
            }

            //*pSurface = (gr_surface) surface;
            break;
        } else {
            int i;
            for (i = 0; i < h; ++i, ++y) {
                png_read_row(png_ptr, row, NULL);
            }
        }
    }

exit:
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (result < 0 && surface != NULL) free(surface);
    return result;
}

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		printf("Usage: ./a.out file_name locale\n");
		exit(1);
	}

	read_png(argv[1], argv[2]);
	return 0;
}

