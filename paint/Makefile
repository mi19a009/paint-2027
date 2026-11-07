#!/usr/bin/make
# Make Paint

OUT = ../bin/libpaint.a
OBJ = $(SRC:%.c=obj/%.o)
SRC = paint.c

.PHONY: build
build: $(OUT)
clean:
	@$(RM) $(OUT) $(OBJ)
$(OUT): $(OBJ)
	@ar rcs $(OUT) $(OBJ)
$(OBJ): obj/%.o: %.c
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
