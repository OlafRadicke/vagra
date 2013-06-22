include ../Makefile.inc

.PHONY: vagra_modules $(MODULES)

vagra_modules: $(MODULES)

$(MODULES):
	$(MAKE) -C $@ 

clean:
	-for d in $(MODULES); do cd $$d && $(MAKE) clean; cd -; done
