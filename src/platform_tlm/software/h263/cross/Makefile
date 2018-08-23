# Makefile for TMN simulation model by kol@nta.no

# This is free software. See the file COPYING for copying conditions.


CROSS_COMPILE=mipsel-unknown-elf-
CROSS_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
CROSS_LDFLAGS=-EL

TARGET_CC = mipsel-unknown-elf-gcc
TARGET_AS = $(CROSS_COMPILE)as
TARGET_LD = $(CROSS_COMPILE)ld
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf

INCLUDE = -I. -I.. -I../../.. -I../../common -I../../common/legacy
TARGET_CFLAGS=-Wall -O3 $(CROSS_CFLAGS) $(INCLUDE) -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) -ggdb
LIBGCC:=$(shell $(TARGET_CC) $(CFLAGS) -print-libgcc-file-name)

EXEC =  a.out
SRCS =	main.c io.c Makefile dct.c coder.c quant.c mot_est.c pred.c snr.c \
        countbit.c putbits.c ratectrl.c sac.c README COPYING \
        TODO CHANGES putvlc.c
HDRS = 	sim.h macros.h config.h \
        sactbls.h indices.h putvlc.h vlc.h
OBJS =	main.o io.o dct.o coder.o quant.o mot_est.o pred.o  snr.o \
        countbit.o putbits.o ratectrl.o sac.o putvlc.o \
        hal.o exception.o system.o libdycton.o newlib_malloc.o

all: $(EXEC)

$(EXEC): $(OBJS) ../../common/ldscript
	$(TARGET_LD) -q $(CROSS_LDFLAGS) $(OBJS) -o $@ -T../../common/ldscript -L../../common -lc -lm $(LIBGCC)
	cp a.out ../../a.out


main.o:		../main.c ../sim.h ../config.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

io.o:		../io.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

coder.o:	../coder.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

dct.o:		../dct.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

quant.o:	../quant.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

pred.o:		../pred.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

mot_est.o:	../mot_est.c ../sim.h ../config.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

snr.o:		../snr.c ../sim.h ../config.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

putbits.o:	../putbits.c ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

putvlc.o:	../putvlc.c ../vlc.h ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

countbit.o:	../countbit.c ../sim.h ../sactbls.h ../indices.h ../putvlc.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

ratectrl.o:	../ratectrl.c ../sim.h ../config.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

sac.o:      ../sac.c ../sactbls.h ../sim.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

hal.o: ../../common/hal.c ../../../address_map.h ../../common/hal.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

%.o: ../../common/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

%.o: ../../common/legacy/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

../../common/ldscript: ../../common/ldscript.c ../../../address_map.h # not intuitive, but its a way to define the stack address ONLY in address_map.h
	gcc -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) $(INCLUDE) -P -E ../../common/ldscript.c -o ../../common/ldscript

check-env:
ifndef DY_ARCH
    DY_ARCH=-1
endif
