CC = gcc
CFLAGS = -x c -DSTR_IMPLEMENTATION -fPIC -shared
debug ?= 0

ifeq ($(debug),1)
	CFLAGS += -g -O0
else
	CFLAGS += -O2
endif

STR_H = str.h
BUILDF = build
TARGET = $(BUILDF)/libstr.so

all: $(TARGET)

$(BUILDF):
	mkdir -p $(BUILDF)

$(TARGET): $(STR_H) | $(BUILDF)
	$(CC) $(CFLAGS) $(STR_H) -o $(TARGET)

clean:
	rm -rf $(BUILDF)
