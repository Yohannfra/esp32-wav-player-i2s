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

1. There is a small fix to make to *dr_wav.h* to be able to compile it, apply the patch **./dr_wav_patch_esp32.patch** to do it.

```bash
$ cd main/lib/dr_libs
$ git apply ../../../dr_wav_patch_esp32.patch
```

2. Move the file *./sample.wav* to your sdcard

## Build

```bash
$ maks
```

## Flash
```bash
$ make flash
```
