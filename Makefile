#!/usr/bin/make
# Make

LIBS   = $(shell pkg-config gtk4 --libs) -lpaint
CFLAGS = $(shell pkg-config gtk4 --cflags) -Wall -I../paint -L../bin
DEBUG   = -O0 -g -rdynamic
RELEASE = -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS
NATIVE  = -march=native -mtune=native
export CFLAGS LIBS

.PHONY: build clean install uninstall debug release native
all: release clean
build clean install uninstall:
	@cd locale && $(MAKE) $@
	@cd paint && $(MAKE) $@
	@cd viewer && $(MAKE) $@
debug:
	@$(MAKE) build "CFLAGS = $(CFLAGS) $(DEBUG)"
release:
	@$(MAKE) build "CFLAGS = $(CFLAGS) $(RELEASE)"
native:
	@$(MAKE) build "CFLAGS = $(CFLAGS) $(RELEASE) $(NATIVE)"
