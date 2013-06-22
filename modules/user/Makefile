include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)
OBJECTS     = user.o cacheduser.o auth.o

all: user.a

user.a: $(OBJECTS)
	$(AR) -rcs $@ $(OBJECTS)

clean:
	-rm -f *.o *.a
