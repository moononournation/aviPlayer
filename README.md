# AVI Player

A simple AVI player for Arduino IDE.

## Dependent Libraries

- Dev Device Pins: <https://github.com/moononournation/Dev_Device_Pins.git>
- Arduino_GFX: <https://github.com/moononournation/Arduino_GFX.git>
- avilib: <https://github.com/lanyou1900/avilib.git>
- JPEGDEC: <https://github.com/bitbank2/JPEGDEC.git>
- ESP32_JPEG: <https://github.com/esp-arduino-libs/ESP32_JPEG.git>
- arduino-libhelix: <https://github.com/pschatzmann/arduino-libhelix.git>

## conversion

### Cinepak

```sh
ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 10 -vf "fps=30,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviMp3Cinepak240p30fps.avi

ffmpeg -y -i input.mp4 -ac 1 -ar 44100 -af loudnorm -c:a pcm_u8 -c:v cinepak -q:v 10 -vf "fps=30,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Cinepak240p30fps.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 10 -vf "fps=30,scale=-1:272:flags=lanczos,crop=480:272:(in_w-480)/2:0" AviMp3Cinepak272p30fps.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 100 -vf "fps=30,scale=-1:600:flags=lanczos,crop=1024:600:(in_w-1024)/2:0" AviMp3Cinepak1024x600.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 10 -vf "scale=-1:360:flags=lanczos,crop=360:360:(in_w-360)/2:0" AviMp3Cinepak360sq.avi
```

### Cinepak color screen for debug only

```sh
ffmpeg -f lavfi -i color=red:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" red.avi

ffmpeg -f lavfi -i color=green:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" green.avi

ffmpeg -f lavfi -i color=blue:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" blue.avi

ffmpeg -f lavfi -i color=white:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" white.avi
```

### MJPEG

```sh
ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviMp3Mjpeg240p15fps.avi

ffmpeg -y -i input.mp4 -ac 1 -ar 44100 -af loudnorm -c:a pcm_u8 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Mjpeg240p15fps.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:272:flags=lanczos,crop=480:272:(in_w-480)/2:0" AviMp3Mjpeg272p15fps.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 7 -vf "scale=-1:360:flags=lanczos,crop=360:360:(in_w-360)/2:0" AviMp3Mjpeg360sq.avi

ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=12,scale=-1:320:flags=lanczos,crop=480:320:(in_w-480)/2:0" AviMp3Mjpeg480x320.avi
```

### AVI for multi display

```sh
ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 20 -vf "fps=10,scale=800:-1:flags=lanczos,crop=800:400:0:(in_h-400)/2" AviMp3Cinepak400p10fps.avi
```

### AVI for iBubly

```sh
ffmpeg -y -i input.mp4 -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:240:flags=lanczos,crop=288:240:(in_w-288)/2:0" AviMp3Mjpeg288x240.avi
```

### AVI for ESP32-C3

```sh
ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v cinepak -q:v 20 -vf "fps=12,scale=-1:240:flags=lanczos,crop=288:240:(in_w-288)/2:0" AviMp3Cinepak288x240.avi

ffmpeg -y -i input.mp4 -ac 1 -ar 22050 -af loudnorm -c:a pcm_u8 -c:v cinepak -q:v 20 -vf "fps=15,scale=-1:240:flags=lanczos,crop=288:240:(in_w-288)/2:0" AviPcmu8Cinepak288x240.avi

ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=10,scale=-1:240:flags=lanczos,crop=288:240:(in_w-288)/2:0" AviMp3Mjpeg288x240.avi

ffmpeg -y -i input.mp4 -ac 1 -ar 22050 -af loudnorm -c:a pcm_u8 -c:v mjpeg -q:v 9 -vf "fps=12,scale=-1:240:flags=lanczos,crop=288:240:(in_w-288)/2:0" AviPcmu8Mjpeg288x240.avi
```

### AVI for T-Display S3 Pro

```sh
ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=15,scale=480:-1:flags=lanczos,crop=480:220:0:(in_h-220)/2" AviMp3Mjpeg480x220.avi
```

### AVI for ESP32-1732S019

```sh
ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=15,scale=320:-1:flags=lanczos,crop=320:170:0:(in_h-172)/2" AviMp3Mjpeg320x170.avi

ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=15,scale=320:-1:flags=lanczos,crop=320:172:0:(in_h-172)/2" AviMp3Mjpeg320x172.avi

ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=15,scale=-1:320:flags=lanczos,crop=170:320:(in_w-170)/2:0" AviMp3Mjpeg170x320.avi

ffmpeg -y -i input.mp4 -ab 32k -ac 2 -ar 44100 -af loudnorm -c:a mp3 -c:v mjpeg -q:v 9 -vf "fps=15,scale=-1:320:flags=lanczos,crop=172:320:(in_w-172)/2:0" AviMp3Mjpeg172x320.avi
```
