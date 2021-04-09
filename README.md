# Vita-Media-Player
An Open Source PSVita/TV MP4 player with 1080p playback and subtitle support

1080i output supported on the PSTV natively and on the Vita with Sharpscale

SRT subtitle support (Has to be the same name as the video file)

MKVs do NOT work. If you want to preserve subtitles with MKVs I recommend using Handbrake to convert them to MP4 with Timed Text Subtitles. Direct FFMPEG will also work.


![image](https://user-images.githubusercontent.com/40341589/113678803-ac918700-9684-11eb-9581-2c76fce52e46.png)

![image](https://user-images.githubusercontent.com/40341589/113678919-d185fa00-9684-11eb-9dc9-2c30f9f49daf.png)

![image](https://user-images.githubusercontent.com/40341589/113679024-e9f61480-9684-11eb-8cb7-0afaa817d4a0.png)


## Note to Devs
SceAvPlayer does not support 1080p video playback by default. You need to patch the module in order to do so. That's what [ReAvPlayer](https://github.com/SonicMastr/ReAvPlayer) is for.

## Special Thanks

#### 1080p Video Playback was made possible by GrapheneCt and his Reverse Engineering efforts into SceAvPlayer<br><br>Cuevavirus for Sharpscale providing 1080i support<br><br>Joel16 for the original ElevenMPV file browser used as a base (I can't do UI)<br><br>SomeonPC for Livearea Assets<br><br>And Usagi for constantly getting on my ass to get this to work.
