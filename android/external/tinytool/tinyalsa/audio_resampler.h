#ifndef __AUDIO_RESAMPLER_H__
#define __AUDIO_RESAMPLER_H__

struct resample_para {
    unsigned int FractionStep;
    unsigned int SampleFraction;
    short lastsample_left;
    short lastsample_right;
    unsigned int input_sr;
    unsigned int output_sr;
    unsigned int channels;
};

int resampler_init(struct resample_para *resample);
int resample_process(struct resample_para *resample, unsigned int in_frame,
	short* input, short* output);


#endif
