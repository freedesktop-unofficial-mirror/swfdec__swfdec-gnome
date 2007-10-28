bin_PROGRAMS = swfdec-player

swfdec_player_SOURCES = \
	swfdec-player.c \
	swfdec-window.c \
	swfdec-window-handlers.c

noinst_HEADERS = \
	swfdec-window.h

INCLUDES = $(GLOBAL_CFLAGS) $(PLAYER_CFLAGS) \
	-DDATADIR=\""$(datadir)"\"

swfdec_player_LDFLAGS = $(PLAYER_LIBS)

pkgdata_DATA = \
	swfdec-player.ui

EXTRA_DIST = \
	$(pkgdata_DATA)

