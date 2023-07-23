# AVI Player

A simple AVI player for Arduino IDE.

## conversion

```sh
ffmpeg -i input.mp4 -c:a mp3 -c:v cinepak -q:v 10 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviMp3Cinepak240p15fps.avi

ffmpeg -i input.mp4 -c:a mp3 -c:v mjpeg -q:v 10 -vf "fps=10,scale=-1:320:flags=lanczos,crop=480:320:(in_w-480)/2:0" AviMp3Mjpeg320p10fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v mjpeg -q:v 10 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Mjpeg240p15fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v mjpeg -q:v 10 -vf "fps=10,scale=-1:320:flags=lanczos,crop=480:320:(in_w-480)/2:0" AviPcmu8Mjpeg320p10fps.avi
```

## color screen for debug only

```sh
ffmpeg -f lavfi -i color=red:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" red.avi

ffmpeg -f lavfi -i color=green:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" green.avi

ffmpeg -f lavfi -i color=blue:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" blue.avi

ffmpeg -f lavfi -i color=white:320x240:d=3,format=rgb24 -c:v cinepak -q:v 10 -vf "fps=15" white.avi
```
