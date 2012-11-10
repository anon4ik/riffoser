#include <riffoser_r.h>

int riffoser_wav_savetofile(struct riffoser_io_struct *io) {
	FILE *fp;
	unsigned long i,pow256,fpi;
	unsigned long len;

	long v;

	char *buf;

	fp=fopen(io->filename,"wb");

	len=io->srcsize*io->bytespersample;

	i=4+24+8+len+(io->srcsize%2>0?1:0);
	riffoser_writestr("RIFF");
	riffoser_writeint(4,i);
	riffoser_writestr("WAVEfmt ");
	riffoser_writeint(4,16);
	riffoser_writeint(2,1);
	riffoser_writeint(2,io->channels);
	riffoser_writeint(4,io->samplerate);
	riffoser_writeint(4,io->bytespersample*io->channels*io->samplerate);
	riffoser_writeint(2,io->bytespersample*io->channels);
	riffoser_writeint(2,io->bytespersample*8);
	riffoser_writestr("data");
	riffoser_writeint(4,len);
	if (len%2>0)
		riffoser_writeint(1,0);

	buf=malloc(len);

	pow256=pow(256,io->bytespersample);
//	printf("POW=%lu\n",pow256);
	for (i=0;i<io->srcsize;i++) {
		v=floor(((io_src_t *)io->src)[i]*pow256/*-(io->bytespersample>1?pow256/2:1)*/);
		if (v>=pow256)
			printf("%lu >= %lu\n",v,pow256);
		else if (io->src[i]>=1)
			printf("%f >= 1\n",io->src[i]);
		else if (io->src[i]<0)
			printf("%f < 0\n",io->src[i]);
		else if (io->bytespersample==1)
			((unsigned char *)buf)[i]=v;
		else if (io->bytespersample==2)
			((short *)buf)[i]=v;
		else if (io->bytespersample==4)
			((int *)buf)[i]=v;
//		printf("%u %lu\n",io->bytespersample,v);
	}
	riffoser_writebuf(len,buf);

	free(buf);

//	printf("\n");
//	riffoser_writebuf(io->srcsize,io->src);
	
	fclose(fp);
	
	return (EXIT_SUCCESS);
}

int riffoser_wav_loadfromfile(struct riffoser_io_struct *io) {
	FILE * fp;
	char * tmps;
	unsigned int dlen,tmpi;
	unsigned short tmpw,bitspersample;
	unsigned long tmpl,pow256;

	char *buffer;

	if ((fp=fopen(io->filename,"rb"))==NULL) {
		printf("failed to open file %s for reading!\n",io->filename);
		return;
	}
	tmps=malloc(5);
	riffoser_readstr(tmps,4);
	
	if (!strcmp(tmps,"RIFF")) { // its RIFF

		
		riffoser_readint(dlen,4);
		
		free(tmps);
		tmps=malloc(9);
		riffoser_readstr(tmps,8);
		
		if (!strcmp(tmps,"WAVEfmt ")) { // its WAV
		
			riffoser_readint(tmpi,4);
			
			if (tmpi==16) { // its PCM
			
				riffoser_readint(tmpw,2);
				
				if (tmpw==1) { // there's no compression
				
					riffoser_readint(io->channels,2);
					
					riffoser_readint(io->samplerate,4);
					
					riffoser_readint(tmpi,4);
					riffoser_readint(tmpw,2);
					
					riffoser_readint(bitspersample,2);
					io->bytespersample=bitspersample/8;
					
					free(tmps);
					tmps=malloc(5);
					riffoser_readstr(tmps,4);
					
					if (!strcmp(tmps,"data")) { // header was read correctly
						riffoser_readint(io->srcsize,4);
						buffer=malloc(io->srcsize);
						memset(buffer,0,io->srcsize);
						riffoser_readbuf(buffer,io->srcsize);
						io->srcsize/=io->bytespersample;
						io->src=malloc(sizeof(io_src_t)*io->srcsize);
						memset(io->src,0,sizeof(io_src_t)*io->srcsize);

						pow256=pow(256,io->bytespersample)/2;

						for (tmpl=0;tmpl<io->srcsize;tmpl++) {
							if (io->bytespersample==1)
								io->src[tmpl]=((io_src_t)((char *)buffer)[tmpl]/pow256);
							else if (io->bytespersample==2)
								io->src[tmpl]=((io_src_t)((short *)buffer)[tmpl]/pow256);
							else if (io->bytespersample==4)
								io->src[tmpl]=((io_src_t)((int *)buffer)[tmpl]/pow256);
							if (io->src[tmpl]>=1) {
								printf("%u %f\n",io->bytespersample,io->src[tmpl]);
								exit(1);
							}
						}


						free(buffer);
					}
				}
			}
		}
	}

	free(tmps);
	fclose(fp);

	return (EXIT_SUCCESS);
}

