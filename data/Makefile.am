EXTRA_DIST =			\
	swfdec-player.desktop.in\
	swfdec.xml

desktop_in_files = swfdec-player.desktop.in
desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

mimedir = $(datadir)/mime/packages
mime_DATA = swfdec.xml

CLEANFILES = 			\
	swfdec-player.desktop

