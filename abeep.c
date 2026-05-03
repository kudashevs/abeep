#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PCM_DEFAULT "default"
#define SAMPLE_RATE 8000

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

	int samples = (SAMPLE_RATE * duration) / 1000;
	uint8_t buffer[samples];
	for (int i = 0; i < samples; i++) {
		buffer[i] = (uint8_t)(128 + 127 * sin(2 * M_PI * freq * i / SAMPLE_RATE));
	}

	if (snd_pcm_writei(playback_handle, buffer, samples) < 0) {
		perror("Can't write to the connection");
		snd_pcm_close(playback_handle);
		exit(EXIT_FAILURE);
	}

	snd_pcm_drain(playback_handle);
	snd_pcm_close(playback_handle);
}

int main() {
	beep(440, 200);

	return EXIT_SUCCESS;
}
