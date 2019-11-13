LIBS=-lglfw -lGLEW -lGL -ljack -lm -lfftw3
SRCS=spectrograph.cpp audio_buffer.cpp main.cpp stb_image.cpp
HEADERS=spectrograph.h audio_buffer.h

sndvew: $(SRCS) $(HEADERS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
