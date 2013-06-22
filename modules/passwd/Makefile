include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)
OBJECTS     = passwd.o

all: passwd.a

passwd.a: $(OBJECTS)
	$(AR) -rcs $@ $(OBJECTS)

clean:
	-rm -f *.o *.a
