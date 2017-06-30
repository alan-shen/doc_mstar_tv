/* tinyplay.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include "audio_resampler.h"

/* mitv, debug, shenpengru@xiaomi.com */
#define SUPPORT_PLAY_MONO_ON_STERO_DEVICE (1)
#define SUPPORT_PLAY_RESAMPLE_16K_MUSIC   (1)

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header {
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

static int close = 0;

void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                 unsigned int rate, unsigned int bits, unsigned int period_size,
                 unsigned int period_count);

void stream_close(int sig)
{
    /* allow the stream to be closed gracefully */
    signal(sig, SIG_IGN);
    close = 1;
}

/*
 * -f:
 *     Force to play some format of wave which not support on hw device.
 *     Now, only support force to play:
 *         1.MONO file;
 *         2.Resample 16K to 48K
 **/
int gForce = 0;
int main(int argc, char **argv)
{
    FILE *file;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    unsigned int device = 0;
    unsigned int card = 0;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    char *filename;
    int more_chunks = 1;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.wav [-D card] [-d device] [-p period_size]"
                " [-n n_periods] \n", argv[0]);
        return 1;
    }

    filename = argv[1];
    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        return 1;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
        (riff_wave_header.wave_id != ID_WAVE)) {
        fprintf(stderr, "Error: '%s' is not a riff/wave file\n", filename);
        fclose(file);
        return 1;
    }

    do {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id) {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    } while (more_chunks);

    /* parse command line arguments */
    argv += 2;
    while (*argv) {
        if (strcmp(*argv, "-d") == 0) {
            argv++;
            if (*argv)
                device = atoi(*argv);
        }
        if (strcmp(*argv, "-p") == 0) {
            argv++;
            if (*argv)
                period_size = atoi(*argv);
        }
        if (strcmp(*argv, "-n") == 0) {
            argv++;
            if (*argv)
                period_count = atoi(*argv);
        }
        if (strcmp(*argv, "-D") == 0) {
            argv++;
            if (*argv)
                card = atoi(*argv);
        }
        if (strcmp(*argv, "-f") == 0) {
            if (*argv)
                gForce = 1;
        }
        if (*argv)
            argv++;
    }

    play_sample(file, card, device, chunk_fmt.num_channels, chunk_fmt.sample_rate,
                chunk_fmt.bits_per_sample, period_size, period_count);

    fclose(file);

    return 0;
}

int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;

    min = pcm_params_get_min(params, param);
    if (value < min) {
        fprintf(stderr, "%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }

    max = pcm_params_get_max(params, param);
    if (value > max) {
        fprintf(stderr, "%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }

    return is_within_bounds;
}

int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    struct pcm_params *params;
    int can_play;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        fprintf(stderr, "Unable to open PCM device %u.\n", device);
        return 0;
    }

    can_play = check_param(params, PCM_PARAM_RATE, rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, period_count, "Period count", "Hz");

    pcm_params_free(params);

    return can_play;
}

int is_support_force_play(void) {
    return gForce;
}

void play_sample(FILE *file, unsigned int card, unsigned int device, unsigned int channels,
                 unsigned int rate, unsigned int bits, unsigned int period_size,
                 unsigned int period_count)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    int size;
#if SUPPORT_PLAY_MONO_ON_STERO_DEVICE
    char *buffer_stero;
    char *buffer_48k;
    int i,j,loops;
    int size_per_frame;
    int need_remix = 0;
#endif
#if SUPPORT_PLAY_RESAMPLE_16K_MUSIC
    struct resample_para para;
    int need_resample = 0;
#endif
    int num_read;
    int can_play = 1;

    memset(&config, 0, sizeof(config));
    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    if (bits == 32)
        config.format = PCM_FORMAT_S32_LE;
    else if (bits == 16)
        config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    if (!sample_is_playable(card, device, channels, rate, bits, period_size, period_count)) {
        can_play = 0;
        #if SUPPORT_PLAY_RESAMPLE_16K_MUSIC
        /* check if need do resample? */
        if (is_support_force_play() && (rate == 16000)) {
            need_resample = 1;
            config.rate = 48000;
            if (!sample_is_playable(card, device, channels, config.rate, bits, period_size, period_count)) {
                can_play = 0;
            } else {
                can_play = 1;
            }
        }
        #endif
        #if SUPPORT_PLAY_MONO_ON_STERO_DEVICE
        /* check if need do remix for mono music? */
        if (is_support_force_play()  && (channels == 1)) {
            need_remix = 1;
            size_per_frame = bits*channels/8;
            if (!sample_is_playable(card, device, 2, config.rate, bits, period_size, period_count)) {
                can_play = 0;
            } else {
                can_play = 1;
            }
        }
        #endif
    }

    if (!can_play) {
        return;
    }

#if SUPPORT_PLAY_MONO_ON_STERO_DEVICE
    if (need_remix) {
        config.channels = 2;
        pcm = pcm_open(card, device, PCM_OUT, &config);
    } else {
        pcm = pcm_open(card, device, PCM_OUT, &config);
    }
#else
    pcm = pcm_open(card, device, PCM_OUT, &config);
#endif
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\n",
                device, pcm_get_error(pcm));
        return;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return;
    }

    printf("Playing sample: %u ch, %u hz, %u bit, %d size, %d size_per_frame\n", channels, rate, bits, size, size_per_frame);

#if SUPPORT_PLAY_MONO_ON_STERO_DEVICE
    if (need_remix) {
        buffer_stero = malloc(size*2);
        printf("-- mitv: force to support one channel --\n");
    }
#endif
#if SUPPORT_PLAY_RESAMPLE_16K_MUSIC
    if (need_resample) {
        buffer_48k   = malloc(size*2);
        printf("-- mitv: force to support sample rate < 32K --\n");
	}
#endif

    /* catch ctrl-c to shutdown cleanly */
    signal(SIGINT, stream_close);

    do {
        num_read = fread(buffer, 1, size, file);
#if SUPPORT_PLAY_MONO_ON_STERO_DEVICE
        if (need_remix) {
            if (num_read > 0) {
                loops = num_read/size_per_frame;
                /* create stero-data from mono-data!! */
                for (i=0; i<loops; i++) {
                    j = 2*i*size_per_frame;
                    memcpy(&buffer_stero[j],                &buffer[i*size_per_frame], size_per_frame);
                    memcpy(&buffer_stero[j+size_per_frame], &buffer[i*size_per_frame], size_per_frame);
                }
#if SUPPORT_PLAY_RESAMPLE_16K_MUSIC
                if (need_resample) {
                   para.input_sr  = rate;
                   para.output_sr = 48000;
                   para.channels  = 2;
                   resampler_init(&para);
                   resample_process(&para, loops, buffer_stero, buffer_48k);
                }
#endif
                if (pcm_write(pcm, buffer_stero, num_read*2)) {
                    fprintf(stderr, "Error playing sample\n");
                    break;
                }
            }
        } else {
            if (num_read > 0) {
                if (pcm_write(pcm, buffer, num_read)) {
                    fprintf(stderr, "Error playing sample\n");
                    break;
                }
            }
        }
#else
        if (num_read > 0) {
            if (pcm_write(pcm, buffer, num_read)) {
                fprintf(stderr, "Error playing sample\n");
                break;
            }
        }
#endif
    } while (!close && num_read > 0);

    free(buffer);
    pcm_close(pcm);
}

