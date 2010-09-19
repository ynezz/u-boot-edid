CFLAGS = -W -Wall -g -O2

all: edid

edid: edid.o main.o
	$(CC) $(CFLAGS) -o edid edid.o main.o

clean:
	-rm -f core core.* a.out edid.o main.o edid
