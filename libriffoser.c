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
	if (track->instruments_count>0) {
		for (i=0;i<track->instruments_count;i++)
			free(track->instrumentstates[i]);
		free(track->instrumentstates);
		free(track->instruments);
	}
	free(track);
}

#define RIFFOSER_ENSUREBOUNDS(v,min,max) {\
	if (v<min)\
		v=min;\
	else if (v>max)\
		v=max;\
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
		if (floor(RIFFOSER_RENDER___FROM(track->wavestates[i2]))==i5) {\
			track->wavestates[i2]->state=RIFFOSER_WAVESTATE_RENDERING;\
			0&&printf("%lu: wave %lu started\n",i1,i2);\
		}\
	}
#define RIFFOSER_RENDER___FROM(_v) (track->channels*samplerate*_v->from)
#define RIFFOSER_RENDER___TO(_v) (track->channels*samplerate*_v->to)
#define RIFFOSER_RENDER___WSC(_w) ((1*samplerate/_w->frequency))
#define RIFFOSER_RENDER___WPP(_w,_wp) (_wp*100/RIFFOSER_RENDER___WSC(_w))
void riffoser_track_writewav(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample) {
	FILE * fp;
	unsigned long i1,i2,i3,i4,i5,i6,fpi;
	unsigned char c1,bytespersample;
	unsigned char * skipbuf;
	float fret,val,phi1,phi2;
	long ival;
	unsigned char nomorewaves,vcount;
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
						fret=fret*track->waves[i2]->amplitude/200;
						RIFFOSER_ENSUREBOUNDS(fret,0,99);
						
						// can be improved somehow
//						phi1 = ((360° * dt) / track->wavestates[i2]->samplenum);
						val=sqrt(val + pow(fret,2) + 2 * val * fret)/(val!=0?2:1);
//						val=(val*vcount+fret)/(++vcount);
//						val+=fret;
						RIFFOSER_ENSUREBOUNDS(val,0,99);
					}
					else {
//						printf("wrong channel ( %u / %u )\n",track->wavestates[i2]->channel,chan);
					}
					c1=1;
				}
			}
		}
		if (bytespersample==1) {
			ival=round(val*2.56);
			if (ival>255)
				ival=255;
		}
		else if (bytespersample==2) {
			ival=round(val*655.36)-32768;
			if (ival>32767)
				ival=32767;
		}
		else if (bytespersample==4) {
			ival=round(val*42949672.96)-21474836.48;
			if (ival>2147483647)
				ival=2147483647;
		}
		riffoser_writeint(bytespersample,ival);
		if ((c1==0)) {
			if ((i3!=track->waves_count)) {
				0&&printf("%lu: warping to position %lu\n",i1,i5);
				i4=(i5-i1+chan)*bytespersample;
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
//		printf("%u\n",i1);
	//usleep(100000);
	}
	
	fclose(fp);
}



struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t type,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch) {
	struct riffoser_wave * wave;
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	wave->type=type;
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->amplitude=amplitude;
	wave->frequency=frequency;
	RIFFOSER_ENSUREBOUNDS(pitch,0,99);
	wave->pitch=pitch;
	return wave;
}

#define riffoser_readstr(arg,size) {\
	fread(arg,size,1,fp);\
	arg[size]='\0';\
}
#define riffoser_readint(arg,padding) {\
	fread(&arg,padding,1,fp);\
}
#define riffoser_readbuf(buf,size) {\
	fread(buf,size,1,fp);\
}
struct riffoser_wave * riffoser_wave_loadfromwav(char * filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	FILE * fp;
	struct riffoser_wave * wave;
	char * tmps;
	unsigned int dlen,tmpi,samplerate,bitspersample,bytespersample;
	unsigned short tmpw;
	
	char *buf;
	
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->type=_RIFFOSER_WAVE_DATA;
	wave->amplitude=amplitude;
	wave->pitch=50;
	
	fp=fopen(filename,"rb");
	tmps=malloc(5);
	riffoser_readstr(tmps,4);
	
	if (!strcmp(tmps,"RIFF")) { // its RIFF
//		printf("%s\n",tmps);
		
		riffoser_readint(dlen,4);
		
		free(tmps);
		tmps=malloc(9);
		riffoser_readstr(tmps,8);
		
		if (!strcmp(tmps,"WAVEfmt ")) { // its WAV
		
			riffoser_readint(tmpi,4);
			
			if (tmpi==16) { // its PCM
			
				riffoser_readint(tmpw,2);
				
				if (tmpw==1) { // there's no compression
				
					
					riffoser_readint(wave->channels,2);
					
					riffoser_readint(samplerate,4);
					
					riffoser_readint(tmpi,4);
					riffoser_readint(tmpw,2);
					
					riffoser_readint(bitspersample,2);
					bytespersample=bitspersample/8;
					
					free(tmps);
					tmps=malloc(5);
					riffoser_readstr(tmps,4);
					
					if (!strcmp(tmps,"data")) { // header was read correctly
						riffoser_readint(tmpi,4);
						buf=malloc(tmpi);
						wave->frequency=length*1/(riffoser_frequency_t)(tmpi/(riffoser_frequency_t)wave->channels/(riffoser_frequency_t)samplerate/(riffoser_frequency_t)bytespersample)/100;
						wave->data_count=tmpi/bytespersample;
						wave->data=malloc(sizeof(double)*wave->data_count);
						
						riffoser_readbuf(buf,tmpi);
						
						for (tmpi=0;tmpi<wave->data_count;tmpi++) {
							if (bytespersample==1)
								wave->data[tmpi]=(riffoser_data_t)(((((unsigned char *)buf)[tmpi]))*amplitude/256);
							else if (bytespersample==2)
								wave->data[tmpi]=(riffoser_data_t)(((((short *)buf)[tmpi])+32768)*amplitude*2/65536);
							else if (bytespersample==4)
								wave->data[tmpi]=(riffoser_data_t)((riffoser_data_t)((((int *)buf)[tmpi])+2147483648)*amplitude/4294967296);
						}
						
						free(buf);
						
//						printf("%u %f %u\n",wave->data_count,wave->frequency,wave->data_count);
						
//						printf("%u %s %u %u %u %u %u %s %lu %f\n",dlen,tmps,tmpi,wave->channels,samplerate,bitspersample,bytespersample,tmps,wave->data_count,wave->frequency);
					}
				}
			
			}
		
		}
			
		
	}
	
	free(tmps);
	
	fclose(fp);
	
	return wave;
}

void riffoser_wave_free(struct riffoser_wave * wave) {
	unsigned int i;
	if (wave->data!=NULL) {
		free(wave->data);
	}
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

struct riffoser_instrument * riffoser_instrument_init() {
	struct riffoser_instrument * instrument;
	instrument=malloc(sizeof(struct riffoser_instrument));
	memset(instrument,0,sizeof(struct riffoser_instrument));
	return instrument;
}

void riffoser_instrument_free(struct riffoser_instrument * instrument) {
	free(instrument);
}