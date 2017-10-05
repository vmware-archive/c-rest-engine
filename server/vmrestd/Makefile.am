sbin_PROGRAMS = vmrestd

vmrestd_SOURCES = \
    main.c \
    ssl-context.c

vmrestd_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

vmrestd_LDADD = \
    $(top_builddir)/common/libcommon.la \
    $(top_builddir)/server/restengine/librestengine.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

vmrestd_LDFLAGS = \
    -rdynamic \
    @OPENSSL_LDFLAGS@
