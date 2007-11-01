bin_PROGRAMS = swfdec-thumbnailer

swfdec_thumbnailer_SOURCES = swfdec-thumbnailer.c

INCLUDES = $(GLOBAL_CFLAGS) $(THUMBNAILER_CFLAGS)
swfdec_thumbnailer_LDFLAGS = $(THUMBNAILER_LIBS)

schemas_in_files = swfdec-thumbnailer.schemas.in
schemasdir = $(GCONF_SCHEMA_FILE_DIR)
schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

install-data-local:
if GCONF_SCHEMAS_INSTALL
	if test -z "$(DESTDIR)" ; then \
	for p in $(schema_DATA) ; do \
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/thumbnailer/$$p >&1 > /dev/null; \
	done \
	fi
endif

EXTRA_DIST = \
	$(schemas_in_files)

CLEANFILES = \
	$(schemas_DATA)

