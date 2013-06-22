include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)
OBJECTS     = cachedcommet.o comment.o

all: comment.a

comment.a: $(OBJECTS)
	$(AR) -rcs $@ $(OBJECTS)
	
clean:
	-rm -f *.o *.a
