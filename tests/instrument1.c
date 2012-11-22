#include <riffoser.h>

int main() {
	struct riffoser_track *track;
	struct riffoser_instrument *instrument;
	struct riffoser_wave *wave;

	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);
	instrument=riffoser_instrument_init(RIFFOSER_CHANNELS_MONO);
	wave=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);

	riffoser_instrument_addwave(instrument,wave,RIFFOSER_CHANNEL_MONO,0,3);
	riffoser_instrument_play(track,instrument,RIFFOSER_CHANNEL_MONO,0,3);

	riffoser_track_writewav(track,"test.wav",48000,16);

	riffoser_wave_free(wave);
	riffoser_instrument_free(instrument);
	riffoser_track_free(track);


	return (EXIT_SUCCESS);
}
