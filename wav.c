#include <riffoser_r.h>

int riffoser_wav_savetofile(struct riffoser_io_struct *io) {
	FILE *fp;
	unsigned long i,fpi;
	fp=fopen(io->filename,"wb");
	
	i=4+24+8+io->srcsize+(io->srcsize%2>0?1:0);
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
	riffoser_writeint(4,io->srcsize);
	if (io->srcsize%2>0)
		riffoser_writeint(1,0);
	
	riffoser_writebuf(io->srcsize,io->src);
	
	fclose(fp);
	
	return (EXIT_SUCCESS);
}

int riffoser_wav_loadfromfile(struct riffoser_io_struct *io) {
	FILE * fp;
	char * tmps;
	unsigned int dlen,tmpi;
	unsigned short tmpw,bitspersample;
	
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
						io->src=malloc(io->srcsize);
						memset(io->src,0,io->srcsize);

						riffoser_readbuf(io->src,io->srcsize);
					}
				}
			}
		}
	}

	free(tmps);
	fclose(fp);

	return (EXIT_SUCCESS);
}

