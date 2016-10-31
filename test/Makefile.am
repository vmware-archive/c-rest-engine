sbin_PROGRAMS = vmresttest

vmresttest_SOURCES = \
    AllTests.c \
    CuTest.c \
    TestTransport.c \
    TestHTTPProtocolHead.c \
    testUtils.c \
    TestRestProtocolHead.c \
    TestHttpUtilsExternal.c \
    TestHttpUtilsInternal.c \
    TestHttpValidate.c

vmresttest_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

vmresttest_LDADD = \
    $(top_builddir)/common/libcommon.la \
    $(top_builddir)/server/restengine/librestengine.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

vmresttest_LDFLAGS = \
    -rdynamic \
    @OPENSSL_LDFLAGS@
