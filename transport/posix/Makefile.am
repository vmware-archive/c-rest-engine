lib_LTLIBRARIES = libvmsockposix.la

libvmsockposix_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    -I$(top_srcdir)/transport/includes \
    @OPENSSL_INCLUDES@

libvmsockposix_la_SOURCES = \
    globals.c \
    libtransport.c \
    socket.c

libvmsockposix_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

libvmsockposix_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@
