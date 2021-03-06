/*

        a stress-test of some kind for waves mixing / addition
        this generates 1-minute-length stereo track with random waves
        parameters are easily tweakable via defines

*/

#include <stdio.h>

#include <riffoser.h>

#define WAVES_COUNT 200 // number of different waves
#define ADDITIONS_COUNT 600 // number of sounds in track
#define ADDITION_LENGTH (rand()%100)/10
#define TRACK_LENGTH 60 // track length, seconds
#define CHANNELS 2 // 1 - mono, 2 - stereo
#define MINFREQUENCY 50
#define MAXFREQUENCY 500
#define MINPITCH 10
#define MAXPITCH 90
#define MINVOLUME 30
#define MAXVOLUME 100

int main(){
	struct riffoser_track * track;
	struct riffoser_wave ** waves;

	unsigned char c1,c2;
	unsigned int i;
	unsigned int start,end;

	srand(time(NULL));

	track=riffoser_track_init(2);

	waves=malloc(sizeof(struct riffoser_wave *)*WAVES_COUNT);

	for (i=0;i<WAVES_COUNT;i++) {
		c1=rand()%3;
		c2=(c1==0?RIFFOSER_WAVE_SINE:(c1==1?RIFFOSER_WAVE_TRIANGLE:RIFFOSER_WAVE_SQUARE));
		waves[i]=riffoser_wave_init(c2,rand()%(MAXVOLUME-MINVOLUME+1)+MINVOLUME,rand()%(MAXFREQUENCY-MINFREQUENCY+1)+MINFREQUENCY,rand()%(MAXPITCH-MINPITCH+1)+MINPITCH);
	}

	for (i=0;i<ADDITIONS_COUNT;i++) {
		start=rand()%(TRACK_LENGTH*10);
		end=start+(ADDITION_LENGTH);
		riffoser_track_addwave(track,waves[rand()%WAVES_COUNT],rand()%CHANNELS,(float)start/10,(float)end/10);
	}


	riffoser_track_writewav(track,"example4.wav",44100,16);

	for (i=0;i<WAVES_COUNT;i++)
		riffoser_wave_free(waves[i]);
	free(waves);

        riffoser_track_free(track);

	exit(EXIT_SUCCESS);

}
