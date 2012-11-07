/*

	example for importing and combining mono wave files
	it loads /usr/share/sounds/alsa/Front_Left.wav and plays it looped with 70% of speed on left channel
	it loads /usr/share/sounds/alsa/Front_Right.wav and plays it looped with 140% of speed on right channel
	it loads /usr/share/sounds/alsa/Noise.wav and plays it looped with 50% of volume, 150% of speed on both channels
	finally it saves them to 16-bit 48000Hz WAV PCM example5.wav

*/

#include <stdio.h>

#include <riffoser.h>

int main(){
	struct riffoser_track * track;

	struct riffoser_wave *frontleft, *frontright, *noise;

	track=riffoser_track_init(2);

	frontleft=riffoser_wave_loadfromwav("/usr/share/sounds/alsa/Front_Left.wav",100,70);
	frontright=riffoser_wave_loadfromwav("/usr/share/sounds/alsa/Front_Right.wav",100,140);
	noise=riffoser_wave_loadfromwav("/usr/share/sounds/alsa/Noise.wav",50,150);

	riffoser_track_addwave(track,frontleft,1,0,10);

	riffoser_track_addwave(track,frontright,0,0,10);

	riffoser_track_addwave(track,noise,0,0,10);
	riffoser_track_addwave(track,noise,1,0,10);

	riffoser_track_writewav(track,"example5.wav",48000,16);

	riffoser_wave_free(frontleft);
	riffoser_wave_free(frontright);
	riffoser_wave_free(noise);

        riffoser_track_free(track);

	exit(EXIT_SUCCESS);

}
