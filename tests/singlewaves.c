#include <riffoser.h>

int main() {
	struct riffoser_track *track;
	struct riffoser_wave *wave;

#define writewave(wavetype,volume,frequency,pitch,filename)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	wave=riffoser_wave_init(wavetype,volume,frequency,pitch);\
	riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,0,5);\
	riffoser_track_writewav(track,filename"_8000hz_8bit.wav",8000,8);\
	riffoser_track_writewav(track,filename"_8000hz_16bit.wav",8000,16);\
	riffoser_track_writewav(track,filename"_8000hz_32bit.wav",8000,32);\
	riffoser_track_writewav(track,filename"_11025hz_8bit.wav",11025,8);\
	riffoser_track_writewav(track,filename"_11025hz_16bit.wav",11025,16);\
	riffoser_track_writewav(track,filename"_11025hz_32bit.wav",11025,32);\
	riffoser_track_writewav(track,filename"_22050hz_8bit.wav",22050,8);\
	riffoser_track_writewav(track,filename"_22050hz_16bit.wav",22050,16);\
	riffoser_track_writewav(track,filename"_22050hz_32bit.wav",22050,32);\
	riffoser_track_writewav(track,filename"_44100hz_8bit.wav",44100,8);\
	riffoser_track_writewav(track,filename"_44100hz_16bit.wav",44100,16);\
	riffoser_track_writewav(track,filename"_44100hz_32bit.wav",44100,32);\
	riffoser_track_writewav(track,filename"_48000hz_8bit.wav",48000,8);\
	riffoser_track_writewav(track,filename"_48000hz_16bit.wav",48000,16);\
	riffoser_track_writewav(track,filename"_48000hz_32bit.wav",48000,32);\
	riffoser_track_writewav(track,filename"_96000hz_8bit.wav",96000,8);\
	riffoser_track_writewav(track,filename"_96000hz_16bit.wav",96000,16);\
	riffoser_track_writewav(track,filename"_96000hz_32bit.wav",96000,32);\
	riffoser_track_writewav(track,filename"_192000hz_8bit.wav",192000,8);\
	riffoser_track_writewav(track,filename"_192000hz_16bit.wav",192000,16);\
	riffoser_track_writewav(track,filename"_192000hz_32bit.wav",192000,32);\
	riffoser_wave_free(wave);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define writewavetype(wavetype,filename)\
	writewave(wavetype,100,150,50,filename"_v100_hz150_p50");\
	writewave(wavetype,50,150,50,filename"_v50_hz150_p50");\
	writewave(wavetype,100,250,50,filename"_v100_hz250_p50");\
	writewave(wavetype,100,150,50,filename"_v100_hz150_p10");\
	writewave(wavetype,100,150,50,filename"_v100_hz150_p90");

	writewavetype(RIFFOSER_WAVE_SQUARE,"SQUARE");
	writewavetype(RIFFOSER_WAVE_TRIANGLE,"TRIANGLE");
	writewavetype(RIFFOSER_WAVE_SINE,"SINE");


	return (EXIT_SUCCESS);
}
