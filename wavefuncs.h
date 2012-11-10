#ifndef WAVEFUNCS_H
#define	WAVEFUNCS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RIFFOSER_WAVE_FUNC(_wave,x) {\
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
		i7=(unsigned long)floor((double)x*(double)((double)_wave->data_count))+(chan<track->channels?chan:0);\
		if (i7>=_wave->data_count)\
			i7=_wave->data_count-1;\
		fret=_wave->data[i7];\
		if (fret>=1)\
			printf("==%f\n",fret);\
	}\
}

#ifdef	__cplusplus
}
#endif

#endif	/* WAVEFUNCS_H */

