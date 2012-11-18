#include <riffoser_r.h>

#ifdef USE_MP3
	// mp3-specific io structs here, see ogg.c for examples
#else
#define MP3_ERR\
	printf("Error: riffoser was compiled without mp3 support (recompile with --with-mp3)\n");\
	return (EXIT_FAILURE);
#endif

int riffoser_mp3_write_start(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_write_bytes(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_write_end(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_read_start(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_read_bytes(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_read_end(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_read_reset(struct riffoser_io_struct *io) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}

int riffoser_mp3_read_skip(struct riffoser_io_struct *io,unsigned long samples) {
#ifndef USE_MP3
	MP3_ERR
#else
	printf("mp3 not implemented yet\n");
	return (EXIT_FAILURE);
#endif
}
