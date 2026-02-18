CC = gcc
CFLAGS = -g -DSTR_IMPLEMENTATION

STR_H = ../str.h
BUILD = build

LIFECYCLE = lifecycle
TRIMS = trims
APPENDS = appends
CHARS = chars
VIEWS = views

TARGETS = $(BUILD)/$(LIFECYCLE) \
          $(BUILD)/$(APPENDS) \
          $(BUILD)/$(TRIMS) \
          $(BUILD)/$(CHARS) \
          $(BUILD)/$(VIEWS)

.PHONY: all clean
all: $(BUILD) $(TARGETS)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%: %.c $(STR_H) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD)

