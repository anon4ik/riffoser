#include <riffoser_r.h>

int riffoser_mp3_savetofile(struct riffoser_io_struct *io) {
#ifndef USE_LAME
	printf("Error: riffoser was compiled without mp3 support (recompile with --with-lame)\n");
	return (EXIT_FAILURE);
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_loadfromfile(struct riffoser_io_struct *io) {
#ifndef USE_LAME
	printf("Error: riffoser was compiled without mp3 support (recompile with --with-lame)\n");
	return (EXIT_FAILURE);
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}
