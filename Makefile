LIBS=-lglfw -lGLEW -lGL -ljack -lm -lfftw3f
SRCS=main.cpp

sndvew: $(SRCS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
