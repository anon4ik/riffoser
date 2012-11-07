/*

	this composes simple mono melody. lack of effects, beats and instruments makes it a bit flat, but its still a good
	example of how waves can be combined
	feel free to play with this source and listen

*/

#include <stdio.h>

#include <riffoser.h>

int main(){
	struct riffoser_track * track;
	struct riffoser_wave *bass1,*chim1,*chim2,*bass2,*bass3,*chip1,*chip2,*main1,*main2,*main3,*main4,*main5,*main6;

	unsigned int f,ff,i;

	track=riffoser_track_init(1);

	bass1=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,30,100,50);
	chim1=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);
	chim2=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,100,200,50);
	bass2=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,40,70,50);
	bass3=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,30,120,50);
	chip1=riffoser_wave_init(RIFFOSER_WAVE_SQUARE,40,90,20);
	chip2=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,40,100,50);
	main1=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,130,50);
	main2=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);
	main3=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,200,50);
	main4=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,300,50);
	main5=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,350,50);
	main6=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,500,50);

	// chim beats
	f=0;
	while (f<36) {
		riffoser_track_addwave(track,chim1,0,f,f+0.1);
		riffoser_track_addwave(track,chim1,0,f+0.175,f+0.3);
		riffoser_track_addwave(track,chim2,0,f+0.4,f+0.5);
		riffoser_track_addwave(track,chim1,0,f+0.6,f+0.7);
		riffoser_track_addwave(track,chim2,0,f+0.8,f+0.9);
		f=f+1;
	}

	// bass stuff
	f=0;
	for (f=0;f<5;f++) {
		ff=f*8;
		riffoser_track_addwave(track,bass1,0,ff,ff+4);
		riffoser_track_addwave(track,bass2,0,ff+4,ff+6);
		riffoser_track_addwave(track,bass3,0,ff+6,ff+8);

		// add chipping
		if (f>0) {
			for (i=0;i<4;i++) {
				riffoser_track_addwave(track,(f>1?chip2:chip1),0,ff+i*0.2,ff+i*0.2+0.08);
				riffoser_track_addwave(track,(f>1?chip2:chip1),0,ff+4+i*0.2,ff+4+i*0.2+0.08);
			}
		}
	}

	// main theme
	riffoser_track_addwave(track,main3,0,16,20);
	riffoser_track_addwave(track,main4,0,20,22);
	riffoser_track_addwave(track,main5,0,22,24);
	riffoser_track_addwave(track,main6,0,24,24.225);
	riffoser_track_addwave(track,main4,0,24.3,24.525);
	riffoser_track_addwave(track,main3,0,24.6,24.825);
	riffoser_track_addwave(track,main1,0,24.9,32);
	riffoser_track_addwave(track,main2,0,32,34);
	riffoser_track_addwave(track,main3,0,34,36);


	riffoser_track_writewav(track,"example3.wav",44100,16);

	riffoser_wave_free(bass1);

	riffoser_wave_free(chim1);
	riffoser_wave_free(chim2);
	riffoser_wave_free(bass2);
	riffoser_wave_free(bass3);
	riffoser_wave_free(chip1);
	riffoser_wave_free(chip2);
	riffoser_wave_free(main1);
	riffoser_wave_free(main2);
	riffoser_wave_free(main3);
	riffoser_wave_free(main4);
	riffoser_wave_free(main5);
	riffoser_wave_free(main6);

        riffoser_track_free(track);

	exit(EXIT_SUCCESS);

}
