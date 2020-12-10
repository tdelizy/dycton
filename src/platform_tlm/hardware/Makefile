MODULE = hardware

SRCS = $(shell find . -name '*.cpp')
TARGET = libhardware.a

ROOT=..
include $(ROOT)/Makefile.common

EXTRALDLIBS += -L../hardware -lhardware -L../elf-loader -lloader
CXXEXTRAFLAGS = -I../hardware -I.. -D$(DY_SOFT)
CEXTRAFLAGS = -I.

%.o: %.cpp $(filter-out %.d, $(MAKEFILE_LIST))
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS) $(CXXEXTRAFLAGS)


