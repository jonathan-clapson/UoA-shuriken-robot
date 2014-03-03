SOURCES=main.cpp tlc5940.cpp timespec_math.cpp bcm2835_timer.cpp

all:
	g++ -o hardwarepwmtest $(SOURCES) -lbcm2835 -lpthread -lrt

debug:
	g++ -g -o hardwarepwmtest $(SOURCES) -lbcm2835 -lpthread -lrt
