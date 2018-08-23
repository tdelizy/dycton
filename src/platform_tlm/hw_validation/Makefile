MODULE = iss

SRCS = $(shell find . -name '*.cpp')

TARGET = run.x

all: $(TARGET)

ROOT=..
include $(ROOT)/Makefile.common

EXTRALDLIBS += -L../hardware -lhardware
CXXEXTRAFLAGS = -I../hardware
CEXTRAFLAGS = -I.


$(TARGET): $(OBJS) hardware
	$(LD) $(OBJS) -o $@ $(LDFLAGS) $(EXTRALDLIBS) $(LDLIBS)

%.o: %.c $(filter-out %.d, $(MAKEFILE_LIST))
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) $(CEXTRAFLAGS)


.PHONY: hardware
hardware:
	@cd .. && $(MAKE) hardware

