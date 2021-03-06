/* gb-devhelp-panel.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib/gi18n.h>
#include <devhelp/devhelp.h>
#include <libpeas/peas.h>

#include "gb-devhelp-document.h"
#include "gb-devhelp-panel.h"
#include "gb-devhelp-resources.h"
#include "gb-devhelp-view.h"
#include "gb-document.h"
#include "gb-editor-view.h"
#include "gb-view.h"
#include "gb-view-grid.h"
#include "gb-widget.h"
#include "gb-workbench-addin.h"
#include "gb-workbench.h"
#include "gb-workspace.h"

struct _GbDevhelpPanel
{
  GtkBin             parent_instance;

  DhBookManager     *book_manager;
  GbDevhelpDocument *document;

  GbView            *current_view;
  gulong             current_view_handler;

  GtkWidget         *sidebar;
};

static void workbench_addin_iface_init (GbWorkbenchAddinInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (GbDevhelpPanel, gb_devhelp_panel, GTK_TYPE_BIN, 0,
                                G_IMPLEMENT_INTERFACE_DYNAMIC (GB_TYPE_WORKBENCH_ADDIN,
                                                               workbench_addin_iface_init))

static void
focus_devhelp_search_cb (GSimpleAction  *action,
                         GVariant       *variant,
                         GbDevhelpPanel *self)
{
  GbWorkbench *workbench;
  GtkWidget *workspace;
  GtkWidget *pane;

  g_assert (G_IS_SIMPLE_ACTION (action));
  g_assert (GB_IS_DEVHELP_PANEL (self));

  workbench = gb_widget_get_workbench (GTK_WIDGET (self));
  workspace = gb_workbench_get_workspace (workbench);
  pane = gb_workspace_get_right_pane (GB_WORKSPACE (workspace));

  gtk_container_child_set (GTK_CONTAINER (workspace), pane,
                           "reveal", TRUE,
                           NULL);

  dh_sidebar_set_search_focus (DH_SIDEBAR (self->sidebar));
}

static void
request_documentation_cb (GbDevhelpPanel *self,
                          const gchar    *keywords,
                          GbEditorView   *view)
{
  g_assert (GB_IS_EDITOR_VIEW (view));
  g_assert (GB_IS_DEVHELP_PANEL (self));

  if (ide_str_empty0 (keywords))
    return;

  dh_sidebar_set_search_string (DH_SIDEBAR (self->sidebar), keywords);
  dh_sidebar_set_search_focus (DH_SIDEBAR (self->sidebar));
}

static void
notify_active_view_cb (GbDevhelpPanel *self,
                       GParamSpec     *pspec,
                       GbWorkbench    *workbench)
{
  GtkWidget *view;

  g_assert (GB_IS_DEVHELP_PANEL (self));
  g_assert (GB_IS_WORKBENCH (workbench));

  view = gb_workbench_get_active_view (workbench);

  /* If the active view is NULL, the current view is already destroyed but
   * the weak pointer has not been call yet so self->current_view is not NULL
   */
  if (view != NULL && self->current_view)
    {
      g_signal_handler_disconnect (self->current_view, self->current_view_handler);
      self->current_view_handler = 0;
      ide_clear_weak_pointer (&self->current_view);
    }

  if (!GB_IS_EDITOR_VIEW (view))
    return;

  ide_set_weak_pointer (&self->current_view, (GbView *)view);
  self->current_view_handler = g_signal_connect_object (view,
                                                        "request-documentation",
                                                        G_CALLBACK (request_documentation_cb),
                                                        self,
                                                        G_CONNECT_SWAPPED);
}

static void
gb_devhelp_panel_load (GbWorkbenchAddin *addin,
                       GbWorkbench      *workbench)
{
  GbDevhelpPanel *self = (GbDevhelpPanel *)addin;
  GtkWidget *workspace;
  GtkWidget *pane;
  const gchar * const accels[] = { "<ctrl><shift>f", NULL };
  GSimpleAction *action;
  GApplication *app;
  GtkWidget *parent;

  g_assert (GB_IS_DEVHELP_PANEL (self));
  g_assert (GB_IS_WORKBENCH (workbench));

  workspace = gb_workbench_get_workspace (workbench);
  pane = gb_workspace_get_right_pane (GB_WORKSPACE (workspace));
  gb_workspace_pane_add_page (GB_WORKSPACE_PANE (pane), GTK_WIDGET (self),
                              _("Documentation"), "help-contents-symbolic");

  parent = gtk_widget_get_parent (GTK_WIDGET (self));
  gtk_container_child_set (GTK_CONTAINER (parent), GTK_WIDGET (self),
                           "position", 0,
                           NULL);

  action = g_simple_action_new ("focus-devhelp-search", NULL);
  g_signal_connect_object (action,
                           "activate",
                           G_CALLBACK (focus_devhelp_search_cb),
                           self,
                           0);
  g_action_map_add_action (G_ACTION_MAP (workbench), G_ACTION (action));
  g_clear_object (&action);

  app = g_application_get_default ();
  gtk_application_set_accels_for_action (GTK_APPLICATION (app),
                                         "win.focus-devhelp-search",
                                         accels);

  gtk_widget_show (GTK_WIDGET (self));

  gtk_stack_set_visible_child (GTK_STACK (parent), GTK_WIDGET (self));

  g_signal_connect_object (workbench,
                           "notify::active-view",
                           G_CALLBACK (notify_active_view_cb),
                           self,
                           G_CONNECT_SWAPPED);
}

static void
gb_devhelp_panel_unload (GbWorkbenchAddin *addin,
                         GbWorkbench      *workbench)
{
  /*
   * TODO: We shouldn't use ourself as the workbench addin because it is
   *       too difficult to reliably control lifecycle. Instead use another
   *       addin object and make this code only deal with the panel.
   */
}

void
gb_devhelp_panel_set_uri (GbDevhelpPanel *self,
                          const gchar    *uri)
{
  GbWorkbench *workbench;
  GbViewGrid *view_grid;

  g_return_if_fail (GB_IS_DEVHELP_PANEL (self));

  workbench = gb_widget_get_workbench (GTK_WIDGET (self));
  view_grid = GB_VIEW_GRID (gb_workbench_get_view_grid (workbench));

  dh_sidebar_select_uri (DH_SIDEBAR (self->sidebar), uri);
  gb_devhelp_document_set_uri (GB_DEVHELP_DOCUMENT (self->document), uri);
  gb_view_grid_focus_document (view_grid, GB_DOCUMENT (self->document));
}

static void
link_selected_cb (GbDevhelpPanel *self,
                  DhLink         *link,
                  DhSidebar      *sidebar)
{
  GbWorkbench *workbench;
  GbViewGrid *view_grid;
  gchar *uri;

  g_assert (GB_IS_DEVHELP_PANEL (self));
  g_assert (link != NULL);
  g_assert (DH_IS_SIDEBAR (sidebar));

  workbench = gb_widget_get_workbench (GTK_WIDGET (self));
  view_grid = GB_VIEW_GRID (gb_workbench_get_view_grid (workbench));

  uri = dh_link_get_uri (link);
  gb_devhelp_document_set_uri (GB_DEVHELP_DOCUMENT (self->document), uri);
  g_free (uri);

  gb_view_grid_raise_document (view_grid, GB_DOCUMENT (self->document), FALSE);
}

static void
fixup_box_border_width (GtkWidget *widget,
                        gpointer   user_data)
{
  if (GTK_IS_BOX (widget))
    gtk_container_set_border_width (GTK_CONTAINER (widget), 0);
}

static void
gb_devhelp_panel_grab_focus (GtkWidget *widget)
{
  GbDevhelpPanel *self = (GbDevhelpPanel *)widget;

  dh_sidebar_set_search_focus (DH_SIDEBAR (self->sidebar));
}

static void
gb_devhelp_panel_finalize (GObject *object)
{
  GbDevhelpPanel *self = (GbDevhelpPanel *)object;

  g_clear_object (&self->book_manager);
  g_clear_object (&self->document);
  ide_clear_weak_pointer (&self->current_view);

  G_OBJECT_CLASS (gb_devhelp_panel_parent_class)->finalize (object);
}

static void
gb_devhelp_panel_class_init (GbDevhelpPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gb_devhelp_panel_finalize;

  widget_class->grab_focus = gb_devhelp_panel_grab_focus;
}

static void
gb_devhelp_panel_class_finalize (GbDevhelpPanelClass *klass)
{
}

static void
gb_devhelp_panel_init (GbDevhelpPanel *self)
{
  self->book_manager = dh_book_manager_new ();
  dh_book_manager_populate (self->book_manager);

  self->document = g_object_new (GB_TYPE_DEVHELP_DOCUMENT, NULL);

  self->sidebar = dh_sidebar_new (self->book_manager);
  gtk_container_foreach (GTK_CONTAINER (self->sidebar), fixup_box_border_width, NULL);
  gtk_container_add (GTK_CONTAINER (self), self->sidebar);
  gtk_widget_show (self->sidebar);

  g_signal_connect_object (self->sidebar,
                           "link-selected",
                           G_CALLBACK (link_selected_cb),
                           self,
                           G_CONNECT_SWAPPED);
}

static void
workbench_addin_iface_init (GbWorkbenchAddinInterface *iface)
{
  iface->load = gb_devhelp_panel_load;
  iface->unload = gb_devhelp_panel_unload;
}

void
_gb_devhelp_panel_register_type (GTypeModule *module)
{
  gb_devhelp_panel_register_type (module);
}
