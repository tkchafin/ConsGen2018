#configuration
CONFIG +=  no_mocdepend release stl qt_no_framework
QT_ARCH = i386
QT_EDITION = OpenSource
QT_CONFIG +=  qt3support accessibility minimal-config small-config medium-config large-config full-config reduce_exports ipv6 clock-monotonic mremap getaddrinfo ipv6ifname getifaddrs inotify jpeg mng png gif tiff freetype zlib nis iconv release

#versioning
QT_VERSION = 4.3.1
QT_MAJOR_VERSION = 4
QT_MINOR_VERSION = 3
QT_PATCH_VERSION = 1

QMAKE_RPATHDIR += "/usr/local/Trolltech/Qt-4.3.1/lib"
