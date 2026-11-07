#!/usr/bin/make
# Make Viewer

OUT      = ../bin/viewer
OBJ      = $(SRC:%.c=obj/%.o)
SRC      = app.c resource.c viewer.c window.c
INC      = viewer.h header.h.gch ../paint/paint.h
RESOURCE = menus.ui viewer.svg
PREFIX   = $(HOME)/.local
DESKTOP  = $(PREFIX)/share/applications/com.github.mi19a009.PictureViewer.desktop
SCHEMA   = $(PREFIX)/share/glib-2.0/schemas/com.github.mi19a009.PictureViewer.gschema.xml
EXEC     = $(abspath ../bin)
ICON     = $(abspath .)

.PHONY: build clean install uninstall
build: $(OUT)
clean:
	@$(RM) $(OBJ) resource.c header.h.gch viewer.gschema.valid
install: $(DESKTOP) $(SCHEMA)
	@glib-compile-schemas $(dir $(SCHEMA))
uninstall:
	@$(RM) $(OUT) $(DESKTOP) $(SCHEMA)
	@glib-compile-schemas $(dir $(SCHEMA))
$(DESKTOP): viewer.desktop
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@sed -e "s#Exec=#Exec=$(EXEC)/#" -e "s#Icon=#Icon=$(ICON)/#" $< > $@
$(SCHEMA): viewer.gschema.xml viewer.gschema.valid
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@cp $< $@
$(OUT): $(OBJ)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): obj/%.o: %.c $(INC)
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -include header.h -o $@ $<
resource.c: viewer.gresource.xml $(RESOURCE)
	@echo $(abspath $@)
	@glib-compile-resources --generate-source --target $@ $<
header.h.gch: header.h
	@echo $(abspath $@)
	@$(CC) $(CFLAGS) -x c-header -o $@ $<
viewer.gschema.valid: viewer.gschema.xml
	@echo $(abspath $@)
	@glib-compile-schemas --strict --dry-run --schema-file=$<
	@touch $@
