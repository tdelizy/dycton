
# Also see "include/mbedtls/config.h"
CROSS_COMPILE=mipsel-unknown-elf-
CROSS_CFLAGS=-mips2 -mno-branch-likely -gstabs+ -DSOCLIB_MIPS_R3000 -EL
CROSS_LDFLAGS=-EL

TARGET_CC = mipsel-unknown-elf-gcc
TARGET_AS = $(CROSS_COMPILE)as
TARGET_LN = $(CROSS_COMPILE)ln
TARGET_AR = $(CROSS_COMPILE)ar
TARGET_OBJDUMP = $(CROSS_COMPILE)objdump
TARGET_READELF = $(CROSS_COMPILE)readelf

INCLUDE = -I. -I.. -I../../.. -I../../common -I../../common/legacy
TARGET_CFLAGS=-Wall -O3 $(CROSS_CFLAGS) $(INCLUDE) -D$(DY_SOFT) -DDY_ARCH=$(DY_ARCH) -ggdb
LIBGCC:=$(shell $(TARGET_CC) $(CFLAGS) -print-libgcc-file-name)

CFLAGS	?= -O2
WARNING_CFLAGS ?=  -Wall -W -Wdeclaration-after-statement
LDFLAGS ?=

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I../include -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS =

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

# if were running on Windows build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
endif

# To compile as a shared library:
ifdef SHARED
# all code is position-indep with mingw, avoid warning about useless flag
ifndef WINDOWS_BUILD
LOCAL_CFLAGS += -fPIC -fpic
endif
endif

SOEXT_TLS=so.10
SOEXT_X509=so.0
SOEXT_CRYPTO=so.0

DLEXT=so
# OSX shared library extension:
# DLEXT=dylib

# Windows shared library extension:
ifdef WINDOWS_BUILD
DLEXT=dll
endif

OBJS_CRYPTO=	aes.o		aesni.o		arc4.o		\
		asn1parse.o	asn1write.o	base64.o	\
		bignum.o	blowfish.o	camellia.o	\
		ccm.o		cipher.o	cipher_wrap.o	\
		cmac.o		ctr_drbg.o	des.o		\
		dhm.o		ecdh.o		ecdsa.o		\
		ecjpake.o	ecp.o				\
		ecp_curves.o	entropy.o	entropy_poll.o	\
		error.o		gcm.o		havege.o	\
		hmac_drbg.o	md.o		md2.o		\
		md4.o		md5.o		md_wrap.o	\
		memory_buffer_alloc.o		oid.o		\
		padlock.o	pem.o		pk.o		\
		pk_wrap.o	pkcs12.o	pkcs5.o		\
		pkparse.o	pkwrite.o	platform.o	\
		ripemd160.o	rsa.o		sha1.o		\
		sha256.o	sha512.o	threading.o	\
		timing.o	version.o			\
		version_features.o		xtea.o

OBJS_X509=	certs.o		pkcs11.o	x509.o		\
		x509_create.o	x509_crl.o	x509_crt.o	\
		x509_csr.o	x509write_crt.o	x509write_csr.o

OBJS_TLS=	debug.o		net_sockets.o		\
		ssl_cache.o	ssl_ciphersuites.o	\
		ssl_cli.o	ssl_cookie.o		\
		ssl_srv.o	ssl_ticket.o		\
		ssl_tls.o

.SILENT:

.PHONY: all static clean

all: static

static: libmbedcrypto.a libmbedx509.a libmbedtls.a


# tls
libmbedtls.a: $(OBJS_TLS)
	echo "  AR    $@"
	$(TARGET_AR) -rc $@ $(OBJS_TLS)
	echo "  RL    $@"
	$(TARGET_AR) -s $@


# x509
libmbedx509.a: $(OBJS_X509)
	echo "  AR    $@"
	$(TARGET_AR) -rc $@ $(OBJS_X509)
	echo "  RL    $@"
	$(TARGET_AR) -s $@


# crypto
libmbedcrypto.a: $(OBJS_CRYPTO)
	echo "  AR    $@"
	$(TARGET_AR) -rc $@ $(OBJS_CRYPTO)
	echo "  RL    $@"
	$(TARGET_AR) -s $@
.c.o:
	echo "  CC    $<"
	$(TARGET_CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $<

clean:
ifndef WINDOWS
	rm -f *.o libmbed*
else
	del /Q /F *.o libmbed*
endif
