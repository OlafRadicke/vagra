include ../Makefile.inc

CXXFLAGS  = -I../include $(VAGRA_CXXFLAGS)

all: baseobject.o date.o helpers.o nexus.o
	cp *.o ../

clean:
	rm -f *.so *.o
