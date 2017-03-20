CFLAGS=-I../../include -std=c++11 -fopenmp
LDFLAGS=../../lib/libroutingkit.a -lm -lz -pthread

all:
	g++ -o index-load-from-disk ${CFLAGS} index-load-from-disk.cpp ${LDFLAGS}
	g++ -o index-save-to-disk ${CFLAGS} index-save-to-disk.cpp ${LDFLAGS}

clean:
	rm index-load-from-disk index-save-to-disk
