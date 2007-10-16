bin_PROGRAMS = swfplay

swfplay_SOURCES = \
	swfdec_slow_loader.c \
	swfplay.c

INCLUDES = $(GLOBAL_CFLAGS) $(GTK_CFLAGS) $(SWFDEC_GTK_CFLAGS)
swfplay_LDFLAGS = $(SWFDEC_GTK_LIBS) $(GTK_LIBS)
