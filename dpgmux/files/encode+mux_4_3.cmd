ffmpeg -i %1 -s 256x192 -vcodec mpeg1video -b 448k %1.m1v
ffmpeg -i %1 -acodec mp2 -ab 128k -ar 32000 -ac 2  %1.mp2
dpgmux %1.m1v %1.mp2
del %1.m1v
del %1.mp2