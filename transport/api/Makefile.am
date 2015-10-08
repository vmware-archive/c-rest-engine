lib_LTLIBRARIES = libvmsock.la

libvmsock_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    -I$(top_srcdir)/transport/includes \
    @OPENSSL_INCLUDES@

libvmsock_la_SOURCES = \
    libmain.c 

libvmsock_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @top_builddir@/transport/posix/libvmsockposix.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

libvmsock_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@
