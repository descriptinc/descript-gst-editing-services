built_header_make =
built_source_make =

lib_LTLIBRARIES = libges-@GST_API_VERSION@.la


EXTRA_libges_@GST_API_VERSION@_la_SOURCES = gesmarshal.list

EXTRA_DIST=parse.l

CLEANFILES = $(BUILT_SOURCES) $(built_header_make) $(built_source_make) *.gcno *.gcda *.gcov *.gcov.out

nodist_libges_@GST_API_VERSION@_la_SOURCES = lex.priv_ges_parse_yy.c ges-parse-lex.h


libges_@GST_API_VERSION@_la_SOURCES = 		\
	$(built_source_make)			\
	ges.c					\
	ges-enums.c				\
	ges-meta-container.c        \
	ges-timeline.c				\
	ges-layer.c			\
	ges-clip.c			\
	ges-pipeline.c			\
	ges-source-clip.c			\
	ges-base-effect-clip.c		\
	ges-effect-clip.c		\
	ges-uri-clip.c		\
	ges-operation-clip.c		\
	ges-base-transition-clip.c		\
	ges-transition-clip.c	\
	ges-test-clip.c		\
	ges-title-clip.c		\
	ges-overlay-clip.c			\
	ges-text-overlay-clip.c		\
	ges-track.c				\
	ges-audio-track.c \
	ges-video-track.c \
	ges-track-element.c			\
	ges-source.c			\
	ges-operation.c			\
	ges-video-source.c \
	ges-audio-source.c \
	ges-video-uri-source.c			\
	ges-audio-uri-source.c	\
	ges-image-source.c		\
	ges-multi-file-source.c		\
	ges-transition.c			\
	ges-audio-transition.c		\
	ges-video-transition.c		\
	ges-video-test-source.c		\
	ges-audio-test-source.c		\
	ges-title-source.c		\
	ges-text-overlay.c		\
	ges-base-effect.c		\
	ges-effect.c		\
	ges-screenshot.c			\
	ges-formatter.c				\
	ges-pitivi-formatter.c			\
	ges-asset.c \
	ges-uri-asset.c \
	ges-clip-asset.c \
	ges-track-element-asset.c \
	ges-extractable.c \
	ges-project.c \
	ges-base-xml-formatter.c \
	ges-xml-formatter.c \
	ges-command-line-formatter.c \
	ges-auto-transition.c \
	ges-timeline-element.c \
	ges-timeline-tree.c \
	ges-container.c \
	ges-effect-asset.c \
	ges-smart-adder.c \
	ges-smart-video-mixer.c \
	ges-utils.c \
	ges-group.c \
	ges-validate.c \
	ges-structured-interface.c \
	ges-structure-parser.c \
	gstframepositioner.c

libges_@GST_API_VERSION@includedir = $(includedir)/gstreamer-@GST_API_VERSION@/ges/
libges_@GST_API_VERSION@include_HEADERS = 	\
	$(built_header_make)			\
	ges-types.h				\
	ges.h					\
	ges-prelude.h				\
	ges-enums.h				\
	ges-gerror.h				\
	ges-meta-container.h        \
	ges-timeline.h				\
	ges-layer.h			\
	ges-clip.h			\
	ges-pipeline.h			\
	ges-source-clip.h			\
	ges-uri-clip.h		\
	ges-base-effect-clip.h		\
	ges-effect-clip.h		\
	ges-operation-clip.h		\
	ges-base-transition-clip.h		\
	ges-transition-clip.h	\
	ges-test-clip.h		\
	ges-title-clip.h		\
	ges-overlay-clip.h			\
	ges-text-overlay-clip.h		\
	ges-base-effect.h		\
	ges-effect.h		\
	ges-track.h				\
	ges-audio-track.h \
	ges-video-track.h \
	ges-track-element.h			\
	ges-source.h			\
	ges-operation.h			\
	ges-video-source.h \
	ges-audio-source.h \
	ges-video-uri-source.h			\
	ges-audio-uri-source.h			\
	ges-image-source.h		\
	ges-multi-file-source.h		\
	ges-transition.h			\
	ges-audio-transition.h		\
	ges-video-transition.h		\
	ges-video-test-source.h		\
	ges-audio-test-source.h		\
	ges-title-source.h		\
	ges-text-overlay.h		\
	ges-screenshot.h			\
	ges-formatter.h				\
	ges-pitivi-formatter.h			\
	ges-asset.h \
	ges-uri-asset.h \
	ges-clip-asset.h \
	ges-track-element-asset.h \
	ges-extractable.h \
	ges-project.h \
	ges-base-xml-formatter.h \
	ges-xml-formatter.h \
	ges-command-line-formatter.h \
	ges-timeline-element.h \
	ges-container.h \
	ges-effect-asset.h \
	ges-utils.h \
	ges-group.h \
	ges-version.h

noinst_HEADERS = \
	ges-internal.h \
	ges-auto-transition.h \
	ges-structured-interface.h \
	ges-structure-parser.h \
	ges-smart-video-mixer.h \
	ges-smart-adder.h \
	ges-timeline-tree.h \
	gstframepositioner.h

libges_@GST_API_VERSION@_la_CFLAGS = -I$(top_srcdir) $(GST_PBUTILS_CFLAGS) \
		$(GST_VIDEO_CFLAGS) $(GST_CONTROLLER_CFLAGS) $(GST_PLUGINS_BASE_CFLAGS) \
		$(GST_CFLAGS) $(XML_CFLAGS) $(GIO_CFLAGS) $(GST_VALIDATE_CFLAGS) \
		-DG_LOG_DOMAIN=\"GES\" -DBUILDING_GES
libges_@GST_API_VERSION@_la_LIBADD = $(GST_PBUTILS_LIBS) \
		$(GST_VIDEO_LIBS) $(GST_CONTROLLER_LIBS) $(GST_PLUGINS_BASE_LIBS) \
		$(GST_BASE_LIBS) $(GST_LIBS) $(XML_LIBS) $(GIO_LIBS) $(GST_VALIDATE_LIBS)
libges_@GST_API_VERSION@_la_LDFLAGS = $(GST_LIB_LDFLAGS) $(GST_ALL_LDFLAGS) \
		$(GST_LT_LDFLAGS) $(GIO_CFLAGS) $(GST_VALIDATE_CFLAGS)

DISTCLEANFILE = $(CLEANFILES)

#files built on make all/check/instal
BUILT_SOURCES =			\
	$(built_header_make)	\
	$(built_source_make)    \
	lex.priv_ges_parse_yy.c \
	ges-parse-lex.h

include $(top_srcdir)/common/gst-glib-gen.mak

if HAVE_INTROSPECTION
BUILT_GIRSOURCES = GES-@GST_API_VERSION@.gir

gir_headers_temp=$(patsubst %,$(srcdir)/%, $(libges_@GST_API_VERSION@include_HEADERS))
gir_headers=$(subst $(srcdir)/ges-version.h,$(builddir)/ges-version.h,$(gir_headers_temp))
gir_sources=$(patsubst %,$(srcdir)/%, $(libges_@GST_API_VERSION@_la_SOURCES))

GES-@GST_API_VERSION@.gir: $(INTROSPECTION_SCANNER) libges-@GST_API_VERSION@.la
	$(AM_V_GEN)PKG_CONFIG_PATH="$(GST_PKG_CONFIG_PATH)" \
		CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" CC="$(CC)" PKG_CONFIG="$(PKG_CONFIG)" DLLTOOL="$(DLLTOOL)" \
		$(INTROSPECTION_SCANNER) -v --namespace GES \
		--nsversion=@GST_API_VERSION@ \
		--identifier-prefix=GES \
		--symbol-prefix=ges \
		--warn-all \
		--c-include='ges/ges.h' \
		-I$(top_srcdir) \
		-I$(top_builddir) \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-pbutils-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-audio-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-video-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-tag-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gstreamer-base-@GST_API_VERSION@` \
		--add-include-path=`$(PKG_CONFIG) --variable=girdir gio-2.0` \
		--library=libges-@GST_API_VERSION@.la \
		--include=Gst-@GST_API_VERSION@ \
		--include=GstVideo-@GST_API_VERSION@ \
		--include=GstPbutils-@GST_API_VERSION@ \
		--include=Gio-2.0 \
		--libtool="$(top_builddir)/libtool" \
		--pkg gstreamer-@GST_API_VERSION@ \
		--pkg gstreamer-pbutils-@GST_API_VERSION@ \
		--pkg gstreamer-controller-@GST_API_VERSION@ \
		--pkg gio-2.0 \
		--pkg-export gst-editing-services-@GST_API_VERSION@ \
		--add-init-section="$(INTROSPECTION_INIT)" \
		--add-init-section="extern gboolean ges_init(void); ges_init();" \
		--output $@ \
		$(gir_headers) \
		$(gir_sources)

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/

typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(AM_V_GEN)PKG_CONFIG_PATH="$(GST_PKG_CONFIG_PATH)" \
		$(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=$(srcdir)/../video \
		--includedir=$(builddir) \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-pbutils-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-audio-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-video-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-tag-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-base-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gstreamer-controller-@GST_API_VERSION@` \
		--includedir=`$(PKG_CONFIG) --variable=girdir gio-2.0` \
		$(INTROSPECTION_COMPILER_OPTS) $< -o $(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif

%.c.gcov: .libs/libges_@GST_API_VERSION@_la-%.gcda %.c
	$(GCOV) -b -f -o $^ > $@.out

gcov: $(libges_@GST_API_VERSION@_la_SOURCES:=.gcov)

lex.priv_ges_parse_yy.c ges-parse-lex.h: parse.l
	$(AM_V_GEN)$(FLEX_PATH) --header-file=ges-parse-lex.h -Ppriv_ges_parse_yy $^
