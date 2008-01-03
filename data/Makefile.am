man1_MANS = swfdec-player.1 swfdec-thumbnailer.1

desktop_in_files = swfdec-player.desktop.in
desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =			\
	$(desktop_in_files)

CLEANFILES = 			\
	$(desktop_DATA)

