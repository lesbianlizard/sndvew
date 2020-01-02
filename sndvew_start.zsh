#!/usr/bin/env zsh

jack_control start

cd /home/gandalfs_cat/sndvew
killall carla-patchbay
carla-patchbay -n ./filtering.carxp &
./sndvew &
sleep 1
sleep 5
jack_connect system:capture_1 Carla:audio-in1
jack_connect system:capture_2 Carla:audio-in2
jack_connect Carla:audio-out1 sndvew:mic1
jack_connect Carla:audio-out2 sndvew:mic2
sleep 5
xdotool mousemove 1920 1080
