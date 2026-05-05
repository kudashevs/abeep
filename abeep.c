#include <alsa/asoundlib.h>
#include <assert.h>
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
void beep(float freq, int rate, int duration) {
  snd_pcm_t* playback_handle;

  if (snd_pcm_open(&playback_handle, PCM_DEFAULT, SND_PCM_STREAM_PLAYBACK, 0) <
      0) {
    perror("Can't connect to the default interface");
    exit(EXIT_FAILURE);
  }

  if (snd_pcm_set_params(playback_handle, SND_PCM_FORMAT_U8,
                         SND_PCM_ACCESS_RW_INTERLEAVED, 1, rate, 1,
                         1000000) < 0) {
    perror("Can't set connection parameters");
    exit(EXIT_FAILURE);
  }

  assert(freq != 0);
  assert(duration != 0);

  int samples = (rate * duration) / 1000;
  if (samples > MAX_SAMPLES) {
    perror("Number of samples exceeds safety limits");
    exit(EXIT_FAILURE);
  }

  uint8_t* buffer = malloc(samples);
  if (buffer == NULL) {
    perror("Can't allocate necessary memory");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < samples; i++) {
    buffer[i] = (uint8_t)(128 + 127 * sin(2 * M_PI * freq * i / rate));
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

static void print_help(char* name) {
  fprintf(stdout,
          "Usage:%s [OPTION]... \n"
          "-h		this help\n",
          basename(name));
}

static void get_default_device_hints() {
    void **hints, **n;
    char *name, *desc;

    if (snd_device_name_hint(-1, "pcm", &hints) < 0) {
        perror("Can't retrieve device information");
        exit(EXIT_FAILURE);
    }

    for (n = hints; *n; n++) {
        name = snd_device_name_get_hint(*n, "NAME");

        if (name && strcmp(name, "default") == 0) {
            desc = snd_device_name_get_hint(*n, "DESC");

            printf("Device: %s (%s)\n", name, desc ? desc : "N/A");

            if (desc) free(desc);
            break;
        }

        if (name) free(name);
    }

    snd_device_name_free_hint(hints);
}

static void print_info() {
  snd_pcm_t* handle;
  snd_pcm_hw_params_t* params;
  unsigned int tmp_u, tmp_min, tmp_max;
  int tmp_i;

  get_default_device_hints();

  if (snd_pcm_open(&handle, PCM_DEFAULT, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
    perror("Can't connect to the default interface");
    exit(EXIT_FAILURE);
  }

  snd_pcm_hw_params_alloca(&params);
  if (snd_pcm_hw_params_any(handle, params) < 0) {
    perror("Can't get device parameters\n");
    exit(EXIT_FAILURE);
  }

  snd_pcm_hw_params_get_rate(params, &tmp_u, &tmp_i);
  printf("Default rate: %u Hz\n", tmp_u);

  snd_pcm_hw_params_get_rate_min(params, &tmp_min, &tmp_i);
  snd_pcm_hw_params_get_rate_max(params, &tmp_max, &tmp_i);
  printf("Rate range: %u to %u Hz\n", tmp_min, tmp_max);

  tmp_u = tmp_i = tmp_min = tmp_max = 0;

  snd_pcm_hw_params_get_channels(params, &tmp_u);
  printf("Default channels: %u\n", tmp_u);

  snd_pcm_hw_params_get_channels_min(params, &tmp_min);
  snd_pcm_hw_params_get_channels_max(params, &tmp_max);
  printf("Channels range: %u to %u Hz\n", tmp_min, tmp_max);

  snd_pcm_close(handle);
}

int main(int argc, char** argv) {
  float freq = FREQUENCY;
  int rate = SAMPLE_RATE;
  int duration = DURATION;
  int opt;

  while ((opt = getopt(argc, argv, "hi")) != EOF) {
    switch (opt) {
      case 'h':
        print_help(argv[0]);
        exit(EXIT_SUCCESS);

      case 'i':
        print_info();
        exit(EXIT_SUCCESS);

      case '?':
        fprintf(stderr, "Unknown option -%c", optopt);
        exit(EXIT_FAILURE);
    }
  }

  beep(freq, rate, duration);

  return EXIT_SUCCESS;
}
