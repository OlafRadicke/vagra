include ../Makefile.inc

CXXFLAGS  = -I../include $(VAGRA_CXXFLAGS)

all: baseobject.o date.o helpers.o nexus.o config.o
	cp *.o ../

clean:
	rm -f *.so *.o
