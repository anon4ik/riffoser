#include <riffoser_r.h>

struct riffoser_wavestate {
	riffoser_channel_t channel;
	riffoser_trackpos_t from;
	riffoser_trackpos_t to;
	riffoser_wavestate_state_t state;
	double samplenum;

	// for _RIFFOSER_WAVE_IO
	struct riffoser_io_struct *io;
	unsigned long bufoffset;
};

struct riffoser_instrumentstate {
	riffoser_wavestate_state_t state;
	riffoser_trackpos_t from;
	riffoser_trackpos_t to;
	double samplenum;
	riffoser_channel_t channel;
	riffoser_percent_t amplitude;
	riffoser_frequency_t frequency;
	riffoser_percent_t pitch;
};

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
		for (i=0;i<track->waves_count;i++) {
			if (track->wavestates[i]->io!=NULL)
				free(track->wavestates[i]->io);
			free(track->wavestates[i]);
		}
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
		if ((floor(RIFFOSER_RENDER___FROM(track->wavestates[i2]))==i5)&&(track->wavestates[i2]->state==RIFFOSER_WAVESTATE_IDLE)) {\
			track->wavestates[i2]->state=RIFFOSER_WAVESTATE_RENDERING;\
			if (track->waves[i2]->type==_RIFFOSER_WAVE_IO) {\
				track->wavestates[i2]->io->srcsize=chunksize_read;\
				track->wavestates[i2]->io->src=malloc(sizeof(io_src_t)*track->wavestates[i2]->io->srcsize);\
				memset(track->wavestates[i2]->io->src,0,sizeof(io_src_t)*track->wavestates[i2]->io->srcsize);\
				track->waves[i2]->readfuncs.bytes(track->wavestates[i2]->io);\
			}\
			0&&printf("%lu: wave %lu started\n",i1,i2);\
		}\
	}
#define RIFFOSER_RENDER___FROM(_v) (track->channels*io->samplerate*_v->from)
#define RIFFOSER_RENDER___TO(_v) (track->channels*io->samplerate*_v->to)
#define RIFFOSER_RENDER___WSC(_w) ((io->samplerate/_w->frequency))
#define RIFFOSER_RENDER___WPP(_w,_wp) (_wp/RIFFOSER_RENDER___WSC(_w)/1) // magic?
void riffoser_track_write(struct riffoser_track *track, struct riffoser_io_struct *io,int (*io_write_func)(struct riffoser_io_struct *io),unsigned long chunksize_read,unsigned long chunksize_write) {
	unsigned long i1,i2,i3,i4,i5,i6,i7,i8;
	long i9,dlen;
	unsigned char nomorewaves,vcount,c1;
	double fret,val;
	long long ival;
	riffoser_channel_t chan;

	dlen=ceil(track->length*track->channels*io->samplerate);

	io->channels=track->channels;
	io->src=malloc(sizeof(io_src_t)*chunksize_write);
	memset(io->src,0,sizeof(io_src_t)*chunksize_write);
	
	i3=track->waves_count;
	for (i1=0;i1<i3;i1++)
		track->wavestates[i1]->state=RIFFOSER_WAVESTATE_IDLE;
	i1=0;
	i8=0;
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
					if (track->waves[i2]->type==_RIFFOSER_WAVE_IO) {
						track->waves[i2]->readfuncs.end(track->wavestates[i2]->io);
						free(track->wavestates[i2]->io->src);
					}
					0&&printf("%lu: wave %lu finished\n",i1,i2);
				} else {
					if (track->wavestates[i2]->channel==chan) {
//						printf("correct channel\n");
						track->wavestates[i2]->samplenum++;
						if (track->wavestates[i2]->samplenum>RIFFOSER_RENDER___WSC(track->waves[i2]))
							track->wavestates[i2]->samplenum-=RIFFOSER_RENDER___WSC(track->waves[i2]);
						0&&printf("%lu: rendering wave %lu (%f-%f), samplenum is %f (%f%%)\n",i1,i2,RIFFOSER_RENDER___FROM(track->wavestates[i2]),RIFFOSER_RENDER___TO(track->wavestates[i2]),track->wavestates[i2]->samplenum,RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						RIFFOSER_WAVE_FUNC(track->waves[i2],track->wavestates[i2],RIFFOSER_RENDER___WPP(track->waves[i2],track->wavestates[i2]->samplenum));
						vcount++;
						fret=fret*track->waves[i2]->amplitude;
						if (fret>1.1)
							printf("range (0.00-1.00(+/-0.1)) overflow detected! (%f at position %f sec)\n",fret,(double)(i1/io->samplerate/io->channels));
						RIFFOSER_ENSUREBOUNDS(fret,0,1);
						
						val=sqrt(pow(val,1) + pow(fret,2) + 2 * val * fret)/*/pow(vcount+1,2)*pow(track->waves[i2]->channels,2)*//2;
						if (val>=1)
							printf("val>=1 %f\n",val);
						RIFFOSER_ENSUREBOUNDS(val,0,1);
					}
					else {
//						printf("wrong channel ( %u / %u )\n",track->wavestates[i2]->channel,chan);
					}
					c1=1;
				}
			}
		}
		if (val>=1)
			printf("(2) val>=1 %f\n",val);
		RIFFOSER_ENSUREBOUNDS(val,0,1);
		i4=i1-i8;
		if (i4<chunksize_write)
			io->src[i4]=(io_src_t)val;
		else {
			io->srcsize=chunksize_write;
			io_write_func(io);
			memset(io->src,0,sizeof(io_src_t)*chunksize_write);
			io->src[0]=(io_src_t)val;
			i8=i1;
		}
		
/*		if ((c1==0)) {
			if ((i3!=track->waves_count)) {
				0&&printf("%lu: warping to position %lu\n",i1,i5);
				i4=floor((i5-i1+chan)/chunksize_write);
				while (i4>0) {
					io->srcsize=chunksize_write;
					io_write_func(io);
					memset(io->src,0,sizeof(io_src_t)*chunksize_write);
					i4--;
				}
				i1=i5;
				RIFFOSER_RENDER___INITWAVES;
				i3=track->waves_count;
			}
			else {
				break;
			}
		}*/
		if ((c1==0)&&(i3==track->waves_count))
//		if (i1>=dlen)
			break;
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
	io->srcsize=i1-i8;
	io_write_func(io);
	free(io->src);
	
}

extern int riffoser_ogg_write_bytes(struct riffoser_io_struct *io);

void riffoser_track_writeogg(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_kbps_t kbps) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	io->samplerate=samplerate;
	io->kbps=kbps;
	io->channels=track->channels;
	io->datalen=ceil((double)(track->length*io->channels*io->samplerate));
	
	if (!riffoser_ogg_write_start(io)) {
		riffoser_track_write(track,io,riffoser_ogg_write_bytes,CHUNKSIZE_OGG_READ,CHUNKSIZE_OGG_WRITE);
		riffoser_ogg_write_end(io);
	}
	else printf("Error writing ogg\n");
	
	free(io);
}

extern int riffoser_mp3_write_bytes(struct riffoser_io_struct *io);

void riffoser_track_writemp3(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_kbps_t kbps) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->channels=track->channels;
	io->filename=filename;
	io->kbps=kbps;
	io->samplerate=samplerate;
	io->datalen=ceil((double)(track->length*io->channels*io->samplerate));

	if (!riffoser_mp3_write_start(io)) {
		riffoser_track_write(track,io,riffoser_mp3_write_bytes,CHUNKSIZE_MP3_READ,CHUNKSIZE_MP3_WRITE);
		riffoser_mp3_write_end(io);
	}
	else printf("Error writing mp3\n");

	free(io);
}

extern int riffoser_wav_write_bytes(struct riffoser_io_struct *io);

void riffoser_track_writewav(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample) {
	struct riffoser_io_struct *io;
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	io->samplerate=samplerate;
	io->bytespersample=bitspersample/8;
	io->channels=track->channels;
	io->datalen=ceil((double)(track->length*io->channels*io->samplerate));

	riffoser_wav_write_start(io);
	riffoser_track_write(track,io,riffoser_wav_write_bytes,CHUNKSIZE_WAV_READ,CHUNKSIZE_WAV_WRITE);
	riffoser_wav_write_end(io);

	free(io);
	
}

void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to) {
	//printf("riffoser_track_addwave(track,wave,%u,%f,%f)\n",channel,from,to);
	riffoser_tracklen_t tracklength;
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
	if (wave->type==_RIFFOSER_WAVE_IO) {
		track->wavestates[track->waves_count]->io=malloc(sizeof(struct riffoser_io_struct));
		memset(track->wavestates[track->waves_count]->io,0,sizeof(struct riffoser_io_struct));
		track->wavestates[track->waves_count]->io->filename=track->waves[track->waves_count]->filename;
		track->waves[track->waves_count]->readfuncs.start(track->wavestates[track->waves_count]->io);
//		printf("datalen=%lu\n",track->wavestates[track->waves_count]->io->datalen);
		tracklength=(riffoser_tracklen_t)((riffoser_tracklen_t)track->wavestates[track->waves_count]->io->datalen/track->wavestates[track->waves_count]->io->channels/track->wavestates[track->waves_count]->io->samplerate);
//		track->waves[track->waves_count]->frequency=tracklength/(riffoser_frequency_t)(riffoser_frequency_t)track->waves[track->waves_count]->channels/(riffoser_frequency_t)track->wavestates[track->waves_count]->io->samplerate);
		track->waves[track->waves_count]->frequency=track->waves[track->waves_count]->lengthscale/tracklength;
//		printf("frequency=%f tracklen=%f\n",track->waves[track->waves_count]->frequency,tracklength);
//		printf("bps=%u channels=%u kbps=%u samplerate=%u tracklen=%f datalen=%lu frequency=%f\n",track->wavestates[track->waves_count]->io->bytespersample,track->wavestates[track->waves_count]->io->channels,track->wavestates[track->waves_count]->io->kbps,track->wavestates[track->waves_count]->io->samplerate,tracklength,track->wavestates[track->waves_count]->io->datalen,track->waves[track->waves_count]->frequency);
	}
	
	track->waves_count++;
	
	if (to>track->length)
		track->length=to;
}

