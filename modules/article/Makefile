include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)

all: article.o articlecache.o articlepage.o cachedarticle.o
	cp *.o ../../

clean:
	rm -f *.so *.o
