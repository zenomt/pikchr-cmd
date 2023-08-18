CFLAGS = -Os -Wall -pedantic

default: pikchr
all: default README.md usage.svg

pikchr: main.o pikchr.o
	rm -f $@
	$(CC) -o $@ main.o pikchr.o -lm

README.md: README.md.in pikchr
	./pikchr < README.md.in > README.md

usage.svg: README.md.in pikchr
	./pikchr -qb -N @usage -a 'style="font-family:sans-serif;background-color:white"' < README.md.in > usage.svg

clean:
	rm -f pikchr *.o
