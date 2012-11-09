#ifndef WAVEFUNCS_H
#define	WAVEFUNCS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RIFFOSER_WAVE_FUNC(_wave,x) {\
	if (_wave->type==RIFFOSER_WAVE_SQUARE)\
		fret=x<_wave->pitch?0:100;\
	else if (_wave->type==RIFFOSER_WAVE_TRIANGLE)\
		fret=x<_wave->pitch?x*2:100-(x-_wave->pitch)*2;\
	else if (_wave->type==RIFFOSER_WAVE_SINE)\
		fret=100*sin(2*M_PI*x/100+_wave->pitch);/*x<_wave->pitch?sin(x)*100:sin(100-(x-_wave->pitch)*2);*/\
	else if (_wave->type==RIFFOSER_WAVE_COSINUSOID)\
		fret=x<_wave->pitch?cos(x)*100:cos(100-(x-_wave->pitch)*2);\
	else if (_wave->type==RIFFOSER_WAVE_TEST1)\
		fret=30*(cos(M_PI*(x+1))-sin(2*M_PI*x/100))/*x<_wave->pitch?tan(x)*100:tan(100-(x-_wave->pitch)*2)*100*/;\
	else if (_wave->type==_RIFFOSER_WAVE_DATA){\
		i7=(unsigned long)floor((double)x*(double)((double)_wave->data_count/100))+(chan<track->channels?chan:0);\
		if (i7>=_wave->data_count)\
			i7=_wave->data_count-1;\
		fret=_wave->data[i7];\
	}\
}

#ifdef	__cplusplus
}
#endif

#endif	/* WAVEFUNCS_H */
