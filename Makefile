include Makefile.inc

all: libvagra.so

libvagra.so: vagra_modules
	${CXX} -o $@ ${LDFLAGS} *.o

vagra:
	cd src && $(MAKE)

vagra_modules: vagra
	for d in $(MODULES); do cd ./modules/$$d && $(MAKE) || exit 1; cd -; done

clean:
	rm -f *.so *.o
	cd src && $(MAKE) clean
	for d in $(MODULES); do cd ./modules/$$d && $(MAKE) clean; cd -; done
