if ENABLE_STATIC_DRIVERS
noinst_LTLIBRARIES = libsrv.sys.la
DRIVER_CFLAGS =
DRIVER_LDFLAGS =
else
lib_LTLIBRARIES = libsrv.sys.la
DRIVER_CFLAGS = -shared
DRIVER_LDFLAGS = -module -shared -avoid-version
endif

libsrv_sys_la_SOURCES = \
    ccb.c          \
    config.c       \
    device.c       \
    deviceclose.c  \
    devicecreate.c \
    deviceio.c     \
    driver.c       \
    files.c        \
    connections.c  \
    globals.c      \
    monitor.c      \
    sessions.c     \
    srvshares.c    \
    srvstats.c

libsrv_sys_la_CPPFLAGS = \
    -DLW_ENABLE_THREADS=1  \
    -I@top_srcdir@         \
    -I@top_srcdir@/include \
    -I@top_srcdir@/server/include \
    -I@top_srcdir@/server/srv/include

libsrv_sys_la_CFLAGS = $(AM_CFLAGS) $(DRIVER_CFLAGS)

libsrv_sys_la_LIBADD = \
    @top_builddir@/server/srv/protocols/api/libsrv.protocols.api.la \
    @top_builddir@/server/srv/transport/threadpool/libsrv.transport.threadpool.la \
    @top_builddir@/server/srv/elements/libsrvelements.la \
    @top_builddir@/server/srv/shares/api/libshareapi.la \
    @top_builddir@/server/srv/statistics/libstatisticsapi.la \
    @top_builddir@/server/srv/oem/libsrvoem.la \
    @top_builddir@/server/srv/utils/libsrvutils.la \
    @top_builddir@/server/iomgr/libiomgr.la \
    @top_builddir@/server/smbcommon/liblwiosmbcommon.la \
    @top_builddir@/libshareinfo/liblwioshareinfo.la \
    @KRB5_LIBS@ \
    @UUID_LIBS@ \
    @PTHREAD_LIBS@

libsrv_sys_la_LDFLAGS = $(DRIVER_LDFLAGS)

