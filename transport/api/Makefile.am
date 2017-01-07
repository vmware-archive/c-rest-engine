#
# Copyright (c) VMware Inc.  All rights Reserved.
#

noinst_LTLIBRARIES = libvmsock.la

libvmsock_la_SOURCES = \
    api.c \
    globals.c \
    libmain.c

libvmsock_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    -I$(top_srcdir)/transport/include \
    -DHAVE_ARPA_INET_H

libvmsock_la_LDFLAGS = \
    -static

libvmsock_la_LIBADD = \
    $(top_builddir)/transport/posix/libvmsockposix.la
