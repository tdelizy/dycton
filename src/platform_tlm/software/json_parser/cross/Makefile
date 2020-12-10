
CROSS_COMPILE=mipsel-unknown-elf-
CROSS_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
CROSS_LDFLAGS=-EL

TARGET_CC = mipsel-unknown-elf-gcc
TARGET_AS = $(CROSS_COMPILE)as
TARGET_LD = $(CROSS_COMPILE)ld
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf

INCLUDE = -I. -I.. -I../../.. -I../../common -I../../common/legacy
TARGET_CFLAGS=-Wall -O2 -I. $(CROSS_CFLAGS) $(INCLUDE) -D$(DY_SOFT) -ggdb -ffreestanding
LIBGCC:=$(shell $(TARGET_CC) $(CFLAGS) -print-libgcc-file-name)

OBJS = main.o hal.o exception.o system.o libdycton.o newlib_malloc.o parson.o
EXEC = a.out

$(EXEC): $(OBJS) ../../common/ldscript
	$(TARGET_LD) -q $(CROSS_LDFLAGS) $(OBJS) -nostdlib -L../../common -o $@ -T../../common/ldscript -lc -u _printf_float -lm $(LIBGCC)
	$(TARGET_OBJDUMP) a.out -fpwd > a.asm
	$(TARGET_OBJDUMP) a.out -t > symbol_table.txt
	cp a.out ../../a.out

main.o: ../main.c ../../../address_map.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

parson.o: ../parson.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

hal.o: ../../common/hal.c ../../../address_map.h ../../common/hal.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

%.o: ../../common/%.c ../../../address_map.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

%.o: ../../common/legacy/%.c ../../../address_map.h
	$(TARGET_CC) $(TARGET_CFLAGS) -c $<

../../common/ldscript: ../../common/ldscript.c ../../../address_map.h # not intuitive, but its a way to define the stack address ONLY in address_map.h
	gcc -D$(DY_SOFT) $(INCLUDE) -P -E ../../common/ldscript.c -o ../../common/ldscript
