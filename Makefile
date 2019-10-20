LIBS=-lglfw -lGLEW -lGL -ljack -lm -lfftw3
SRCS=main.cpp spectrograph.cpp 

sndvew: $(SRCS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
