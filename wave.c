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
	if (wave->filename!=NULL) {
		free(wave->filename);
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
		if (io->src[tmpi]>1)
			printf("%f > 1\n",io->src[tmpi]);
		wave->data[tmpi]=io->src[tmpi]*amplitude;
	}
}

extern int riffoser_ogg_read_start(struct riffoser_io_struct *io);
extern int riffoser_ogg_read_bytes(struct riffoser_io_struct *io);
extern int riffoser_ogg_read_end(struct riffoser_io_struct *io);
extern int riffoser_ogg_read_skip(struct riffoser_io_struct *io,unsigned long bytes);
extern int riffoser_ogg_read_reset(struct riffoser_io_struct *io);

struct riffoser_wave *riffoser_wave_readogg(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	struct riffoser_wave *wave;
	
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	
	wave->type=_RIFFOSER_WAVE_IO;
	wave->amplitude=amplitude/100;
	wave->lengthscale=length/100;
	wave->pitch=50;
	wave->filename=strdup(filename);
	wave->readfuncs.start=riffoser_ogg_read_start;
	wave->readfuncs.bytes=riffoser_ogg_read_bytes;
	wave->readfuncs.end=riffoser_ogg_read_end;
	wave->readfuncs.skip=riffoser_ogg_read_skip;
	wave->readfuncs.reset=riffoser_ogg_read_reset;
	
	return wave;
}

extern int riffoser_mp3_read_start(struct riffoser_io_struct *io);
extern int riffoser_mp3_read_bytes(struct riffoser_io_struct *io);
extern int riffoser_mp3_read_end(struct riffoser_io_struct *io);
extern int riffoser_mp3_read_skip(struct riffoser_io_struct *io,unsigned long bytes);
extern int riffoser_mp3_read_reset(struct riffoser_io_struct *io);

struct riffoser_wave *riffoser_wave_readmp3(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	struct riffoser_wave *wave;

	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));

	wave->type=_RIFFOSER_WAVE_IO;
	wave->amplitude=amplitude/100;
	wave->lengthscale=length/100;
	wave->pitch=50;
	wave->filename=strdup(filename);
	wave->readfuncs.start=riffoser_mp3_read_start;
	wave->readfuncs.bytes=riffoser_mp3_read_bytes;
	wave->readfuncs.end=riffoser_mp3_read_end;
	wave->readfuncs.skip=riffoser_mp3_read_skip;
	wave->readfuncs.reset=riffoser_mp3_read_reset;
	
	return wave;
}

extern int riffoser_wav_read_start(struct riffoser_io_struct *io);
extern int riffoser_wav_read_bytes(struct riffoser_io_struct *io);
extern int riffoser_wav_read_end(struct riffoser_io_struct *io);
extern int riffoser_wav_read_skip(struct riffoser_io_struct *io,unsigned long bytes);
extern int riffoser_wav_read_reset(struct riffoser_io_struct *io);

struct riffoser_wave *riffoser_wave_readwav(char *filename,riffoser_percent_t amplitude,riffoser_percent_t length) {
	struct riffoser_wave *wave;
	
	wave=malloc(sizeof(struct riffoser_wave));
	memset(wave,0,sizeof(struct riffoser_wave));
	
	wave->type=_RIFFOSER_WAVE_IO;
	wave->amplitude=amplitude/100;
	wave->lengthscale=length/100;
	wave->pitch=50;
	wave->filename=strdup(filename);
	wave->readfuncs.start=riffoser_wav_read_start;
	wave->readfuncs.bytes=riffoser_wav_read_bytes;
	wave->readfuncs.end=riffoser_wav_read_end;
	wave->readfuncs.skip=riffoser_wav_read_skip;
	wave->readfuncs.reset=riffoser_wav_read_reset;
	
	return wave;
}
