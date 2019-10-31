LIBS=-lglfw -lGLEW -lGL -ljack -lm -lfftw3
SRCS=main.cpp spectrograph.cpp 
HEADERS=spectrograph.h

sndvew: $(SRCS) $(HEADERS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
