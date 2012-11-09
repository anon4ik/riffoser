#include <riffoser_r.h>

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
	if (track->instruments_count>0) {
		for (i=0;i<track->instruments_count;i++)
			free(track->instrumentstates[i]);
		free(track->instrumentstates);
		free(track->instruments);
	}
	free(track);
}

#define RIFFOSER_RENDER___INITWAVES \
	for (i2=0;i2<track->waves_count;i2++) {\
		if (floor(RIFFOSER_RENDER___FROM(track->wavestates[i2]))==i5) {\
			track->wavestates[i2]->state=RIFFOSER_WAVESTATE_RENDERING;\
			0&&printf("%lu: wave %lu started\n",i1,i2);\
		}\
	}
#define RIFFOSER_RENDER___FROM(_v) (track->channels*io->samplerate*_v->from)
#define RIFFOSER_RENDER___TO(_v) (track->channels*io->samplerate*_v->to)
#define RIFFOSER_RENDER___WSC(_w) ((1*io->samplerate/_w->frequency))
#define RIFFOSER_RENDER___WPP(_w,_wp) (_wp*100/RIFFOSER_RENDER___WSC(_w))
void riffoser_track_preparesrc(struct riffoser_track *track, struct riffoser_io_struct *io) {
	unsigned long i1,i2,i3,i4,i5,i6,i7;
	unsigned char nomorewaves,vcount,c1;
	double fret,val;
	long long ival;
	riffoser_channel_t chan;
	
	io->channels=track->channels;
	io->srcsize=track->channels*io->samplerate*io->bytespersample*track->length;
	io->src=malloc(io->srcsize);
	memset(io->src,0,io->srcsize);
	
	i3=track->waves_count;
	for (i1=0;i1<i3;i1++)
		track->wavestates[i1]->state=RIFFOSER_WAVESTATE_IDLE;
	i1=0;
	i6=0;
	nomorewaves=0;
	while (1) {
		chan=i1%track->channels;
		if (i3==track->waves_count&&!nomorewaves) {
			i5=i1;
			0&&printf("%lu: finding nearest wave...\n",i1);
			for (i2=0;i2<track->waves_count;i2++){
				if ((track->wavestates[i2]->state==RIFFOSER_WAVESTATE_IDLE)&&(((i3==track->waves_count))||(RIFFOSER_RENDER___FROM(track->wavestates[i2])<=i5))&&(RIFFOSER_RENDER___FROM(track->wavestates[i2])>=i6)) {
					0&&printf("%lu: found wave %lu with from %f\n",i1,i2,RIFFOSER_RENDER___FROM(track->wavestates[i2]));
					i3=i2;
					i5=RIFFOSER_RENDER___FROM(track->wavestates[i2]);
				}
				else {
					0&&printf("%lu: skipping wave %lu\n",i1,i2);
				}
			}
			if (i3!=track->waves_count)
				0&&printf("next wave will be %lu at %f\n",i3,RIFFOSER_RENDER___FROM(track->wavestates[i3]));
			else nomorewaves=1;
		}
		c1=0;
		val=0;
		vcount=0;
		for (i2=0;i2<track->waves_count;i2++){
			if (track->wavestates[i2]->state==RIFFOSER_WAVESTATE_RENDERING) {
				if (RIFFOSER_RENDER___TO(track->wavestates[i2])<i1) {
					track->wavestates[i2]->state=RIFFOSER_WAVESTATE_FINISHED;
					0&&printf("%lu: wave %lu finished\n",i1,i2);
				} else {
					if (track->wavestates[i2]->channel==chan) {
//						printf("correct channel\n");
						track->wavestates[i2]->samplenum++;
						if (track->wavestates[i2]->samplenum>RIFFOSER_RENDER___WSC(track->waves[i2]))
							track->wavestates[i2]->samplenum-=RIFFOSER_RENDER___WSC(track->waves[i2]);
						0&&printf("%lu: rendering wave %lu (%f-%f), samplenum is %f (%f%%)\n",i1,i2,RIFFOSER_RENDER___FROM(track->wavestates[i2]),RIFFOSER_RENDER___TO(track->wavestates[i2]),track->wavestates[i2]->samplenum,RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						RIFFOSER_WAVE_FUNC(track->waves[i2],RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						vcount++;
						fret=fret*track->waves[i2]->amplitude/100/pow(vcount+1,2);
						if (fret>99)
							printf("fret>99 %f\n",fret);
						RIFFOSER_ENSUREBOUNDS(fret,0,99);
						
						val=sqrt(pow(val,2) + pow(fret,2) + 2 * val * fret)/pow(vcount+1,2)*pow(track->waves[i2]->channels,2);
						if (val>99)
							printf("val>99 %f\n",val);
						RIFFOSER_ENSUREBOUNDS(val,0,99);
					}
					else {
//						printf("wrong channel ( %u / %u )\n",track->wavestates[i2]->channel,chan);
					}
					c1=1;
				}
			}
		}
		val*=vcount;
		RIFFOSER_ENSUREBOUNDS(val,0,99);
		if (val>100)
			printf("(2) val>100 %f\n",val);
		if (io->bytespersample==1) {
			ival=round(val*2.56);
			if (ival>255)
				ival=255;
			if (i1<io->srcsize)
				((unsigned char *)io->src)[i1]=(unsigned char)ival;
		}
		else if (io->bytespersample==2) {
			ival=round(val*655.36)-32768;
			if (ival>32767)
				ival=32767;
			if (i1*2<io->srcsize)
				((short *)io->src)[i1]=(short)ival;
		}
		else if (io->bytespersample==4) {
			ival=round(val*42949672.96)-2147483648;
			if (ival>2147483647)
				ival=2147483647;
			if (i1*4<io->srcsize)
				((int *)io->src)[i1]=(int)ival;
		}
		//riffoser_writeint(bytespersample,ival);
		if ((c1==0)) {
			if ((i3!=track->waves_count)) {
				0&&printf("%lu: warping to position %lu\n",i1,i5);
				i4=(i5-i1+chan)*io->bytespersample;
/*				skipbuf=malloc(i4);
				memset(skipbuf,0,i4);
				riffoser_writebuf(i4,skipbuf);
				free(skipbuf);*/
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
//		printf("%u\n",i1);
	//usleep(100000);
	}
	
}

void riffoser_track_writeogg(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample,riffoser_kbps_t kbps) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	io->samplerate=samplerate;
	io->bytespersample=bitspersample/8;
	io->kbps=kbps;
	io->channels=track->channels;
	
	riffoser_track_preparesrc(track,io);
	riffoser_ogg_savetofile(io);
	
	free(io->src);
	free(io);
}

void riffoser_track_writemp3(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample,riffoser_kbps_t kbps) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->bytespersample=bitspersample*8;
	io->channels=track->channels;
	io->filename=filename;
	io->kbps=kbps;
	io->samplerate=samplerate;
	
	riffoser_track_preparesrc(track,io);
	riffoser_mp3_savetofile(io);
	
	free(io);
}

void riffoser_track_writewav(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	io->samplerate=samplerate;
	io->bytespersample=bitspersample/8;
	
	riffoser_track_preparesrc(track,io);
	riffoser_wav_savetofile(io);
	
	free(io->src);
	free(io);
}

void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to) {
	if (track->waves_count==0) {
		track->waves=malloc(sizeof(struct riffoser_wave *));
		track->wavestates=malloc(sizeof(struct riffoser_wavestate *));
	} else {
		track->waves=realloc(track->waves,sizeof(struct riffoser_wave *)*(track->waves_count+1));
		track->wavestates=realloc(track->wavestates,sizeof(struct riffoser_wavestate *)*(track->waves_count+1));
	}
	RIFFOSER_ENSUREBOUNDS(channel,0,track->channels-1);
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
