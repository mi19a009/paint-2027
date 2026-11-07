#!/usr/bin/make
# Make Locale

PO = ja.po
MO = $(PO:%.po=../bin/%/LC_MESSAGES/paint.mo)

.PHONY: build clean install uninstall
build: $(MO)
clean:
install:
uninstall:
	@$(RM) $(MO)
$(MO): ../bin/%/LC_MESSAGES/paint.mo: %.po
	@echo $(abspath $@)
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
