#ifndef WAVEFUNCS_H
#define	WAVEFUNCS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RIFFOSER_WAVE_FUNC(_wave,_wavestate,x) {\
	if (_wave->type==RIFFOSER_WAVE_SQUARE)\
		fret=x<_wave->pitch?0:1;\
	else if (_wave->type==RIFFOSER_WAVE_TRIANGLE)\
		fret=x<_wave->pitch?x*2:1-(x-_wave->pitch)*2;\
	else if (_wave->type==RIFFOSER_WAVE_SINE)\
		fret=1*sin(2*M_PI*x/1+_wave->pitch);/*x<_wave->pitch?sin(x)*1:sin(1-(x-_wave->pitch)*2);*/\
	else if (_wave->type==RIFFOSER_WAVE_COSINUSOID)\
		fret=x<_wave->pitch?cos(x)*1:cos(1-(x-_wave->pitch)*2);\
	else if (_wave->type==RIFFOSER_WAVE_TEST1)\
		fret=30*(cos(M_PI*(x+1))-sin(2*M_PI*x/1))/*x<_wave->pitch?tan(x)*1:tan(1-(x-_wave->pitch)*2)*1*/;\
	else if (_wave->type==_RIFFOSER_WAVE_DATA){\
		i7=(unsigned long)round((double)x*(double)((double)_wave->data_count))+(chan<track->channels?chan:0);\
		if (i7>=_wave->data_count)\
			i7=_wave->data_count-1;\
		fret=_wave->data[i7];\
		if (fret>=1)\
			printf("==%f\n",fret);\
	}\
	else if (_wave->type==_RIFFOSER_WAVE_IO) {\
		i7=(unsigned long)round((double)x*(double)((double)_wavestate->io->datalen))+(chan<track->channels?chan:0);\
		if (i7>=_wavestate->io->datalen)\
			i7=_wavestate->io->datalen-1;\
		if (i7<_wavestate->bufoffset) {\
			i9=floor(i7/chunksize_read);\
			_wave->readfuncs.end(_wavestate->io);\
			_wave->readfuncs.start(_wavestate->io);\
			free(_wavestate->io->src);\
			_wavestate->bufoffset=i9*chunksize_read;\
			if (i9>0) {\
				_wavestate->io->srcsize=_wavestate->bufoffset;\
				_wavestate->io->src=malloc(sizeof(io_src_t)*_wavestate->io->srcsize);\
				memset(_wavestate->io->src,0,sizeof(io_src_t)*_wavestate->io->srcsize);\
				_wave->readfuncs.bytes(_wavestate->io);\
				free(_wavestate->io->src);\
			}\
			_wavestate->io->srcsize=chunksize_read;\
			0&&printf("rrr i9=%lu i7=%lu bufoffset=%lu toread=%lu\n",i9,i7,_wavestate->bufoffset,_wavestate->io->srcsize);\
			_wavestate->io->src=malloc(sizeof(io_src_t)*_wavestate->io->srcsize);\
			memset(_wavestate->io->src,0,sizeof(io_src_t)*_wavestate->io->srcsize);\
			_wave->readfuncs.bytes(_wavestate->io);\
			fret=_wavestate->io->src[i7];\
		}\
		else if (i7<_wavestate->bufoffset+chunksize_read) {\
			fret=_wavestate->io->src[i7-_wavestate->bufoffset];\
			0&&printf("zzz pos=%lu i7=%lu bufoffset=%lu fret=%f\n",i7-_wavestate->bufoffset,i7,_wavestate->bufoffset,fret);\
		}\
		else {\
			i9=floor((i7)/chunksize_read);\
			free(_wavestate->io->src);\
			if (i9*chunksize_read-_wavestate->bufoffset-chunksize_read>0) {\
				_wavestate->io->srcsize=i9*chunksize_read-_wavestate->bufoffset-chunksize_read;\
				0&&printf("fff i9=%lu i7=%lu bufoffset=%lu toskip=%lu\n",i9,i7,_wavestate->bufoffset,_wavestate->io->srcsize);\
				_wavestate->io->src=malloc(sizeof(io_src_t)*_wavestate->io->srcsize);\
				memset(_wavestate->io->src,0,sizeof(io_src_t)*_wavestate->io->srcsize);\
				_wave->readfuncs.bytes(_wavestate->io);\
				free(_wavestate->io->src);\
			}\
			_wavestate->bufoffset=i9*chunksize_read;\
			_wavestate->io->srcsize=chunksize_read;\
			_wavestate->io->src=malloc(sizeof(io_src_t)*_wavestate->io->srcsize);\
			memset(_wavestate->io->src,0,sizeof(io_src_t)*_wavestate->io->srcsize);\
			_wave->readfuncs.bytes(_wavestate->io);\
			fret=_wavestate->io->src[i7-_wavestate->bufoffset];\
			0&&printf("fff pos=%lu i9=%lu i7=%lu bufoffset=%lu toread=%lu fret=%f\n",i7-_wavestate->bufoffset,i9,i7,_wavestate->bufoffset,_wavestate->io->srcsize,fret);\
		}\
	}\
}

#ifdef	__cplusplus
}
#endif

#endif	/* WAVEFUNCS_H */

