lib_LTLIBRARIES = librestengine.la

librestengine_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

librestengine_la_SOURCES = \
    librestengine.c

librestengine_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

librestengine_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@


