bin_PROGRAMS = rest-cli

rest_cli_SOURCES = \
    main.c

rest_cli_CPPFLAGS = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/include/public \
    @OPENSSL_INCLUDES@

rest_cli_LDADD = \
    $(top_builddir)/common/libcommon.la \
    $(top_builddir)/client/libvmrestclient.la
