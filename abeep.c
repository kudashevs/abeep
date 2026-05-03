#include <alsa/asoundlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PCM_DEFAULT "default"
#define SAMPLE_RATE 8000
#define FREQUENCY 750.0
#define DURATION 200
#define MAX_SAMPLES 4194304

/*
 * Generates a beep via ALSA Audio API
 * @param freq Beep frequency in Hz.
 * @param duration Beep duration in ms.
 */
void beep(float freq, int duration) {
	snd_pcm_t *playback_handle;

	if(snd_pcm_open(&playback_handle, PCM_DEFAULT, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		perror("Can't connect to the default interface");
		exit(EXIT_FAILURE);
	}

	if (snd_pcm_set_params(playback_handle, SND_PCM_FORMAT_U8, SND_PCM_ACCESS_RW_INTERLEAVED, 1, SAMPLE_RATE, 1, 1000000) < 0) {
		perror("Can't set connection parameters");
		exit(EXIT_FAILURE);
	}

	assert(freq != 0);
	assert(duration != 0);

	int samples = (SAMPLE_RATE * duration) / 1000;
	if (samples > MAX_SAMPLES) {
		perror("Number of samples exceeds safety limits");
		exit(EXIT_FAILURE);
	}

	uint8_t *buffer = malloc(samples);
	if (buffer == NULL) {
		perror("Can't allocate necessary memory");
		exit(EXIT_FAILURE);
	}


	for (int i = 0; i < samples; i++) {
		buffer[i] = (uint8_t)(128 + 127 * sin(2 * M_PI * freq * i / SAMPLE_RATE));
	}

	if (snd_pcm_writei(playback_handle, buffer, samples) < 0) {
		perror("Can't write to the connection");
		snd_pcm_close(playback_handle);
		exit(EXIT_FAILURE);
	}

	free(buffer);
	snd_pcm_drain(playback_handle);
	snd_pcm_close(playback_handle);
}

int main() {
	beep(FREQUENCY, DURATION);

	return EXIT_SUCCESS;
}
