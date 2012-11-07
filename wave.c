#include <riffoser_r.h>

struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t type,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch) {
	struct riffoser_wave * wave;
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	wave->type=type;
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->amplitude=amplitude;
	wave->frequency=frequency;
	wave->channels=1;
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
	
	if ((fp=fopen(filename,"rb"))==NULL) {
		printf("failed to open file %s for reading!\n",filename);
		return NULL;
	}
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

