SOURCES=main.c pwm.c motor.c
HEADERS=pwm.h motor.h

all: $(SOURCES) $(HEADERS)
	gcc -Wall -o main $(SOURCES) -lpthread -lbcm2835

debug: $(SOURCES) $(HEADERS)
	gcc -g -Wall -o main $(SOURCES) -lpthread -lbcm2835


