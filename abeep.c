#include <alsa/asoundlib.h>
#include <assert.h>
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IS_VALID(val, min, max) ((val >= min && val <= max))

#define DEFAULT_NAME "abeep"
#define DEFAULT_PCM "default"
#define DEFAULT_FREQUENCY 750.0
#define DEFAULT_SAMPLE_RATE 8000
#define DEFAULT_DURATION 200
#define DEFAULT_CHANNELS 1
#define DEFAULT_LATENCY 1000000

#define MAX_SAMPLES 4194304
#define MIN_SAMPLE_RATE 4000
#define MAX_SAMPLE_RATE 96000
#define MIN_FREQUENCY 40.0f
#define MAX_FREQUENCY 16000.0f
#define MIN_DURATION 20
#define MAX_DURATION 3600000

/*
 * Generates a beep via ALSA Audio API
 * @param freq Beep frequency in Hz.
 * @param duration Beep duration in ms.
 */
void beep(float freq, int rate, int duration) {
  snd_pcm_t* playback_handle;

  if (snd_pcm_open(&playback_handle, DEFAULT_PCM, SND_PCM_STREAM_PLAYBACK, 0) <
      0) {
    perror("Can't connect to the default interface");
    exit(EXIT_FAILURE);
  }

  if (snd_pcm_set_params(playback_handle, SND_PCM_FORMAT_U8,
                         SND_PCM_ACCESS_RW_INTERLEAVED, DEFAULT_CHANNELS, rate,
                         1, DEFAULT_LATENCY) < 0) {
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

static void print_help() {
  FILE* output = stdout;
  fprintf(output, "Usage: %s [OPTION]... \n", DEFAULT_NAME);
  fprintf(output, "-f       beep's frequency in Hz (min %.2f, max %.2f Hz)\n",
          MIN_FREQUENCY, MAX_FREQUENCY);
  fprintf(output, "-r       audio stream rate in Hz (min %d, max %d Hz)\n",
          MIN_SAMPLE_RATE, MAX_SAMPLE_RATE);
  fprintf(output, "-l       beep's duration in ms (min %d, max %d ms)\n",
          MIN_DURATION, MAX_DURATION);
  fprintf(output, "-h       print help\n");
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

  if (snd_pcm_open(&handle, DEFAULT_PCM, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
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

  snd_pcm_hw_params_get_channels_min(params, &tmp_min);
  snd_pcm_hw_params_get_channels_max(params, &tmp_max);
  printf("Channels range: %u to %u\n", tmp_min, tmp_max);

  snd_pcm_close(handle);
}

static void validate_frequency(float val) {
  float min = MIN_FREQUENCY;
  float max = MAX_FREQUENCY;

  if (!IS_VALID(val, min, max)) {
    fprintf(stderr, "Option -f is out of range (min: %.2f ms, max: %.2f Hz)\n",
            min, max);
    exit(EXIT_FAILURE);
  }
}

static void validate_rate(int val) {
  int min = MIN_SAMPLE_RATE;
  int max = MAX_SAMPLE_RATE;

  if (!IS_VALID(val, min, max)) {
    fprintf(stderr, "Option -r is out of range (min: %d ms, max: %d Hz)\n", min,
            max);
    exit(EXIT_FAILURE);
  }
}

static void validate_length(int val) {
  int min = MIN_DURATION;
  int max = MAX_DURATION;

  if (!IS_VALID(val, min, max)) {
    fprintf(stderr, "Option -l is out of range (min: %d ms, max: %d ms)\n", min,
            max);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {
  float freq = DEFAULT_FREQUENCY;
  unsigned int rate = DEFAULT_SAMPLE_RATE;
  unsigned int duration = DEFAULT_DURATION;
  int opt;

  while ((opt = getopt(argc, argv, ":hif:r:l:")) != EOF) {
    switch (opt) {
      case 'f':
        float freq_candidate = atof(optarg);
        validate_frequency(freq_candidate);
        freq = freq_candidate;
        break;
      case 'r':
        int rate_candidate = atoi(optarg);
        validate_rate(rate_candidate);
        rate = rate_candidate;
        break;

      case 'l':
        int duration_candidate = atoi(optarg);
        validate_length(duration_candidate);
        duration = duration_candidate;
        break;

      case 'h':
        print_help();
        exit(EXIT_SUCCESS);

      case 'i':
        print_info();
        exit(EXIT_SUCCESS);

      case ':':
        fprintf(stderr, "Option -%c requires an argument\n", optopt);
        exit(EXIT_FAILURE);

      case '?':
        fprintf(stderr, "Unknown option -%c\n", optopt);
        exit(EXIT_FAILURE);

      default:
        fprintf(stderr, "Something went wrong. Please create an issue.\n");
        exit(EXIT_FAILURE);
    }
  }

  beep(freq, rate, duration);

  return EXIT_SUCCESS;
}
