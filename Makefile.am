SUBDIRS=comm utils lib

bin_PROGRAMS=netstream

AM_CFLAGS=-O2 -ggdb3 -std=c99 -I$(LIB_XML_INCLUDE) -lpthread 
AM_LDFLAGS=


netstream_SOURCES=netstream.c netstream.h
netstream_LDADD=./comm/libcommlite.a\
				./comm/libudplite.a\
		 ./lib/libmytools.a\
		 ./utils/libutils.a
#netstream_SOURCES=pma_bm.c
#netstream_LDADD=./comm/libpmacomm.a\
EXTRA_DIST=cfgtool/*\
		cfg.ini
