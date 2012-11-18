/*

	ogg I/O sanity test. create wave, save it to example9src.ogg , then load it and save as example9dst.ogg

*/

#include <stdio.h>
#include <riffoser.h>

int main() {
	struct riffoser_track *track;
	struct riffoser_wave *wave;

	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);
	wave=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,140,50);
	riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,0,5);
	riffoser_track_writeogg(track,"example9src.ogg",44100,128);
	riffoser_wave_free(wave);
	riffoser_track_free(track);

	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);
	wave=riffoser_wave_loadfromogg("example9src.ogg",100,100);
	riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,0,5);
	riffoser_track_writeogg(track,"example9dst.ogg",44100,128);
	riffoser_wave_free(wave);
	riffoser_track_free(track);

}
