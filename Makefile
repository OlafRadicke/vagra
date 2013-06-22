include Makefile.inc

all: libvagra.so

SUBDIRS = src modules

.PHONY: vagra_objects $(SUBDIRS)

vagra_objects: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

libvagra.so: vagra_objects 
	${CXX} -Wl,--whole-archive -o $@ src/vagra.a $$(for module in $(MODULES); do echo modules/$${module}/$${module}.a; done) -Wl,--no-whole-archive ${LDFLAGS}

clean:
	rm -f libvagra.so
	-for d in $(SUBDIRS); do cd $$d && $(MAKE) clean; cd -; done
