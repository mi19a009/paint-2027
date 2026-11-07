#!/usr/bin/make
# Make

LIBS   = -lpaint
CFLAGS = -I../paint -L../bin
export CFLAGS LIBS

.PHONY: build
build:
	@cd paint && $(MAKE) $@
	@cd viewer && $(MAKE) $@
