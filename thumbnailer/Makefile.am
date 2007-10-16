@INTLTOOL_SCHEMAS_RULE@

bin_PROGRAMS = swfdec-thumbnailer

swfdec_thumbnailer_SOURCES = swfdec-thumbnailer.c

INCLUDES = $(GLOBAL_CFLAGS) $(THUMBNAILER_CFLAGS)
swfdec_thumbnailer_LDFLAGS = $(THUMBNAILER_LIBS)

schema_files = \
	swfdec-thumbnailer.schemas.in

schema_DATA = \
	swfdec-thumbnailer.schemas

schemadir = $(GCONF_SCHEMA_FILE_DIR)

install-data-local:
if GCONF_SCHEMAS_INSTALL
	if test -z "$(DESTDIR)" ; then \
	for p in $(schema_DATA) ; do \
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/src/$$p >&1 > /dev/null; \
	done \
	fi
endif

EXTRA_DIST = \
	$(schema_files)

DISTCLEANFILES = \
	$(schema_DATA)
