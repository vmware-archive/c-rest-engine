lib_LTLIBRARIES = libtransport.la

libtransport_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

libtransport_la_SOURCES = \
    libtransport.c

libtransport_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

libtransport_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@
