# AVI Player

A simple AVI player for Arduino IDE.

## conversion

### Cinepak

```sh
ffmpeg -y -i input.mp4 -c:a mp3 -c:v cinepak -q:v 10 -vf "fps=30,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviMp3Cinepak240p30fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v cinepak -q:v 10 -vf "fps=30,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Cinepak240p30fps.avi

ffmpeg -y -i input.mp4 -c:a mp3 -c:v cinepak -q:v 20 -vf "fps=30,scale=-1:272:flags=lanczos,crop=480:272:(in_w-480)/2:0" AviMp3Cinepak272p30fps.avi

ffmpeg -y -i input.mp4 -c:a mp3 -c:v cinepak -q:v 20 -vf "fps=10,scale=800:-1:flags=lanczos,crop=800:400:0:(in_h-400)/2" AviMp3Cinepak400p10fps.avi
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
ffmpeg -i input.mp4 -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviMp3Mjpeg240p15fps.avi

ffmpeg -i input.mp4 -c:a mp3 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:272:flags=lanczos,crop=480:272:(in_w-480)/2:0" AviMp3Mjpeg272p15fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v mjpeg -q:v 7 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Mjpeg240p15fps.avi
```
