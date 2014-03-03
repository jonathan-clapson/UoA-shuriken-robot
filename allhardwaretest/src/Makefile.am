AUTOMAKE_OPTIONS = subdir-objects

## ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

AM_CPPFLAGS = $(SENSORTEST_CFLAGS)

bin_PROGRAMS = sensortest

sensortest_SOURCES = main.cpp adc/adcSpi.cpp adc/adcSpi.h mouse/mouse.cpp mouse/mouse.h tlc5940/tlc5940.cpp tlc5940/tlc5940.h tlc5940/bcm2835_timer.cpp tlc5940/bcm2835_timer.h tlc5940/timespec_math.cpp tlc5940/timespec_math.h tlc5940/tlc5940_pin_mapping.h adc/aps00b_calculation.cpp adc/aps00b_calculation.h
sensortest_LDADD = -lbcm2835 -lpthread -lm
