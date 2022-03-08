#
#

exe: main.o math.o
	cc -o exe  math.o main.o

math.o: math.c
	cc -c math.c

main.o: main.c
	cc -c main.c

clean:
	rm -f exe *.o
