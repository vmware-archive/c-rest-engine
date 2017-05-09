#
# Copyright (c) VMware Inc.  All rights Reserved.
#

noinst_LTLIBRARIES = libvmsockposix.la

libvmsockposix_la_SOURCES = \
    libmain.c \
    global.c \
    socket.c

libvmsockposix_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    -I$(top_srcdir)/transport/include \
    -DHAVE_SYS_EPOLL_H \
    -DHAVE_FCNTL_H \
    -DHAVE_ARPA_INET_H

libvmsockposix_la_LDFLAGS = \
    -static \
    @LWBASE_LIBS@ \
    @PTHREAD_LIBS@
