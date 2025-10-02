# Try to get a tag description; if that fails, use branch name + commit
VERSION := $(shell \
    git describe --tags --dirty 2>/dev/null || \
    git describe --all --long --dirty 2>/dev/null | sed -E 's|heads/||; s|-0-|-|' \
)
CFLAGS = -Os -Wall -pedantic -DPIKCHR_CMD_VERSION=\"$(VERSION)\"

default: pikchr
all: default version README.md usage.svg

pikchr: main.o pikchr.o
	rm -f $@
	$(CC) -o $@ main.o pikchr.o -lm

version:
	./pikchr -v

README.md: README.md.in pikchr
	./pikchr -S 'title="Click Me!" style="font-size: smaller"' < README.md.in > README.md

usage.svg: README.md.in pikchr
	./pikchr -qb -N @usage -a 'style="font-size:initial;font-family:sans-serif;background-color:white"' < README.md.in > usage.svg

clean:
	rm -f pikchr *.o
