#include <riffoser.h>

int main() {
	FILE *fp;
	struct riffoser_track *track;
	struct riffoser_wave *left;
	struct riffoser_wave *right;

#define testwav_stereo(filename,hz,bitrate)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);\
	left=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	right=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,250,50);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,0,2);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,2,4);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,4,6);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,4,6);\
	riffoser_track_writewav(track,"stereo_"filename".wav_orig",hz,bitrate);\
	riffoser_wave_free(left);\
	riffoser_wave_free(right);\
	riffoser_track_free(track);\
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);\
	left=riffoser_wave_readwav("stereo_"filename".wav_orig",100,100);\
	right=riffoser_wave_readwav("stereo_"filename".wav_orig",100,100);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,0,6);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,0,6);\
	riffoser_track_writewav(track,"stereo_"filename".wav",hz,bitrate);\
	riffoser_wave_free(left);\
	riffoser_wave_free(right);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define testwav_mono(filename,hz,bitrate)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	left=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_MONO,0,4);\
	riffoser_track_writewav(track,"mono_"filename".wav_orig",hz,bitrate);\
	riffoser_wave_free(left);\
	riffoser_track_free(track);\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	left=riffoser_wave_readwav("mono_"filename".wav_orig",100,100);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_MONO,0,4);\
	riffoser_track_writewav(track,"mono_"filename".wav",hz,bitrate);\
	riffoser_wave_free(left);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define testwav(filename,hz,bitrate)\
	testwav_mono(filename,hz,bitrate);\
	testwav_stereo(filename,hz,bitrate);

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
