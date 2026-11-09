#!/usr/bin/make
# Make Paint

OUT = ../bin/libpaint.a
OBJ = $(SRC:%.c=obj/%.o)
SRC = paint.c
INC = paint.h header.h.gch
override CFLAGS += $(shell pkg-config gtk4 --cflags)

.PHONY: build clean install uninstall
build: $(OUT)
clean:
	@$(RM) $(OUT) $(OBJ) header.h.gch
install:
uninstall:
$(OUT): $(OBJ)
	@ar rcs $(OUT) $(OBJ)
$(OBJ): obj/%.o: %.c $(INC)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -include header.h -o $@ $<
header.h.gch: header.h
	@echo $(abspath $@)
	@$(CC) $(CFLAGS) -x c-header -o $@ $<
