CFLAGS = -Os -Wall -pedantic

default: pikchr
all: default README.md usage.svg

pikchr: main.o pikchr.o
	rm -f $@
	$(CC) -o $@ $+ -lm

README.md: README.md.in pikchr
	./pikchr < README.md.in > README.md

usage.svg: README.md.in pikchr
	./pikchr -qb -n 1 < README.md.in > usage.svg

clean:
	rm -f pikchr *.o
