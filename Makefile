CXXFLAGS = -Os -std=c++11 -Wall -pedantic
CFLAGS = -Os -Wall -pedantic

default: pikchr
all: default README.md

pikchr: main.o pikchr.o
	rm -f $@
	$(CXX) -o $@ $+ -lm

pikchr.o: drhsqlite-pikchr/pikchr.c
	$(CC) $(CFLAGS) -c -o pikchr.o $+

README.md: README.md.in pikchr
	./pikchr < README.md.in > README.md

clean:
	rm -f pikchr *.o
