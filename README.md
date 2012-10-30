riffoser

by anon@anonymous.lv
========

A C library for generating RIFF sounds, combining them into tracks and adding effects. other formats (MP3, S3M, ...) exporting is planned later

===============================================

OBJECT TYPES:
========

struct riffoser_track
object representing wave track - collection of wave sequences and effects

struct riffoser_wave
object representing wave specification


FUNCTIONS:
========

struct riffoser_track * riffoser_track_init(riffoser_channel_t channels)
initializes and returns track object. parameters are:
channels - number of channels for track, i.e. 1 for mono, 2 for stereo, etc

--------

void riffoser_track_free(struct riffoser_track * track)
frees track object

--------

struct riffoser_wave * riffoser_wave_init(riffoser_wavetype_t wavetype,riffoser_amplitude_t amplitude,riffoser_frequency_t frequency,riffoser_pitch_t pitch)
initializes and returns wave object. parameters are:
wavetype can be one of RIFFOSER_WAVE_SQUARE RIFFOSER_WAVE_TRIANGLE RIFFOSER_WAVE_SINE RIFFOSER_WAVE_COSINUSOID
amplitude is generally the volume of wave, in percents (0-100)
frequency is frequency of the wave, in Hz
pitch is position of wave's top point, in percents (0-100). 50 is common choise

--------

void riffoser_wave_free(struct riffoser_wave * wave)
frees wave object

--------

void riffoser_track_writeriff(struct riffoser_track * track,char * filename,riffoser_samplerate_t samplerate,riffoser_bitspersample_t bitspersample)
writes composed track into PCM Wav Riff file (.wav). parameters are:
track - track object to write
filename - name of file to be created (something.wav)
samplerate - samples per second, common values are 8000 11025 22100 44100 48000 96000 192000 (larger means better quality)
bitspersample - bits per sample, common values are 8 16 24 32(not guaranteed to work)

--------

void riffoser_track_addwave(struct riffoser_track * track,struct riffoser_wave * wave,riffoser_channel_t channel,riffoser_trackpos_t from,riffoser_trackpos_t to);
adds wave to track in desired track period. parameters:
track - track object
wave - wave object to add
channel - channel to add wave on, on mono must be 0, on stereo 0 for left and 1 for right, etc
from - track position (float, seconds) from which wave will be sequenced
to - track position (float, seconds) to which wave will be sequenced

