#include <riffoser_r.h>

struct riffoser_instrument * riffoser_instrument_init() {
	struct riffoser_instrument * instrument;
	instrument=malloc(sizeof(struct riffoser_instrument));
	memset(instrument,0,sizeof(struct riffoser_instrument));
	return instrument;
}

void riffoser_instrument_free(struct riffoser_instrument * instrument) {
	free(instrument);
}

