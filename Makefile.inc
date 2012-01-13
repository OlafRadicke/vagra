CXX=g++
VAGRA_CXXFLAGS+=-I/opt/gsd/include -fpic -DPIC -O5 -funroll-loops -m64 -Wall -pedantic
LDFLAGS+=-shared -L. -R. -L/opt/gsd/lib -R/opt/gsd/lib -ltntnet -lcxxtools -ltntdb -m64

MODULES = article comment user passwd
