include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)

all: article.o cachedarticle.o articlesearch.o tagsum.o
	cp *.o ../../

clean:
	rm -f *.so *.o
