#include <riffoser_r.h>

int riffoser_wav_write_start(struct riffoser_io_struct *io) {
//	printf("start\n");
	unsigned long len,i,fpi;
	unsigned int dlen;

	io->fp=fopen(io->filename,"wb");

	dlen=io->datalen*io->bytespersample;

	i=4+24+8+dlen+(dlen%2>0?1:0);
	riffoser_writestr(io->fp,"RIFF");
	riffoser_writeint(io->fp,4,i);
	riffoser_writestr(io->fp,"WAVEfmt ");
	riffoser_writeint(io->fp,4,16);
	riffoser_writeint(io->fp,2,1);
	riffoser_writeint(io->fp,2,io->channels);
	riffoser_writeint(io->fp,4,io->samplerate);
	riffoser_writeint(io->fp,4,io->bytespersample*io->channels*io->samplerate);
	riffoser_writeint(io->fp,2,io->bytespersample*io->channels);
	riffoser_writeint(io->fp,2,io->bytespersample*8);
	riffoser_writestr(io->fp,"data");
	riffoser_writeint(io->fp,4,dlen);
	if (dlen%2>0)
		riffoser_writeint(io->fp,1,0);
	
}

int riffoser_wav_write_bytes(struct riffoser_io_struct *io) {
//	printf("bytes\n");
	char *buf;
	unsigned int i;
	signed long v,pow256,len;
	
	len=io->srcsize*io->bytespersample;
	buf=malloc(len);

	pow256=pow(256,io->bytespersample);
	for (i=0;i<io->srcsize;i++) {
		v=round((((io_src_t *)io->src)[i])*pow256/*-(io->bytespersample>1?pow256/2:1)*/);
		if (v>=pow256)
			printf("%li >= %lu\n",v,pow256);
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
	}
	riffoser_writebuf(io->fp,io->bytespersample,io->srcsize,buf);

	free(buf);
}

int riffoser_wav_write_end(struct riffoser_io_struct *io) {
//	printf("end\n");
	fclose(io->fp);
}

int riffoser_wav_read_start(struct riffoser_io_struct *io) {
	0&&printf("start %s\n",io->filename);
	char * tmps;
	unsigned int tmpi;	
	unsigned short tmpw;
	
	if ((io->fp=fopen(io->filename,"rb"))==NULL) {
		printf("failed to open file %s for reading!\n",io->filename);
		return (EXIT_FAILURE);
	}
	
	tmps=malloc(5);
	riffoser_readstr(io->fp,tmps,4);
	
	if (!strcmp(tmps,"RIFF")) { // its RIFF

		
		riffoser_readint(io->fp,tmpi,4);
		
		free(tmps);
		tmps=malloc(9);
		riffoser_readstr(io->fp,tmps,8);
		
		if (!strcmp(tmps,"WAVEfmt ")) { // its WAV
		
			riffoser_readint(io->fp,tmpi,4);
			
			if (tmpi==16) { // its PCM
			
				riffoser_readint(io->fp,tmpw,2);
				
				if (tmpw==1) { // there's no compression
				
					riffoser_readint(io->fp,io->channels,2);
					
					riffoser_readint(io->fp,io->samplerate,4);
					
					riffoser_readint(io->fp,tmpi,4);
					riffoser_readint(io->fp,tmpw,2);
					
					riffoser_readint(io->fp,tmpw,2);
					io->bytespersample=tmpw/8;
					
					free(tmps);
					tmps=malloc(5);
					riffoser_readstr(io->fp,tmps,4);
					
					if (!strcmp(tmps,"data")) { // header was read correctly
						free(tmps);

						riffoser_readint(io->fp,tmpi,4);
						
						io->datalen=ceil(tmpi/io->bytespersample);
						
						0&&printf("/start %s\n",io->filename);
						return (EXIT_SUCCESS);
					}
				}
			}
		}
	}
	free(tmps);
	return (EXIT_FAILURE);
}

int riffoser_wav_read_bytes(struct riffoser_io_struct *io) {
	unsigned long tmpl,pow256,dlen;

	0&&printf("bytes %s\n",io->filename);
	char *buffer;

	dlen=io->srcsize*io->bytespersample;
	0&&printf("io->srcsize=%lu dlen=%lu\n",io->srcsize,dlen);
	buffer=malloc(dlen);
	memset(buffer,0,dlen);
	riffoser_readbuf(io->fp,buffer,dlen);

	pow256=pow(256,io->bytespersample);

	for (tmpl=0;tmpl<io->srcsize;tmpl++) {
		if (io->bytespersample==1)
			io->src[tmpl]=((io_src_t)((unsigned char *)buffer)[tmpl]/pow256+0.5f);
		else if (io->bytespersample==2)
			io->src[tmpl]=((io_src_t)((short *)buffer)[tmpl]/pow256+0.5f);
		else if (io->bytespersample==4)
			io->src[tmpl]=((io_src_t)((int *)buffer)[tmpl]/pow256+0.5f);
		if (io->src[tmpl]<0) {
			printf("%f < 0\n",io->src[tmpl]);
			exit(1);
		}
		if (io->src[tmpl]>=1) {
			printf("%f >= 1\n",io->src[tmpl]);
			exit(1);
		}
	}


	free(buffer);

	0&&printf("/bytes %s\n",io->filename);
}

int riffoser_wav_read_end(struct riffoser_io_struct *io) {
	0&&printf("end %s\n",io->filename);
	fclose(io->fp);
	0&&printf("/end %s\n",io->filename);
}

int riffoser_wav_read_reset(struct riffoser_io_struct *io) {
	riffoser_wav_read_end(io);
	riffoser_wav_read_start(io);
}

int riffoser_wav_read_skip(struct riffoser_io_struct *io,unsigned long samples) {
	printf("skip %lu\n",samples);
	fseek(io->fp,samples*io->bytespersample,SEEK_CUR);
}