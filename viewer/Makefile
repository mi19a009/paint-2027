#!/usr/bin/make
# Make Viewer

OUT = ../bin/viewer
OBJ = $(SRC:%.c=obj/%.o)
SRC = viewer.c

.PHONY: build
build: $(OUT)
$(OUT): $(OBJ)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): obj/%.o: %.c
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
