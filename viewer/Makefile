#!/usr/bin/make
# Make Viewer

OUT = ../bin/viewer
OBJ = $(SRC:%.c=obj/%.o)
SRC = app.c viewer.c window.c
INC = viewer.h header.h.gch ../paint/paint.h

.PHONY: build clean
build: $(OUT)
clean:
	@$(RM) $(OBJ) header.h.gch
$(OUT): $(OBJ)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): obj/%.o: %.c $(INC)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -include header.h -o $@ $<
header.h.gch: header.h
	@echo $(abspath $@)
	@$(CC) $(CFLAGS) -x c-header -o $@ $<
