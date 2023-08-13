CFLAGS = -Os -Wall -pedantic

default: pikchr
all: default README.md usage.svg

pikchr: main.o pikchr.o
	rm -f $@
	$(CC) -o $@ main.o pikchr.o -lm

README.md: README.md.in pikchr
	./pikchr < README.md.in > README.md

usage.svg: README.md.in pikchr
	./pikchr -qb -N @usage < README.md.in | sed -e '/^<svg /s//<svg style="font-family:sans-serif" /' > usage.svg

clean:
	rm -f pikchr *.o
