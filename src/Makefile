include ../Makefile.inc

CXXFLAGS  = -I../include $(VAGRA_CXXFLAGS)

OBJECTS  = nexus.o config.o date.o helpers.o
OBJECTS += context.o contextcache.o cachedcontext.o baseobject.o
OBJECTS += page.o search.o

all: ${OBJECTS}
	cp *.o ../

clean:
	rm -f *.so *.o
