#ifndef RIFFOSER_R_H
#define	RIFFOSER_R_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <riffoser.h>
#include <wavefuncs.h>

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
#define riffoser_writebuf(fp,size,buf) {\
	fwrite(buf,size,1,fp);\
}

#ifdef	__cplusplus
}
#endif

#endif	/* RIFFOSER_R_H */

