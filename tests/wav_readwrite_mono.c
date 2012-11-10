#include <riffoser.h>

int main() {
	FILE *fp;
	struct riffoser_track *track;
	struct riffoser_wave *wave;

#define testwav(filename,hz,bitrate)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	wave=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,0,4);\
	riffoser_track_writewav(track,filename".wav_orig",hz,bitrate);\
	riffoser_wave_free(wave);\
	riffoser_track_free(track);\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	wave=riffoser_wave_loadfromwav(filename".wav_orig",100,100);\
	riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,0,4);\
	riffoser_track_writewav(track,filename".wav",hz,bitrate);\
	riffoser_wave_free(wave);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define testhz(filename,hz)\
	testwav(filename"_8",hz,8);\
	testwav(filename"_16",hz,16);\
	testwav(filename"_32",hz,32);

	testhz("WAV_8000",8000);\
	testhz("WAV_11025",11025);\
	testhz("WAV_22050",22050);\
	testhz("WAV_44100",44100);\
	testhz("WAV_48000",48000);\
	testhz("WAV_96000",96000);\
	testhz("WAV_192000",192000);

}
