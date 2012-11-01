#ifndef RIFFOSER_H
#define	RIFFOSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define riffoser_percent_t float
#define riffoser_frequency_t float
#define riffoser_wavetype_t unsigned char
#define riffoser_trackpos_t float
#define riffoser_wavestate_state_t unsigned char
#define RIFFOSER_WAVESTATE_IDLE 0
#define RIFFOSER_WAVESTATE_RENDERING 1
#define RIFFOSER_WAVESTATE_FINISHED 2
#define riffoser_channel_t unsigned char
struct riffoser_wavestate {
	riffoser_channel_t channel;
	riffoser_trackpos_t from;
	riffoser_trackpos_t to;
	riffoser_wavestate_state_t state;
	float samplenum;
};

struct riffoser_instrumentstate {
	riffoser_wavestate_state_t state;
	riffoser_trackpos_t from;
	riffoser_trackpos_t to;
	float samplenum;
	riffoser_channel_t channel;
	riffoser_percent_t amplitude;
	riffoser_frequency_t frequency;
	riffoser_percent_t pitch;
};

struct riffoser_instrument {
	struct riffoser_wave ** waves;
	struct riffoser_wavestate ** wavestates;
	unsigned short waves_count;
	struct riffoser_instrument ** instruments;
	struct riffoser_instrumentstate ** instrumentstates;
	unsigned short instruments_count;
};

#define riffoser_samplerate_t unsigned int
#define riffoser_bitspersample_t unsigned char
#define riffoser_tracklen_t float
struct riffoser_track {
	riffoser_channel_t channels;
	struct riffoser_wave ** waves;
	struct riffoser_wavestate ** wavestates;
	unsigned long waves_count;
	struct riffoser_instrument ** instruments;
	struct riffoser_instrumentstate ** instrumentstates;
	unsigned short instruments_count;
	riffoser_tracklen_t length;
};

struct riffoser_wave {
	riffoser_percent_t amplitude;
	riffoser_frequency_t frequency;
	riffoser_percent_t pitch;
	riffoser_wavetype_t type;
	riffoser_channel_t channels;
	unsigned char * data;
	unsigned long data_count;
};

#define RIFFOSER_WAVE_SQUARE 1
#define RIFFOSER_WAVE_TRIANGLE 2
#define RIFFOSER_WAVE_SINE 3
#define RIFFOSER_WAVE_COSINUSOID 4
#define _RIFFOSER_WAVE_DATA 5

#define RIFFOSER_WAVE_TEST1 11
#define RIFFOSER_WAVE_TEST2 12

double DIVTIMES(double base,double by,unsigned char amount) {
	unsigned char i;
	for (i=0;i<amount;i++)
		base/=by;
	return base;
}
#define RIFFOSER_WAVE_FUNC(_wave,x) {\
	if (_wave->type==RIFFOSER_WAVE_SQUARE)\
		fret=x<_wave->pitch?0:100;\
	else if (_wave->type==RIFFOSER_WAVE_TRIANGLE)\
		fret=x<_wave->pitch?x*2:100-(x-_wave->pitch)*2;\
	else if (_wave->type==RIFFOSER_WAVE_SINE)\
		fret=100*sin(2*M_PI*x/100+_wave->pitch);/*x<_wave->pitch?sin(x)*100:sin(100-(x-_wave->pitch)*2);*/\
	else if (_wave->type==RIFFOSER_WAVE_COSINUSOID)\
		fret=x<_wave->pitch?cos(x)*100:cos(100-(x-_wave->pitch)*2);\
	else if (_wave->type==RIFFOSER_WAVE_TEST1)\
		fret=30*(cos(M_PI*(x+1))-sin(2*M_PI*x/100))/*x<_wave->pitch?tan(x)*100:tan(100-(x-_wave->pitch)*2)*100*/;\
	else if (_wave->type==RIFFOSER_WAVE_TEST2)\
		fret=DIVTIMES(100,2,10*(-cos(M_PI*x)+sin(2*M_PI*x)));\
	else if (_wave->type==_RIFFOSER_WAVE_DATA)\
		fret=_wave->data[(unsigned long)round(x*2)];\
}


extern struct riffoser_track * riffoser_track_init(riffoser_channel_t channels);
extern void riffoser_track_free(struct riffoser_track * track);
extern void riffoser_track_writewav(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample);
extern void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to);

extern struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t wavetype,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch);
extern struct riffoser_wave * riffoser_wave_loadfromwav(char * filename,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch);
extern void riffoser_wave_free(struct riffoser_wave * wave);

extern struct riffoser_instrument * riffoser_instrument_init();
extern void riffoser_instrument_free(struct riffoser_instrument * instrument);

#ifdef	__cplusplus
}
#endif

#endif	/* RIFFOSER_H */

