#include <riffoser.h>

int main() {
	FILE *fp;
	struct riffoser_track *track;
	struct riffoser_wave *left;
	struct riffoser_wave *right;

#define testwav_stereo(filename,hz,kbps)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);\
	left=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	right=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,250,50);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,0,1.5);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,2,3.5);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,4,5.5);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,4,5.5);\
	riffoser_track_writemp3(track,"stereo_"filename".mp3_orig",hz,kbps);\
	riffoser_wave_free(left);\
	riffoser_wave_free(right);\
	riffoser_track_free(track);\
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);\
	left=riffoser_wave_readmp3("stereo_"filename".mp3_orig",100,100);\
	right=riffoser_wave_readmp3("stereo_"filename".mp3_orig",100,100);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_LEFT,0,6);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_RIGHT,0,6);\
	riffoser_track_writemp3(track,"stereo_"filename".mp3",hz,kbps);\
	riffoser_wave_free(left);\
	riffoser_wave_free(right);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define testwav_mono(filename,hz,kbps)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	left=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	right=riffoser_wave_init(RIFFOSER_WAVE_SINE,0,150,50);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_MONO,0,1);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_MONO,2,3);\
	riffoser_track_addwave(track,right,RIFFOSER_CHANNEL_MONO,3,4);\
	riffoser_track_writemp3(track,"mono_"filename".mp3_orig",hz,kbps);\
	riffoser_wave_free(left);\
	riffoser_wave_free(right);\
	riffoser_track_free(track);\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	left=riffoser_wave_readmp3("mono_"filename".mp3_orig",100,100);\
	riffoser_track_addwave(track,left,RIFFOSER_CHANNEL_MONO,0,4);\
	riffoser_track_writemp3(track,"mono_"filename".mp3",hz,kbps);\
	riffoser_wave_free(left);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define testwav(filename,hz,kbps)\
	testwav_mono(filename,hz,kbps);\
	testwav_stereo(filename,hz,kbps);

#define testhz(filename,hz)\
	testwav(filename"_96kbps",hz,96);\
	testwav(filename"_128kbps",hz,128);\
	testwav(filename"_192kbps",hz,192);\
	testwav(filename"_240kbps",hz,240);


	testhz("MP3_44100",44100);
	testhz("MP3_48000",48000);

}
