
CROSS_COMPILE=mipsel-unknown-elf-
CROSS_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
CROSS_LDFLAGS=-EL

TARGET_CC = mipsel-unknown-elf-gcc
TARGET_AS = $(CROSS_COMPILE)as
TARGET_LD = $(CROSS_COMPILE)ld
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf

INCLUDE = -I.. -I../../.. -I../../common -I../../common/legacy -I../include
TARGET_CFLAGS=-Wall -O3 -I. $(CROSS_CFLAGS) $(INCLUDE) -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) -ggdb -ffreestanding
LIBGCC:=$(shell $(TARGET_CC) $(CFLAGS) -print-libgcc-file-name)

OBJS = ecdsa.o hal.o exception.o system.o libdycton.o newlib_malloc.o
EXEC = a.out

DEP=../library/libmbedcrypto.a ../library/libmbedx509.a ../library/libmbedtls.a

$(EXEC): $(OBJS) ../../common/ldscript $(DEP)
	$(TARGET_LD) -q $(CROSS_LDFLAGS) $(OBJS) -nostdlib -L../library -lmbedtls -lmbedx509 -lmbedcrypto -L../../common -lc $(LIBGCC) -o $@ -T../../common/ldscript
	$(TARGET_OBJDUMP) a.out -fpwd > a.asm
	$(TARGET_OBJDUMP) a.out -t > symbol_table.txt
	cp a.out ../../a.out

ecdsa.o: ../ecdsa.c ../../../address_map.h $(DEP)
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

hal.o: ../../common/hal.c ../../../address_map.h ../../common/hal.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

%.o: ../../common/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

%.o: ../../common/legacy/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

$(DEP):
	$(MAKE) -C ../library

../../common/ldscript: ../../common/ldscript.c ../../../address_map.h # not intuitive, but its a way to define the stack address ONLY in address_map.h
	gcc -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) $(INCLUDE) -P -E ../../common/ldscript.c -o ../../common/ldscript

check-env:
ifndef DY_ARCH
    DY_ARCH=-1
endif



pkey/ecdsa$(EXEXT): pkey/ecdsa.c $(DEP)
	echo "  CC    pkey/ecdsa.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) pkey/ecdsa.c       $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@