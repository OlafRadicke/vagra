include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)

all: article.o cachedarticle.o articlesearch.o tagsumpublic.o
	cp *.o ../../

clean:
	rm -f *.so *.o
