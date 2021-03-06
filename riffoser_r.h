#ifndef RIFFOSER_R_H
#define	RIFFOSER_R_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <riffoser.h>
#include <wavefuncs.h>

#define CHUNKSIZE_WAV_READ 65536
#define CHUNKSIZE_WAV_WRITE 65536
#define CHUNKSIZE_MP3_READ 1024
#define CHUNKSIZE_MP3_WRITE 1024
#define CHUNKSIZE_OGG_READ 16384
#define CHUNKSIZE_OGG_WRITE 16384

#define RIFFOSER_ENSUREBOUNDS(v,min,max) {\
	if (v<min)\
		v=min;\
	else if (v>max)\
		v=max;\
} 

#define riffoser_readstr(fp,arg,size) {\
	fread(arg,size,1,fp);\
	arg[size]='\0';\
}
#define riffoser_readint(fp,arg,padding) {\
	fread(&arg,padding,1,fp);\
}
#define riffoser_readbuf(fp,buf,size) {\
	fread(buf,size,1,fp);\
}
#define riffoser_writestr(fp,value) {\
	fprintf(fp,value);\
}
#define riffoser_writeint(fp,padding,value) {\
	fpi=value;\
	fwrite((const void *)&fpi,padding,1,fp);\
}
#define riffoser_writebuf(fp,size,count,buf) {\
	fwrite(buf,size,count,fp);\
}

#ifdef	__cplusplus
}
#endif

#endif	/* RIFFOSER_R_H */

