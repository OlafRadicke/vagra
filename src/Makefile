include ../Makefile.inc

CXXFLAGS  = -I../include $(VAGRA_CXXFLAGS)

OBJECTS  = nexus.o config.o date.o utils.o
OBJECTS += context.o cachedcontext.o baseobject.o
OBJECTS += page.o search.o basesearchvpsu.o

all: vagra.a

vagra.a: ${OBJECTS}
	$(AR) -rcs $@ $(OBJECTS)

clean:
	-rm -f *.o *.a
