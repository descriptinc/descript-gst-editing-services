AC_PREREQ(2.62)

dnl initialize autoconf
dnl when going to/from release please set the nano (fourth number) right !
dnl releases only do Wall, cvs and prerelease does Werror too
AC_INIT(GStreamer Editing Services, 1.17.0.1,
    http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer,
    gstreamer-editing-services)

AG_GST_INIT

dnl initialize automake
AM_INIT_AUTOMAKE([-Wno-portability 1.11 no-dist-gzip dist-xz tar-ustar subdir-objects])

dnl define PACKAGE_VERSION_* variables
AS_VERSION

dnl check if this is a release version
AS_NANO(GST_GIT="no", GST_GIT="yes")

dnl can autoconf find the source ?
AC_CONFIG_SRCDIR([ges/ges-timeline.c])

dnl define the output header for config
AC_CONFIG_HEADERS([config.h])

dnl AM_MAINTAINER_MODE only provides the option to configure to enable it
AM_MAINTAINER_MODE([enable])

dnl sets host_* variables
AC_CANONICAL_HOST

dnl use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
 [AM_DEFAULT_VERBOSITY=1
 AC_SUBST(AM_DEFAULT_VERBOSITY)])

dnl GES versioning, this is mostly informational
GES_VERSION_MAJOR=$PACKAGE_VERSION_MAJOR
GES_VERSION_MINOR=$PACKAGE_VERSION_MINOR
GES_VERSION_MICRO=$PACKAGE_VERSION_MICRO
GES_VERSION_NANO=$PACKAGE_VERSION_NANO
AC_SUBST(GES_VERSION_MAJOR)
AC_SUBST(GES_VERSION_MINOR)
AC_SUBST(GES_VERSION_MICRO)
AC_SUBST(GES_VERSION_NANO)

dnl our libraries and install dirs use major.minor as a version
GST_API_VERSION=$GST_EDITING_SERVICES_VERSION_MAJOR.$GST_EDITING_SERVICES_VERSION_MINOR
dnl we override it here if we need to for the release candidate of new series
GST_API_VERSION=1.0
AC_SUBST(GST_API_VERSION)

AS_LIBTOOL(GST, 1700, 0, 1700)

dnl *** required versions of GStreamer stuff ***
GST_REQ=1.17.0.1
GSTPB_REQ=1.17.0.1

dnl *** autotools stuff ****

dnl allow for different autotools
AS_AUTOTOOLS_ALTERNATE

dnl Add parameters for aclocal
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4 -I common/m4")
AC_CONFIG_MACRO_DIR([m4])

dnl *** check for arguments to configure ***

AG_GST_ARG_DISABLE_FATAL_WARNINGS
AG_GST_ARG_ENABLE_EXTRA_CHECKS

AG_GST_ARG_DEBUG
AG_GST_ARG_PROFILING
AG_GST_ARG_VALGRIND
AG_GST_ARG_GCOV

AG_GST_ARG_EXAMPLES

AG_GST_ARG_WITH_PKG_CONFIG_PATH
AG_GST_ARG_WITH_PACKAGE_NAME
AG_GST_ARG_WITH_PACKAGE_ORIGIN

AG_GST_PKG_CONFIG_PATH

AG_GST_FLEX_CHECK

dnl *** checks for platform ***

dnl * hardware/architecture *

dnl common/m4/gst-arch.m4
dnl check CPU type
AG_GST_ARCH

dnl Determine endianness
AC_C_BIGENDIAN

dnl *** checks for programs ***

dnl find a compiler
AC_PROG_CC

dnl check if the compiler supports '-c' and '-o' options
AM_PROG_CC_C_O

dnl check if the compiler supports do while(0) macros
AG_GST_CHECK_DOWHILE_MACROS

AC_PATH_PROG(VALGRIND_PATH, valgrind, no)
AM_CONDITIONAL(HAVE_VALGRIND, test ! "x$VALGRIND_PATH" = "xno")

dnl check for gobject-introspection
GOBJECT_INTROSPECTION_CHECK([0.9.6])

dnl check for pygobject
AC_SUBST(PYGOBJECT_REQ, 3.0)
PKG_CHECK_MODULES(PYGOBJECT, pygobject-3.0 >= $PYGOBJECT_REQ,
  [
    HAVE_PYGOBJECT=yes
  ], HAVE_PYGOBJECT=no)

AM_CONDITIONAL(WITH_PYTHON, [test "x$HAVE_PYGOBJECT" = "xyes"])

dnl check for gst-validate
PKG_CHECK_MODULES(GST_VALIDATE, gst-validate-1.0 >= 1.12.1,
  [
    HAVE_GST_VALIDATE=yes
    AC_DEFINE(HAVE_GST_VALIDATE, 1, [Define if build with gst-validate support])
  ], HAVE_GST_VALIDATE=no)

AC_SUBST(GST_VALIDATE_CFLAGS)
AC_SUBST(GST_VALIDATE_LIBS)
AM_CONDITIONAL(HAVE_GST_VALIDATE, [test "x$HAVE_GST_VALIDATE" = "xyes"])

dnl needed for scenarios definition files
GST_PREFIX="`$PKG_CONFIG --variable=prefix gstreamer-$GST_API_VERSION`"
AC_SUBST(GST_PREFIX)
GST_DATADIR="$GST_PREFIX/share"
AC_DEFINE_UNQUOTED(GST_DATADIR, "$GST_DATADIR", [system wide data directory])

dnl check for bash completion
AC_ARG_WITH([bash-completion-dir],
    AS_HELP_STRING([--with-bash-completion-dir[=PATH]],
        [Install the bash auto-completion script in this directory. @<:@default=yes@:>@]),
    [],
    [with_bash_completion_dir=yes])

if test "x$with_bash_completion_dir" = "xyes"; then
    PKG_CHECK_MODULES([BASH_COMPLETION], [bash-completion >= 2.0],
        [BASH_COMPLETION_DIR="`pkg-config --variable=completionsdir bash-completion`"],
        [BASH_COMPLETION_DIR="$datadir/bash-completion/completions"])
else
    BASH_COMPLETION_DIR="$with_bash_completion_dir"
fi

AC_SUBST([BASH_COMPLETION_DIR])
AM_CONDITIONAL([ENABLE_BASH_COMPLETION],[test "x$with_bash_completion_dir" != "xno"])

dnl *** checks for libraries ***

dnl check for libm, for sin() etc.
# LT_LIB_M
# AC_SUBST(LIBM)

dnl *** checks for header files ***

AC_CHECK_HEADERS([unistd.h], HAVE_UNISTD_H=yes)
AM_CONDITIONAL(HAVE_UNISTD_H, test "x$HAVE_UNISTD_H" = "xyes")

if test "x$HAVE_UNISTD_H" != "xyes"; then
  GST_PLUGINS_SELECTED=`echo $GST_PLUGINS_SELECTED | $SED -e s/festival//`
fi

dnl *** checks for gst-validate-launcher ***

AC_CHECK_PROG(GST_VALIDATE_LAUNCHER, gst-validate-launcher, yes)
AM_CONDITIONAL(HAVE_GST_VALIDATE_LAUNCHER, [test "x$GST_VALIDATE_LAUNCHER" = "xyes"])

dnl *** checks for types/defines ***

dnl *** checks for structures ***

dnl *** checks for compiler characteristics ***

dnl *** checks for library functions ***

dnl *** checks for headers ***

dnl *** checks for dependency libraries ***

dnl GLib is required
AG_GST_GLIB_CHECK([2.44.0])

PKG_CHECK_MODULES(GIO, gio-2.0 >= 2.16, HAVE_GIO=yes, HAVE_GIO=no)
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

dnl checks for gstreamer
dnl uninstalled is selected preferentially -- see pkg-config(1)
AG_GST_CHECK_GST($GST_API_VERSION, [$GST_REQ], yes)
AG_GST_CHECK_GST_BASE($GST_API_VERSION, [$GST_REQ], yes)
#AG_GST_CHECK_GST_GDP($GST_API_VERSION, [$GST_REQ], yes)
AG_GST_CHECK_GST_CHECK($GST_API_VERSION, [$GST_REQ], no)
AG_GST_CHECK_GST_CONTROLLER($GST_API_VERSION, [$GST_REQ], yes)
AG_GST_CHECK_GST_PLUGINS_BASE($GST_API_VERSION, [$GSTPB_REQ], yes)
AG_GST_CHECK_GST_PLUGINS_BAD($GST_API_VERSION, [$GST_REQ], yes)
AG_GST_CHECK_GST_PLUGINS_GOOD($GST_API_VERSION, [$GST_REQ], yes)
AM_CONDITIONAL(HAVE_GST_CHECK, test "x$HAVE_GST_CHECK" = "xyes")

AG_GST_ARG_WITH_PLUGINS
AG_GST_CHECK_PLUGIN(plugins)

dnl set location of plugin directory
AG_GST_SET_PLUGINDIR

GSTPB_PLUGINS_DIR=`$PKG_CONFIG gstreamer-plugins-base-$GST_API_VERSION --variable pluginsdir`
AC_SUBST(GSTPB_PLUGINS_DIR)
AC_MSG_NOTICE(Using GStreamer Base Plugins in $GSTPB_PLUGINS_DIR)

dnl check for gstreamer-pbutils
PKG_CHECK_MODULES(GST_PBUTILS, gstreamer-pbutils-$GST_API_VERSION, HAVE_GST_PBUTILS="yes", HAVE_GST_PBUTILS="no")
if test "x$HAVE_GST_PBUTILS" != "xyes"; then
  AC_ERROR([gst-pbutils is required for rendering support])
fi
AC_SUBST(GST_PBUTILS_LIBS)
AC_SUBST(GST_PBUTILS_CFLAGS)

dnl check for gst-controller
PKG_CHECK_MODULES(GST_CONTROLLER, gstreamer-controller-$GST_API_VERSION, HAVE_GST_CONTROLLER="yes", HAVE_GST_CONROLLER="no")
if test "x$HAVE_GST_CONTROLLER" != "xyes"; then
  AC_ERROR([gst-controller is required for transition support])
fi
AC_SUBST(GST_CONTROLLER_LIBS)
AC_SUBST(GST_CONTROLLER_CFLAGS)

dnl check for gstvideo
PKG_CHECK_MODULES(GST_VIDEO, gstreamer-video-$GST_API_VERSION, HAVE_GST_VIDEO="yes", HAVE_GST_CONROLLER="no")
if test "x$HAVE_GST_VIDEO" != "xyes"; then
  AC_ERROR([gst-video is required for transition support])
fi
AC_SUBST(GST_VIDEO_LIBS)
AC_SUBST(GST_VIDEO_CFLAGS)

dnl Check for documentation xrefs
GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
GST_PREFIX="`$PKG_CONFIG --variable=prefix gstreamer-$GST_API_VERSION`"
GSTPB_PREFIX="`$PKG_CONFIG --variable=prefix gstreamer-plugins-base-$GST_API_VERSION`"
AC_SUBST(GLIB_PREFIX)
AC_SUBST(GST_PREFIX)
AC_SUBST(GSTPB_PREFIX)

dnl pitivi formatter needs libxml
PKG_CHECK_MODULES(XML, libxml-2.0, HAVE_LIBXML="yes", HAVE_LIBXML="no")
if test "x$HAVE_LIBXML" != "xyes"; then
  AC_ERROR([libxml2 is required])
fi
AC_SUBST(XML_LIBS)
AC_SUBST(XML_CFLAGS)

dnl GTK is optional and only used in examples
HAVE_GTK=no
HAVE_GTK_X11=no
GTK_REQ=3.0.0
if test "x$BUILD_EXAMPLES" = "xyes"; then
  PKG_CHECK_MODULES(GTK, gtk+-3.0 >= $GTK_REQ, HAVE_GTK=yes, HAVE_GTK=no)
  dnl some examples need gtk+-x11
  PKG_CHECK_MODULES(GTK_X11, gtk+-x11-3.0 >= $GTK_REQ, HAVE_GTK_X11=yes, HAVE_GTK_X11=no)
  AC_SUBST(GTK_LIBS)
  AC_SUBST(GTK_CFLAGS)
fi
AM_CONDITIONAL(HAVE_GTK, test "x$HAVE_GTK" = "xyes")
AM_CONDITIONAL(HAVE_GTK_X11, test "x$HAVE_GTK_X11" = "xyes")

dnl Check for -Bsymbolic-functions linker flag used to avoid
dnl intra-library PLT jumps, if available.
AC_ARG_ENABLE(Bsymbolic,
              [AS_HELP_STRING([--disable-Bsymbolic],[avoid linking with -Bsymbolic])],,
              [SAVED_LDFLAGS="${LDFLAGS}" SAVED_LIBS="${LIBS}"
               AC_MSG_CHECKING([for -Bsymbolic-functions linker flag])
               LDFLAGS=-Wl,-Bsymbolic-functions
               LIBS=
               AC_TRY_LINK([], [return 0],
                           AC_MSG_RESULT(yes)
                           enable_Bsymbolic=yes,
                           AC_MSG_RESULT(no)
                           enable_Bsymbolic=no)
               LDFLAGS="${SAVED_LDFLAGS}" LIBS="${SAVED_LIBS}"])

dnl building of benchmarks
AC_ARG_ENABLE(benchmarks,
  AS_HELP_STRING([--disable-benchmarks],[disable building benchmarks apps]),
  [
    case "${enableval}" in
      yes) BUILD_BENCHMARKS=yes ;;
      no)  BUILD_BENCHMARKS=no ;;
      *)   AC_MSG_ERROR(bad value ${enableval} for --disable-benchmarks) ;;
    esac
  ],
[BUILD_BENCHMARKS=yes]) dnl Default value
AM_CONDITIONAL(BUILD_BENCHMARKS, test "x$BUILD_BENCHMARKS" = "xyes")

dnl set license and copyright notice
GST_LICENSE="LGPL"
AC_DEFINE_UNQUOTED(GST_LICENSE, "$GST_LICENSE", [GStreamer license])
AC_SUBST(GST_LICENSE)

dnl define LIBDIR so we can inform people where we live
AS_AC_EXPAND(LIBDIR, $libdir)
AC_DEFINE_UNQUOTED(LIBDIR, "$LIBDIR", [library dir])

dnl set location of plugin directory
AG_GST_SET_PLUGINDIR

dnl define an ERROR_CFLAGS Makefile variable
AG_GST_SET_ERROR_CFLAGS($GST_GIT, [-Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wundef \
				   -Wwrite-strings -Wformat-security -Wold-style-definition \
				   -Winit-self -Wmissing-include-dirs -Waddress -Waggregate-return -Wno-multichar \
				   -Wnested-externs])

dnl define correct level for debugging messages
AG_GST_SET_LEVEL_DEFAULT($GST_GIT)

dnl *** finalize CFLAGS, LDFLAGS, LIBS

dnl Overview:
dnl GST_OPTION_CFLAGS:  common flags for profiling, debugging, errors, ...
dnl GST_*:              flags shared by built objects to link against GStreamer
dnl GST_ALL_LDFLAGS:    linker flags shared by all
dnl GST_LIB_LDFLAGS:    additional linker flags for all libaries
dnl GST_LT_LDFLAGS:     library versioning of our libraries
dnl GST_PLUGIN_LDFLAGS: flags to be used for all plugins

dnl GST_OPTION_CFLAGS
if test "x$USE_DEBUG" = xyes; then
   PROFILE_CFLAGS="-g"
fi
AC_SUBST(PROFILE_CFLAGS)

if test "x$PACKAGE_VERSION_NANO" = "x1"; then
  dnl Define _only_ during CVS (not pre-releases or releases)
  DEPRECATED_CFLAGS="-DGST_DISABLE_DEPRECATED"
else
  DEPRECATED_CFLAGS=""
fi
AC_SUBST(DEPRECATED_CFLAGS)

VISIBILITY_CFLAGS=""
AS_COMPILER_FLAG([-fvisibility=hidden], [
  VISIBILITY_CFLAGS="-fvisibility=hidden"
  AC_DEFINE(GST_API_EXPORT, [extern __attribute__ ((visibility ("default")))], [public symbol export define])
], [
  VISIBILITY_CFLAGS=""
  AC_DEFINE(GST_API_EXPORT, [extern], [public symbol export define])
])
AC_SUBST(VISIBILITY_CFLAGS)

dnl disable strict aliasing
AS_COMPILER_FLAG([-fno-strict-aliasing], [EXTRA_CFLAGS="-fno-strict-aliasing"])
AC_SUBST(EXTRA_CFLAGS)

dnl every flag in GST_OPTION_CFLAGS can be overridden
dnl at make time with e.g. make ERROR_CFLAGS=""
GST_OPTION_CFLAGS="\$(WARNING_CFLAGS) \$(DEBUG_CFLAGS) \$(PROFILE_CFLAGS) \$(GCOV_CFLAGS) \$(OPT_CFLAGS) \$(DEPRECATED_CFLAGS)"
AC_SUBST(GST_OPTION_CFLAGS)

dnl FIXME: do we want to rename to GST_ALL_* ?
dnl prefer internal headers to already installed ones
dnl also add builddir include for enumtypes and marshal
dnl add GST_OPTION_CFLAGS, but overridable
GST_CFLAGS="$GST_CFLAGS $EXTRA_CFLAGS \$(GST_OPTION_CFLAGS) \$(ERROR_CFLAGS) \$(VISIBILITY_CFLAGS) -DGST_USE_UNSTABLE_API"
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)

dnl LDFLAGS really should only contain flags, not libs - they get added before
dnl whatevertarget_LIBS and -L flags here affect the rest of the linking
GST_ALL_LDFLAGS="-no-undefined"
if test "x${enable_Bsymbolic}" = "xyes"; then
  GST_ALL_LDFLAGS="$GST_ALL_LDFLAGS -Wl,-Bsymbolic-functions"
fi
AC_SUBST(GST_ALL_LDFLAGS)

dnl GST_LIB_LDFLAGS
dnl linker flags shared by all libraries
dnl LDFLAGS modifier defining exported symbols from built libraries
GST_LIB_LDFLAGS=""
AC_SUBST(GST_LIB_LDFLAGS)

dnl GST_PLUGIN_LDFLAGS must only contain flags, not libs - they get added before
dnl whatevertarget_LIBS and -L flags here affect the rest of the linking
GST_PLUGIN_LDFLAGS="-module -avoid-version $GST_ALL_LDFLAGS"
AC_SUBST(GST_PLUGIN_LDFLAGS)

dnl *** output files ***

dnl po/Makefile.in

AC_CONFIG_FILES(
Makefile
ges/ges-version.h
common/Makefile
common/m4/Makefile
m4/Makefile
ges/Makefile
tests/Makefile
tests/check/Makefile
tests/benchmarks/Makefile
examples/Makefile
examples/c/Makefile
tests/validate/Makefile
tests/validate/scenarios/Makefile
tools/Makefile
pkgconfig/Makefile
pkgconfig/gst-editing-services.pc
pkgconfig/gst-editing-services-uninstalled.pc
plugins/Makefile
plugins/ges/Makefile
plugins/nle/Makefile
bindings/Makefile
bindings/python/Makefile
bindings/python/gi/Makefile
bindings/python/gi/overrides/Makefile
)
AC_OUTPUT
