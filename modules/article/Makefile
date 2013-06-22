include ../../Makefile.inc

CXXFLAGS  = -I../../include $(VAGRA_CXXFLAGS)
OBJECTS     = article.o cachedarticle.o articlesearch.o tagsum.o

all: article.a

article.a: $(OBJECTS)
	$(AR) -rcs $@ $(OBJECTS)

clean:
	-rm -f *.o *.a
