/*

	this example generates stereo sound (example1.wav, which is 8-bit 44100hz PCM WAV) with 
	frequencies of channels that change dynamically in opposite directions
	right channel is sine wave, left is square wave

	dirty example, but it illustrates some very basic usage.
	when filters and effects will be implemented, life will be much easier

*/

#include <stdio.h>

#include <riffoser.h>

int main(){
	struct riffoser_track * track;
	struct riffoser_wave ** waves1, ** waves2;

	unsigned short i;

	track=riffoser_track_init(2);

	waves1=malloc(sizeof(struct riffoser_wave *)*300);
	waves2=malloc(sizeof(struct riffoser_wave *)*300);

	for (i=0;i<300;i++) {

		waves1[i]=riffoser_wave_init(RIFFOSER_WAVE_SINE,30,100+i*2,50);
		waves2[i]=riffoser_wave_init(RIFFOSER_WAVE_SQUARE,30,700-i*2,50);

	        riffoser_track_addwave(track,waves1[i],0,i,i*1+1);
		riffoser_track_addwave(track,waves2[i],1,i,i*1+1);

	}

	riffoser_track_writewav(track,"example1.wav",44100,8);

	for (i=0;i<300;i++) {
		riffoser_wave_free(waves1[i]);
		riffoser_wave_free(waves2[i]);
	}

	free(waves1);
	free(waves2);

        riffoser_track_free(track);

	exit(EXIT_SUCCESS);

}
