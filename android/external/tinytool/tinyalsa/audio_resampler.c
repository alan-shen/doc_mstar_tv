#include <math.h>
#include <cutils/log.h>

#include "audio_resampler.h"

//Clip from 16.16 fixed-point to 0.15 fixed-point.
inline static short clip(int x) {
    if (x < -32768) {
        return -32768;
    } else if (x > 32767) {
        return 32767;
    } else {
        return x;
    }
}

int resampler_init(struct resample_para *resample) {
	
    printf("%s, Init Resampler: input_sr = %d, output_sr = %d \n",
		__FUNCTION__,resample->input_sr,resample->output_sr);
	
    static const double kPhaseMultiplier = 1L << 28;

    resample->FractionStep = (unsigned int) (resample->input_sr * kPhaseMultiplier 
							/ resample->output_sr);
    resample->SampleFraction = 0;
	resample->lastsample_left = 0;
	resample->lastsample_right = 0;
    return 0;
}

int resample_process(struct resample_para *resample, unsigned int in_frame,
        short* input, short* output) {
    unsigned int inputIndex = 0;
    unsigned int outputIndex = 0;
    unsigned int FractionStep = resample->FractionStep;

    static const unsigned int kPhaseMask = (1LU << 28) - 1;
    unsigned int frac = resample->SampleFraction;
    short lastsample_left = resample->lastsample_left;
    short lastsample_right = resample->lastsample_right;

	if(resample->channels == 2){
    	while (inputIndex == 0) {
        	*output++ = clip((int) lastsample_left + 
				((((int) input[0] - (int) lastsample_left) * ((int) frac >> 13)) >> 15));
        	*output++ = clip((int) lastsample_right + 
				((((int) input[1] - (int) lastsample_right) * ((int) frac >> 13)) >> 15));

        	frac += FractionStep;
        	inputIndex += (frac >> 28);
        	frac = (frac & kPhaseMask);
        	outputIndex++;
    	}

    	while (inputIndex < in_frame) {
        	*output++ = clip((int) input[2 * inputIndex - 2] + ((((int) input[2 * inputIndex] 
				- (int) input[2 * inputIndex - 2]) * ((int) frac >> 13)) >> 15));
        	*output++ = clip((int) input[2 * inputIndex - 1] + ((((int) input[2 * inputIndex + 1]
                - (int) input[2 * inputIndex - 1]) * ((int) frac >> 13)) >> 15));

       		frac += FractionStep;
        	inputIndex += (frac >> 28);
        	frac = (frac & kPhaseMask);
        	outputIndex++;
    	}

    	resample->lastsample_left = input[2 * in_frame - 2];
    	resample->lastsample_right = input[2 * in_frame - 1];
    	resample->SampleFraction = frac;
	}else{
		//left channel as output
		while (inputIndex == 0) {
        	*output++ = clip((int) lastsample_left + 
				((((int) input[0] - (int) lastsample_left) * ((int) frac >> 13)) >> 15));

        	frac += FractionStep;
        	inputIndex += (frac >> 28);
        	frac = (frac & kPhaseMask);
        	outputIndex++;
    	}

    	while (inputIndex < in_frame) {
        	*output++ = clip((int) input[2 * inputIndex - 2] + ((((int) input[2 * inputIndex] 
				- (int) input[2 * inputIndex - 2]) * ((int) frac >> 13)) >> 15));

       		frac += FractionStep;
        	inputIndex += (frac >> 28);
        	frac = (frac & kPhaseMask);
        	outputIndex++;
    	}

    	resample->lastsample_left = input[2 * in_frame - 2];
    	resample->SampleFraction = frac;
	}
	
    return outputIndex;
}



