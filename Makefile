LIBS=-lglfw -lGLEW -lGL -ljack -lm -lfftw3
SRCS=spectrograph.cpp audio_buffer.cpp main.cpp stb_image.cpp opengl_utils.cpp
HEADERS=spectrograph.h audio_buffer.h opengl_utils.h

sndvew: $(SRCS) $(HEADERS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
