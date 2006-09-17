
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = $(GLIB_CFLAGS)

INTHEADERS = \
 sice_event_fifo.h \
 sice_timer.h \
 sice_common.h \
 sice_main_state_machine.h \
 sice_tr_addr_pair_handler.h \
 sice_stun.h \
 sha1.h

siceincdir=$(includedir)/libsice


lib_LTLIBRARIES = libsice.la


libsice_la_LIBADD = $(GLIB_LIBS)

libsice_la_SOURCES = \
	sice_event_fifo.c \
	sice_timer.c \
	sice_common.c \
	sice_stun.c \
	sice_main_state_machine.c \
	sice_tr_addr_pair_handler.c \
	sice_interface.c \
	sha1.c \
	$(INTHEADERS) 
