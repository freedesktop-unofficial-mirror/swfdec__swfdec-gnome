EXTRA_DIST =			\
	swfdec-player.desktop.in\
	swfdec.xml.in

desktop_in_files = swfdec-player.desktop.in
desktopdir = $(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

mime_in_files = swfdec.xml.in
mimedir = $(datadir)/mime/packages
mime_DATA = $(mime_in_files:.xml.in=.xml)
@INTLTOOL_XML_RULE@

CLEANFILES = 			\
	swfdec-player.desktop	\
	swfdec.xml

