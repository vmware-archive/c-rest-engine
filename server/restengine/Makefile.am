lib_LTLIBRARIES = librestengine.la

librestengine_la_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

librestengine_la_SOURCES = \
    globals.c \
    httpValidate.c \
    libmain.c \
    httpProtocolHead.c \
    httpAllocStruct.c \
    httpUtilsInternal.c \
    httpUtilsExternal.c \
    httpMain.c \
    restProtocolHead.c

librestengine_la_LIBADD = \
    @top_builddir@/common/libcommon.la \
    @top_builddir@/transport/api/libvmsock.la \
    @UUID_LIBS@ \
    @CRYPTO_LIBS@ \
    @PTHREAD_LIBS@

librestengine_la_LDFLAGS = \
    @OPENSSL_LDFLAGS@

