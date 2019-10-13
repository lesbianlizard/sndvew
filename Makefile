LIBS=-lglfw -lGLEW -lGL -ljack -lm
SRCS=main.cpp

sndvew: $(SRCS)
	 g++ -ggdb -Wall $(LIBS) $(SRCS) -o sndvew

.PHONY: clean
clean:
	rm sndvew
