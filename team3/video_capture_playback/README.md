 
 Example GCC Compilation Command: for v4l2 capture_mp4
 
$ gcc -o capture_mp4 capture_mp4.c $(pkg-config --cflags --libs libavformat libavcodec libswscale libavutil)


1. Install SDL2 Development Libraries

sudo apt update
sudo apt install libsdl2-dev
gcc -o video_player video_player.c \
    $(pkg-config --cflags --libs sdl2) \
    $(pkg-config --cflags --libs libavformat libavcodec libswscale libavutil) \
    -lm -pthread

./video_player path/to/your/video.mp4




// main playback video


Install dependencies (on Ubuntu/Debian):
sudo apt install -y build-essential pkg-config libsdl2-dev libavformat-dev libavcodec-dev libswscale-dev libavutil-dev

/* compile code */
gcc play_mp4_video.c -o play_mp4_video \
    $(pkg-config --cflags --libs sdl2) \
    $(pkg-config --cflags --libs libavformat libavcodec libswscale libavutil) \
    -lm -pthread

    
//run the command
./play_mp4_video output.mp4


