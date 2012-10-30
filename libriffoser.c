#include <riffoser.h>

struct riffoser_track * riffoser_track_init(riffoser_channel_t channels) {
	struct riffoser_track * track;
	track=malloc(sizeof(struct riffoser_track));
	memset(track,0,sizeof(struct riffoser_track));
	track->channels=channels;
	return track;
}

void riffoser_track_free(struct riffoser_track * track) {
	unsigned long i;
	if (track->waves_count>0) {
		for (i=0;i<track->waves_count;i++)
			free(track->wavestates[i]);
		free(track->wavestates);
		free(track->waves);
	}
	free(track);
}

#define riffoser_writestr(value) {\
	fprintf(fp,value);\
}
#define riffoser_writeint(padding,value) {\
	fpi=value;\
	fwrite((const void *)&fpi,padding,1,fp);\
}
#define riffoser_writebuf(size,buf) {\
	fwrite(buf,size,1,fp);\
}
#define RIFFOSER_RENDER___INITWAVES \
	for (i2=0;i2<track->waves_count;i2++) {\
		if (RIFFOSER_RENDER___FROM(track->wavestates[i2])==i5) {\
			track->wavestates[i2]->state=RIFFOSER_WAVESTATE_RENDERING;\
			1&&printf("%lu: wave %lu started\n",i1,i2);\
		}\
	}
#define RIFFOSER_RENDER___FROM(_v) (track->channels*samplerate*_v->from)
#define RIFFOSER_RENDER___TO(_v) (track->channels*samplerate*_v->to)
// wave samples count (wave)
#define RIFFOSER_RENDER___WSC(_w) ((1*samplerate/_w->frequency))
// wave pos percent
#define RIFFOSER_RENDER___WPP(_w,_wp) (_wp*100/RIFFOSER_RENDER___WSC(_w))
void riffoser_track_writeriff(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample) {
	FILE * fp;
	unsigned long i1,i2,i3,i4,i5,i6,fpi;
	unsigned char c1,bytespersample;
	unsigned char * skipbuf;
	float fret,val;
	long ival;
	unsigned char nomorewaves;
	riffoser_channel_t chan;
	bytespersample=bitspersample/8;
	fp=fopen(filename,"wb");
	i1=track->channels*samplerate*bytespersample*track->length;
	i2=4+24+8+i1+(i1%2>0?1:0);
	riffoser_writestr("RIFF");
	riffoser_writeint(4,4+24+8+i1+(i1%2>0?1:0));
	riffoser_writestr("WAVEfmt ");
	riffoser_writeint(4,16);
	riffoser_writeint(2,1);
	riffoser_writeint(2,track->channels);
	riffoser_writeint(4,samplerate);
	riffoser_writeint(4,bytespersample*track->channels*samplerate);
	riffoser_writeint(2,bytespersample*track->channels);
	riffoser_writeint(2,bytespersample*8);
	riffoser_writestr("data");
	riffoser_writeint(4,i1);
	if (i1%2>0)
		riffoser_writeint(1,0);
	
	i1=0;
	i3=track->waves_count;
	i6=0;
	nomorewaves=0;
	while (1) {
		chan=i1%track->channels;
		if (i3==track->waves_count&&!nomorewaves) {
			i5=i1;
			1&&printf("%lu: finding nearest wave...\n",i1);
			for (i2=0;i2<track->waves_count;i2++){
				if ((track->wavestates[i2]->state==RIFFOSER_WAVESTATE_IDLE)&&(((i3==track->waves_count))||(RIFFOSER_RENDER___FROM(track->wavestates[i2])<=i5))&&(RIFFOSER_RENDER___FROM(track->wavestates[i2])>=i6)) {
					1&&printf("%lu: found wave %lu with from %f\n",i1,i2,RIFFOSER_RENDER___FROM(track->wavestates[i2]));
					i3=i2;
					i5=RIFFOSER_RENDER___FROM(track->wavestates[i2]);
				}
				else {
					1&&printf("%lu: skipping wave %lu\n",i1,i2);
				}
			}
			if (i3!=track->waves_count)
				0&&printf("next wave will be %lu at %f\n",i3,RIFFOSER_RENDER___FROM(track->wavestates[i3]));
			else nomorewaves=1;
		}
		c1=0;
		val=0;
		for (i2=0;i2<track->waves_count;i2++){
			if (track->wavestates[i2]->state==RIFFOSER_WAVESTATE_RENDERING) {
				if (RIFFOSER_RENDER___TO(track->wavestates[i2])<i1) {
					track->wavestates[i2]->state=RIFFOSER_WAVESTATE_FINISHED;
					1&&printf("%lu: wave %lu finished\n",i1,i2);
				} else {
					if (track->wavestates[i2]->channel==chan) {
//						printf("correct channel\n");
						track->wavestates[i2]->samplenum++;
						if (track->wavestates[i2]->samplenum>RIFFOSER_RENDER___WSC(track->waves[i2]))
							track->wavestates[i2]->samplenum-=RIFFOSER_RENDER___WSC(track->waves[i2]);
						0&&printf("%lu: rendering wave %lu (%f-%f), samplenum is %f (%f%%)\n",i1,i2,RIFFOSER_RENDER___FROM(track->wavestates[i2]),RIFFOSER_RENDER___TO(track->wavestates[i2]),track->wavestates[i2]->samplenum,RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						RIFFOSER_WAVE_FUNC(track->waves[i2],RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						fret=fret*track->waves[i2]->amplitude/100;
						// replace for now
						val=fret;
					}
					else {
//						printf("wrong channel ( %u / %u )\n",track->wavestates[i2]->channel,chan);
					}
					c1=1;
				}
			}
		}
		// convert val to needed format and save
		if (bytespersample==1) {
			ival=round(val*2.56)-128;
			if (ival>127)
				ival=127;
		}
		else if (bytespersample==2) {
			ival=round(val*655.36);
			if (ival>65535)
				ival=65535;
		}
		else if (bytespersample==4) {
			ival=round(val*42949672.96);
			if (ival>4294967295)
				ival=4294967295;
		}
		riffoser_writeint(bytespersample,ival);
		if (c1==0) {
			if (i3!=track->waves_count) {
				1&&printf("%lu: warping to position %lu\n",i1,i5);
				i4=(i5-i1)*bytespersample;
				skipbuf=malloc(i4);
				memset(skipbuf,0,i4);
				riffoser_writebuf(i4,skipbuf);
				free(skipbuf);
				i1=i5;
				RIFFOSER_RENDER___INITWAVES;
				i3=track->waves_count;
			}
			else {
				break;
			}
		}
		else {
			if (i5==i1) {
				RIFFOSER_RENDER___INITWAVES;
				i6=i5+1;
				i3=track->waves_count;
			}
			i1++;
		}
	//usleep(100000);
	}
	
	//riffoser_writebuf(i1,_wavesound->data);
	fclose(fp);
}



struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t type,riffoser_amplitude_t amplitude,riffoser_frequency_t frequency,riffoser_pitch_t pitch) {
	struct riffoser_wave * wave;
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	wave->type=type;
	wave->amplitude=amplitude;
	wave->frequency=frequency;
	wave->pitch=pitch;
	return wave;
}

void riffoser_wave_free(struct riffoser_wave * wave) {
	free(wave);
}

void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to) {
	if (track->waves_count==0) {
		track->waves=malloc(sizeof(struct riffoser_wave *));
		track->wavestates=malloc(sizeof(struct riffoser_wavestate *));
	} else {
		track->waves=realloc(track->waves,sizeof(struct riffoser_wave *)*(track->waves_count+1));
		track->wavestates=realloc(track->wavestates,sizeof(struct riffoser_wavestate *)*(track->waves_count+1));
	}
	track->waves[track->waves_count]=wave;
	track->wavestates[track->waves_count]=malloc(sizeof(struct riffoser_wavestate));
	memset(track->wavestates[track->waves_count],0,sizeof(struct riffoser_wavestate));
	track->wavestates[track->waves_count]->from=from;
	track->wavestates[track->waves_count]->to=to;
	track->wavestates[track->waves_count]->channel=channel;
	
	track->waves_count++;
	
	if (to>track->length)
		track->length=to;
}



