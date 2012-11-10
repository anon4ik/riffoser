#include <riffoser_r.h>

struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t type,riffoser_percent_t amplitude,riffoser_frequency_t frequency,riffoser_percent_t pitch) {
	struct riffoser_wave * wave;
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	wave->type=type;
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->amplitude=amplitude/100;
	wave->frequency=frequency;
	wave->channels=1;
	RIFFOSER_ENSUREBOUNDS(pitch,0,99);
	wave->pitch=pitch/100;
	return wave;
}


void riffoser_wave_free(struct riffoser_wave * wave) {
	unsigned int i;
	if (wave->data!=NULL) {
		free(wave->data);
	}
	free(wave);
}

void riffoser_wave_parsesrc(struct riffoser_wave *wave, struct riffoser_io_struct *io,riffoser_percent_t length,riffoser_percent_t amplitude) {
	unsigned int tmpi;
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	amplitude/=100;
	wave->channels=io->channels;
	wave->frequency=length/(riffoser_frequency_t)(io->srcsize/(riffoser_frequency_t)wave->channels/(riffoser_frequency_t)io->samplerate)/100;
	wave->data_count=io->srcsize;
	tmpi=sizeof(riffoser_data_t)*io->srcsize;
	wave->data=malloc(tmpi);
	memset(wave->data,0,tmpi);
	for (tmpi=0;tmpi<io->srcsize;tmpi++) {
		if (io->src[tmpi]>=1)
			printf("%f\n",io->src[tmpi]);
		wave->data[tmpi]=io->src[tmpi]*amplitude;
	}

//	memcpy(wave->data,io->src,tmpi);
/*	wave->data_count=io->srcsize/io->bytespersample;
	wave->data=malloc(sizeof(double)*wave->data_count);
	for (tmpi=0;tmpi<wave->data_count;tmpi++) {
		if (io->bytespersample==1)
			wave->data[tmpi]=(riffoser_data_t)(((((unsigned char *)io->src)[tmpi]))*amplitude/256);
		else if (io->bytespersample==2)
			wave->data[tmpi]=(riffoser_data_t)(((((short *)io->src)[tmpi])+32768)*amplitude*2/65536);
		else if (io->bytespersample==4)
			wave->data[tmpi]=(riffoser_data_t)((riffoser_data_t)((((int *)io->src)[tmpi])+2147483648)*amplitude/4294967296);
	}
	*/
}

struct riffoser_wave *riffoser_wave_loadfrommp3(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	struct riffoser_wave *wave;
	struct riffoser_io_struct *io;
	
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->type=_RIFFOSER_WAVE_DATA;
	wave->amplitude=amplitude/100;
	wave->pitch=50;
	
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	riffoser_mp3_loadfromfile(io);

	riffoser_wave_parsesrc(wave,io,length,amplitude);
	
	free(io->src);
	free(io);
	
	return wave;
}

struct riffoser_wave *riffoser_wave_loadfromwav(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	struct riffoser_wave *wave;
	struct riffoser_io_struct *io;
	
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	RIFFOSER_ENSUREBOUNDS(amplitude,0,99);
	wave->type=_RIFFOSER_WAVE_DATA;
	wave->amplitude=amplitude/100;
	wave->pitch=50;
	
	io=malloc(sizeof(struct riffoser_io_struct));
	memset(io,0,sizeof(struct riffoser_io_struct));
	io->filename=filename;
	riffoser_wav_loadfromfile(io);

	riffoser_wave_parsesrc(wave,io,length,amplitude);
	
	free(io->src);
	free(io);
	
	return wave;
}

