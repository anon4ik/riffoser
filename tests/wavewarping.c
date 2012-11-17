#include <riffoser.h>

int main() {
	struct riffoser_track *track;
	struct riffoser_wave *wave;

	float i,ii;

#define warptestmono(bps,samplerate,periods,filename)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_MONO);\
	wave=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	i=periods;\
	ii=0;\
	while (ii<5) {\
		riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_MONO,ii,ii+i/2);\
		ii+=i;\
	}\
	riffoser_track_writewav(track,filename".wav",samplerate,bps);\
	riffoser_wave_free(wave);\
	riffoser_track_free(track);\
	fprintf(stdout,".");\
	fflush(stdout);

#define warpteststereo(bps,samplerate,periods,filename)\
	track=riffoser_track_init(RIFFOSER_CHANNELS_STEREO);\
	wave=riffoser_wave_init(RIFFOSER_WAVE_SINE,100,150,50);\
	i=periods;\
	ii=0;\
	while (ii<5) {\
		riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_LEFT,ii,ii+i/2);\
		riffoser_track_addwave(track,wave,RIFFOSER_CHANNEL_RIGHT,ii+i/2,ii+i);\
		ii+=i;\
	}\
	riffoser_track_writewav(track,filename".wav",samplerate,bps);\
	riffoser_wave_free(wave);\
	riffoser_track_free(track);\
	printf(".");


#define warptestms(bps,samplerate,periods,filename)\
	warptestmono(bps,samplerate,periods,"mono_"filename);\
	warpteststereo(bps,samplerate,periods,"stereo_"filename);

#define warptestbps(samplerate,periods,filename)\
	warptestms(8,samplerate,periods,filename"_8bps");\
	warptestms(16,samplerate,periods,filename"_16bps");\
	warptestms(32,samplerate,periods,filename"_32bps");

#define warptest(periods,filename)\
	warptestbps(8000,periods,filename"_8000hz");\
	warptestbps(11025,periods,filename"_11025hz");\
	warptestbps(22050,periods,filename"_22050hz");\
	warptestbps(44100,periods,filename"_44100hz");\
	warptestbps(48000,periods,filename"_48000hz");\
	warptestbps(96000,periods,filename"_96000hz");\
	warptestbps(192000,periods,filename"_192000hz");


	warptest(2.5,"2.5");
	warptest(1,"1");
	warptest(0.7,"0.7");
	warptest(0.3,"0.3");
	warptest(0.1,"0.1");

	return (EXIT_SUCCESS);
	
}
