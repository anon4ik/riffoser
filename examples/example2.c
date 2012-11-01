/*

	this generates 5 wave sequences with wave additions (2 waves playing simultaneously), saves to example2.wav in 44100hz 16bit PCM format

*/

#include <stdio.h>

#include <riffoser.h>

int main(){
        struct riffoser_track * track;
        struct riffoser_wave * wave1, * wave2;

        unsigned int i;

        track=riffoser_track_init(2);

        wave1=riffoser_wave_init(RIFFOSER_WAVE_SINE,50,100,50);
        wave2=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,50,300,50);

        for (i=0;i<5;i++) {
                riffoser_track_addwave(track,wave1,0,i,i+0.5);
                riffoser_track_addwave(track,wave2,1,i,i+0.5);
                riffoser_track_addwave(track,wave2,0,i+0.25,i+0.75);
                riffoser_track_addwave(track,wave1,1,i+0.25,i+0.75);
        }

        riffoser_track_writewav(track,"example2.wav",44100,16);

        riffoser_wave_free(wave1);
        riffoser_wave_free(wave2);

        riffoser_track_free(track);

        system("aplay test.wav");

}

