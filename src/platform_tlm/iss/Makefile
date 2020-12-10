MODULE = iss

ESOFT_BIN = ../software/a.out

SRCS = $(shell find . -name '*.cpp')

TARGET = run.x

all: $(TARGET) $(ESOFT_BIN)

ROOT=..
include $(ROOT)/Makefile.common

EXTRALDLIBS += -L../hardware -lhardware -L../elf-loader -lloader
CXXEXTRAFLAGS = -I../hardware -I.. -D$(DY_SOFT)
CEXTRAFLAGS = -I.


$(TARGET): $(OBJS) hardware lib-loader check-env software
	@echo "\033[01;34m"
	@echo "--------------------------------------------------"
	@echo "[DYCTON] simulator run.x link "
	@echo "--------------------------------------------------"
	@echo "\033[0m"
	$(LD) $(ESOFT_OBJS) $(OBJS) -o $@ $(LDFLAGS) $(EXTRALDLIBS) $(LDLIBS)


.PHONY: software
software: sc_main_iss.o
	@echo "\033[01;34m"
	@echo "--------------------------------------------------"
	@echo "[DYCTON] targeted software build: " $(DY_SOFT)
	@echo "--------------------------------------------------"
	@echo "\033[0m"
	@cd ../software/$(DY_SOFT)/cross && $(MAKE) DY_SOFT=$(DY_SOFT)


.PHONY: hardware lib-loader
hardware:
	@echo "\033[01;34m"
	@echo "--------------------------------------------------"
	@echo "[DYCTON] simulation hardware components build"
	@echo "--------------------------------------------------"
	@echo "\033[0m"
	@cd ../hardware && $(MAKE) DY_SOFT=$(DY_SOFT)

lib-loader:
	@echo "\033[01;34m"
	@echo "--------------------------------------------------"
	@echo "[DYCTON] elf-loader build"
	@echo "--------------------------------------------------"
	@echo "\033[0m"
	@cd ../elf-loader/ && $(MAKE)

sc_main_iss.o: sc_main_iss.cpp FORCE
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS) $(CXXEXTRAFLAGS)

.PHONY: FORCE
FORCE:


check-env:
ifndef DY_SOFT
    $(error DY_SOFT must be set (for example DY_SOFT=dummy_memset))
else
    ifeq ($(wildcard ../software/$(DY_SOFT)/.*),)
    	$(error DY_SOFT directory doesn't exist)
    endif
endif

# Include dependency files
-include $(ESOFT_DEPS)


