include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)

all: passwd.o cachedpasswd.o auth.o
	cp *.o ../../

clean:
	rm -f *.so *.o
