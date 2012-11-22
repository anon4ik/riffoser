#include <riffoser_r.h>

struct riffoser_instrument * riffoser_instrument_init(riffoser_channel_t channels) {
	struct riffoser_instrument * instrument;
	instrument=malloc(sizeof(struct riffoser_instrument));
	memset(instrument,0,sizeof(struct riffoser_instrument));
	instrument->channels=channels;
	return instrument;
}

void riffoser_instrument_free(struct riffoser_instrument *instrument) {
	unsigned long i;
	if (instrument->waves_count>0) {
		for (i=0;i<instrument->waves_count;i++) {
			if (instrument->wavestates[i]->io!=NULL)
				free(instrument->wavestates[i]->io);
			free(instrument->wavestates[i]);
		}
		free(instrument->wavestates);
		free(instrument->waves);
	}
	if (instrument->instruments_count>0) {
		for (i=0;i<instrument->instruments_count;i++)
			free(instrument->instrumentstates[i]);
		free(instrument->instrumentstates);
		free(instrument->instruments);
	}
	free(instrument);
}

void riffoser_instrument_addwave(struct riffoser_instrument *instrument, struct riffoser_wave *wave, riffoser_channel_t channel, riffoser_tracklen_t from, riffoser_tracklen_t to) {
	if (instrument->waves_count==0) {
		instrument->waves=malloc(sizeof(struct riffoser_wave *));
		instrument->wavestates=malloc(sizeof(struct riffoser_wavestate *));
	} else {
		instrument->waves=realloc(instrument->waves,sizeof(struct riffoser_wave *)*(instrument->waves_count+1));
		instrument->wavestates=realloc(instrument->wavestates,sizeof(struct riffoser_wavestate *)*(instrument->waves_count+1));
	}
	RIFFOSER_ENSUREBOUNDS(channel,0,instrument->channels-1);
	instrument->waves[instrument->waves_count]=wave;
	instrument->wavestates[instrument->waves_count]=malloc(sizeof(struct riffoser_wavestate));
	memset(instrument->wavestates[instrument->waves_count],0,sizeof(struct riffoser_wavestate));
	instrument->wavestates[instrument->waves_count]->from=from;
	instrument->wavestates[instrument->waves_count]->to=to;
	instrument->wavestates[instrument->waves_count]->channel=channel;
	instrument->waves_count++;
	
	if (to>instrument->length)
		instrument->length=to;
}

void riffoser_instrument_play(struct riffoser_track *track, struct riffoser_instrument *instrument, riffoser_channel_t channel, riffoser_tracklen_t from, riffoser_tracklen_t to) {
	if (track->instruments_count==0) {
		track->instruments=malloc(sizeof(struct riffoser_instrument *));
		track->instrumentstates=malloc(sizeof(struct riffoser_instrumentstate));
	} else {
		track->instruments=realloc(track->instruments,sizeof(struct riffoser_instrument *)*(track->instruments_count+1));
		track->instrumentstates=realloc(track->instrumentstates,sizeof(struct riffoser_instrumentstate *)*(track->instruments_count+1));
	}
	RIFFOSER_ENSUREBOUNDS(channel,0,track->channels-1);
	track->instruments[track->instruments_count]=instrument;
	track->instrumentstates[track->instruments_count]=malloc(sizeof(struct riffoser_instrumentstate));
	memset(track->instrumentstates[track->instruments_count],0,sizeof(struct riffoser_instrumentstate));
	track->instrumentstates[track->instruments_count]->from=from;
	track->instrumentstates[track->instruments_count]->to=to;
	track->instrumentstates[track->instruments_count]->channel=channel;
	track->instruments_count++;
	
	if (to>track->length)
		track->length=to;
}
