lib_LTLIBRARIES = libvmrestclient.la

libvmrestclient_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

libvmrestclient_la_SOURCES = \
    libmain.c

libvmrestclient_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

libvmrestclient_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@

