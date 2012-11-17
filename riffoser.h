#ifndef RIFFOSER_H
#define	RIFFOSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define RIFFOSER_CHANNELS_MONO 1
#define RIFFOSER_CHANNEL_MONO 0
	
#define RIFFOSER_CHANNELS_STEREO 2
#define RIFFOSER_CHANNEL_LEFT 0
#define RIFFOSER_CHANNEL_RIGHT 1

#define RIFFOSER_CHANNEL_AUTO 255
	
#define riffoser_data_t double
#define riffoser_percent_t double
#define riffoser_frequency_t double
#define riffoser_wavetype_t unsigned char
#define riffoser_trackpos_t double
#define riffoser_wavestate_state_t unsigned char
#define RIFFOSER_WAVESTATE_IDLE 0
#define RIFFOSER_WAVESTATE_RENDERING 1
#define RIFFOSER_WAVESTATE_FINISHED 2
#define riffoser_channel_t unsigned char

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
#define riffoser_bytespersample_t unsigned char
#define riffoser_kbps_t unsigned short
#define riffoser_tracklen_t double
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

#define io_src_t riffoser_data_t
struct riffoser_io_struct {
	FILE *fp;
	unsigned long datalen;
	io_src_t *src;
	unsigned long srcsize;
	riffoser_bytespersample_t bytespersample;
	riffoser_samplerate_t samplerate;
	char *filename;
	riffoser_channel_t channels;
	riffoser_kbps_t kbps;
};

struct riffoser_wave {
	// for all waves
	riffoser_percent_t amplitude;
	riffoser_frequency_t frequency;
	riffoser_percent_t pitch;
	riffoser_wavetype_t type;
	riffoser_channel_t channels;
	
	// for _RIFFOSER_WAVE_DATA
	riffoser_data_t * data;
	unsigned long data_count;
	
	// for _RIFFOSER_WAVE_IO
	char *filename;
	struct {
		int (*start)(struct riffoser_io_struct *io);
		int (*bytes)(struct riffoser_io_struct *io);
		int (*end)(struct riffoser_io_struct *io);
	} readfuncs;
	riffoser_percent_t lengthscale;
};

#define RIFFOSER_WAVE_SQUARE 1
#define RIFFOSER_WAVE_TRIANGLE 2
#define RIFFOSER_WAVE_SINE 3
#define RIFFOSER_WAVE_COSINUSOID 4
#define _RIFFOSER_WAVE_DATA 128
#define _RIFFOSER_WAVE_IO 129

#define RIFFOSER_WAVE_TEST1 11
#define RIFFOSER_WAVE_TEST2 12

extern struct riffoser_track * riffoser_track_init(riffoser_channel_t channels);
extern void riffoser_track_free(struct riffoser_track * track);
extern void riffoser_track_writewav(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample);
extern void riffoser_track_writemp3(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample,riffoser_kbps_t kbps);
extern void riffoser_track_writeogg(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_kbps_t kbps);
extern void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to);

extern struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t wavetype,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch);
extern struct riffoser_wave * riffoser_wave_readwav(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length);
extern struct riffoser_wave * riffoser_wave_loadfromwav(char * filename,riffoser_percent_t amplitude,riffoser_percent_t length);
extern struct riffoser_wave * riffoser_wave_loadfromogg(char * filename,riffoser_percent_t amplitude,riffoser_percent_t length);
extern struct riffoser_wave * riffoser_wave_loadfrommp3(char * filename,riffoser_percent_t amplitude,riffoser_percent_t length);
extern void riffoser_wave_free(struct riffoser_wave * wave);

extern struct riffoser_instrument * riffoser_instrument_init();
extern void riffoser_instrument_free(struct riffoser_instrument * instrument);

#define RIFFOSER_VERSION_BASE "riffoser"
#define RIFFOSER_VERSION_MAJOR "0"
#define RIFFOSER_VERSION_MINOR "1"
#define RIFFOSER_VERSION RIFFOSER_VERSION_BASE"-"RIFFOSER_VERSION_MAJOR"."RIFFOSER_VERSION_MINOR

#ifdef	__cplusplus
}
#endif

#endif	/* RIFFOSER_H */

