#!/usr/bin/make
# Make

LIBS   = $(shell pkg-config gtk4 --libs) -lpaint
CFLAGS = $(shell pkg-config gtk4 --cflags) -Wall -I../paint -L../bin
export CFLAGS LIBS

.PHONY: build clean
build clean:
	@cd locale && $(MAKE) $@
	@cd paint && $(MAKE) $@
	@cd viewer && $(MAKE) $@
