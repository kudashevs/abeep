# abeep

This is a beep program that uses ALSA (Advanced Linux Sound Architecture) Audio
API as a medium to play sound.


## Installation

Clone the repository. Then, make install:
```bash
meke install
```


## Options

```
-f       beep's frequency in Hz (min 40.00, max 16000.00 Hz)
-r       audio stream rate in Hz (min 4000, max 96000 Hz)
-l       beep's duration in ms (min 20, max 3600000 ms)
-s       process input and beep on every string
-i       display device and program information
-v       run in verbose mode
-h       print help
-V       display version information
```


## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## License

The MIT License (MIT). Please see the [License file](LICENSE.md) for more information.

