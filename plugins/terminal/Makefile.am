if ENABLE_TERMINAL_PLUGIN

DISTCLEANFILES =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST = $(plugin_DATA)

plugindir = $(libdir)/gnome-builder/plugins
plugin_LTLIBRARIES = libterminal.la
dist_plugin_DATA = terminal.plugin

libterminal_la_SOURCES = \
	gb-terminal-application-addin.c \
	gb-terminal-application-addin.h \
	gb-terminal-document.c \
	gb-terminal-document.h \
	gb-terminal-plugin.c \
	gb-terminal-private.h \
	gb-terminal-view.c \
	gb-terminal-view.h \
	gb-terminal-view-private.h \
	gb-terminal-view-actions.c \
	gb-terminal-view-actions.h \
	gb-terminal-workbench-addin.c \
	gb-terminal-workbench-addin.h \
	$(NULL)

nodist_libterminal_la_SOURCES = \
	gb-terminal-resources.c \
	gb-terminal-resources.h \
	$(NULL)

libterminal_la_CFLAGS = \
	$(BUILDER_CFLAGS) \
	$(TERMINAL_CFLAGS) \
	-I$(top_srcdir)/libide \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/app \
	-I$(top_srcdir)/src/documents \
	-I$(top_srcdir)/src/util \
	-I$(top_srcdir)/src/views \
	-I$(top_srcdir)/src/workbench \
	-I$(top_srcdir)/src/workspace \
	-I$(top_srcdir)/contrib/egg \
	$(NULL)

libterminal_la_LIBADD = \
	$(BUILDER_LIBS) \
	$(TERMINAL_LIBS) \
	$(NULL)

libterminal_la_LDFLAGS = \
	-avoid-version \
	-module \
	$(NULL)

glib_resources_c = gb-terminal-resources.c
glib_resources_h = gb-terminal-resources.h
glib_resources_xml = gb-terminal.gresource.xml
glib_resources_namespace = gb_terminal
include $(top_srcdir)/build/autotools/Makefile.am.gresources

include $(top_srcdir)/plugins/Makefile.plugin

endif

-include $(top_srcdir)/git.mk
