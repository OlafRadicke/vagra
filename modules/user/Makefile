include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)

all: user.o cacheduser.o userpage.o
	cp *.o ../../

clean:
	rm -f *.so *.o
