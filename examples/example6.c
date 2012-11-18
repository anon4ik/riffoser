/*

	create left and right samples, save im .ogg files
	then read them and combine in final 48000hz ~160kbps .ogg file

*/

#include <stdio.h>

#include <riffoser.h>

int main() {

	struct riffoser_track *track;
	struct riffoser_wave *wave1,*wave2;

	// create left channel (triangle wave @120hz)
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);
	wave1=riffoser_wave_init(RIFFOSER_WAVE_TRIANGLE,100,120,50);
	riffoser_track_addwave(track,wave1,RIFFOSER_CHANNEL_LEFT,0,3);
	riffoser_track_writeogg(track,"example6l.ogg",48000,128);
	riffoser_track_writewav(track,"example6l.wav",48000,16);
	riffoser_wave_free(wave1);
	riffoser_track_free(track);

	// create right channel (sine wave @260hz)
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);
	wave1=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,260,50);
	riffoser_track_addwave(track,wave1,RIFFOSER_CHANNEL_RIGHT,0,3);
	riffoser_track_writeogg(track,"example6r.ogg",48000,128);
	riffoser_wave_free(wave1);
	riffoser_track_free(track);

	// now read them from .ogg files and combine into stereo track, then save to wav and ogg
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);
	wave1=riffoser_wave_readogg("example6l.ogg",100,100);
	wave2=riffoser_wave_readogg("example6r.ogg",100,100);
	riffoser_track_addwave(track,wave1,RIFFOSER_CHANNEL_LEFT,0,3);
	riffoser_track_addwave(track,wave2,RIFFOSER_CHANNEL_RIGHT,0,3);
	riffoser_track_writeogg(track,"example6.ogg",48000,160);
	riffoser_wave_free(wave1);
	riffoser_wave_free(wave2);
	riffoser_track_free(track);

}
