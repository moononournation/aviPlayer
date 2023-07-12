# AVI Player

A simple AVI player for Arduino IDE.

## conversion

```sh
ffmpeg -i input.mp4 -c:a mp3 -c:v mjpeg -q:v 10 -vf "fps=10,scale=-1:320:flags=lanczos,crop=480:320:(in_w-480)/2:0" AviMp3Mjpeg320p10fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v cinepak -q:v 10 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Cinepak240p15fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v mjpeg -q:v 10 -vf "fps=15,scale=-1:240:flags=lanczos,crop=320:240:(in_w-320)/2:0" AviPcmu8Mjpeg240p15fps.avi

ffmpeg -i input.mp4 -ac 1 -c:a pcm_u8 -c:v mjpeg -q:v 10 -vf "fps=10,scale=-1:320:flags=lanczos,crop=480:320:(in_w-480)/2:0" AviPcmu8Mjpeg320p10fps.avi
```