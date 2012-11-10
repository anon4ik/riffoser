#ifndef RIFFOSER_R_H
#define	RIFFOSER_R_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <riffoser.h>
#include <wavefuncs.h>

#define io_src_t riffoser_data_t

	struct riffoser_io_struct {
		io_src_t *src;
		unsigned long srcsize;
		riffoser_bytespersample_t bytespersample;
		riffoser_samplerate_t samplerate;
		char *filename;
		riffoser_channel_t channels;
		riffoser_kbps_t kbps;
	};
	
#define RIFFOSER_ENSUREBOUNDS(v,min,max) {\
	if (v<min)\
		v=min;\
	else if (v>max)\
		v=max;\
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

#ifdef	__cplusplus
}
#endif

#endif	/* RIFFOSER_R_H */

