CXX=g++
VAGRA_CXXFLAGS+=-I/opt/gsd/include -fpic -DPIC -O5 -funroll-loops -m64 -Wall -Wno-long-long -pedantic
LDFLAGS+=-shared -L. -R. -L/opt/gsd/lib -R/opt/gsd/lib -lcxxtools -ltntdb -m64

MODULES = article comment user passwd
