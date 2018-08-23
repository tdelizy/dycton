
CROSS_COMPILE=mipsel-unknown-elf-
CROSS_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
CROSS_LDFLAGS=-EL

TARGET_CC = mipsel-unknown-elf-gcc
TARGET_AS = $(CROSS_COMPILE)as
TARGET_LD = $(CROSS_COMPILE)ld
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf

INCLUDE = -I. -I.. -I../../.. -I../../common -I../../common/legacy -I../jpeg-6a
TARGET_CFLAGS=-Wall -O3 $(CROSS_CFLAGS) $(INCLUDE) -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) -ggdb
LIBGCC:=$(shell $(TARGET_CC) $(CFLAGS) -print-libgcc-file-name)

OBJS = wrppm.o cdjpeg.o rdppm.o main.o hal.o exception.o system.o libdycton.o newlib_malloc.o
EXEC = a.out

$(EXEC): $(OBJS) ../../common/ldscript
	$(TARGET_LD) -q $(CROSS_LDFLAGS) $(OBJS) -o $@ -T../../common/ldscript -L../jpeg-6a/ -ljpeg  -L../../common -lc -lm $(LIBGCC)
	cp a.out ../../a.out

main.o: ../main.c ../../../address_map.h ../../common/hal.h ../jpeg_app.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

rdppm.o: ../rdppm.c ../jpeg-6a/jinclude.h ../jpeg-6a/jconfig.h ../jpeg-6a/jpeglib.h ../jpeg-6a/jmorecfg.h ../jpeg-6a/jerror.h ../jpeg-6a/cderror.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

cdjpeg.o: ../cdjpeg.c ../cdjpeg.h ../jpeg-6a/jinclude.h ../jpeg-6a/jconfig.h ../jpeg-6a/jpeglib.h ../jpeg-6a/jmorecfg.h ../jpeg-6a/jerror.h ../jpeg-6a/cderror.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

wrppm.o: ../wrppm.c ../cdjpeg.h ../jpeg-6a/jinclude.h ../jpeg-6a/jconfig.h ../jpeg-6a/jpeglib.h ../jpeg-6a/jmorecfg.h ../jpeg-6a/jerror.h ../jpeg-6a/cderror.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

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
