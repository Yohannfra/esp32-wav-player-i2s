# Wav i2s player

## Requirements

- ESP-ADF
- ESP-IDF
- An ESP32 audio board (I've only tested on LyraT-4.3)

## Install

```bash
$ git clone --recursive https://github.com/Yohannfra/esp32-wav-player-i2s
```

## Setup

1. There is a small fix to make to *dr_wav.h* to be able to compile it, apply the patch *dr_wav_patch_esp32.patch* to do it.

```bash
$ cd main/lib/dr_libs
$ git apply ../../../dr_wav_patch_esp32.patch
```

2. Move the file *sample.wav* to the root folder of your sdcard

## Build

1. You need to have **$IDF_PATH** and **$ADF_PATH** in you **$PATH** variable, then run (see more [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#step-4-set-up-the-environment-variables))
```
$ get_idf
```

2. build the project
```bash
$ make
```

## Flash
```bash
$ make flash
```

## License

This project is licensed under the terms of the [MIT license](./LICENSE).
