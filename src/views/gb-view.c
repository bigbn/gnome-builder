/* gb-view.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib/gi18n.h>

#include "gb-view.h"

typedef struct
{
  GtkBox *controls;
  GMenu  *menu;
} GbViewPrivate;

static void buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (GbView, gb_view, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (GbView)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, buildable_iface_init))

enum {
  PROP_0,
  PROP_CAN_SPLIT,
  PROP_DOCUMENT,
  PROP_MODIFIED,
  PROP_SPECIAL_TITLE,
  PROP_TITLE,
  LAST_PROP
};

static GParamSpec *properties [LAST_PROP];

/**
 * gb_view_get_can_preview:
 * @self: A #GbView.
 *
 * Checks if @self can create a preview view (such as html, markdown, etc).
 *
 * Returns: %TRUE if @self can create a preview view.
 */
gboolean
gb_view_get_can_preview (GbView *self)
{
  g_return_val_if_fail (GB_IS_VIEW (self), FALSE);

  if (GB_VIEW_GET_CLASS (self)->get_can_preview)
    return GB_VIEW_GET_CLASS (self)->get_can_preview (self);

  return FALSE;
}

/**
 * gb_view_get_can_split:
 * @self: A #GbView.
 *
 * Checks if @self can create a split view. If so, %TRUE is returned. Otherwise, %FALSE.
 *
 * Returns: %TRUE if @self can create a split.
 */
gboolean
gb_view_get_can_split (GbView *self)
{
  g_return_val_if_fail (GB_IS_VIEW (self), FALSE);

  if (GB_VIEW_GET_CLASS (self)->get_can_split)
    return GB_VIEW_GET_CLASS (self)->get_can_split (self);

  return FALSE;
}

/**
 * gb_view_create_split:
 * @self: A #GbView.
 *
 * Creates a new view similar to @self that can be displayed in a split.
 * If the view does not support splits, %NULL will be returned.
 *
 * Returns: (transfer full): A #GbView.
 */
GbView *
gb_view_create_split (GbView *self)
{
  g_return_val_if_fail (GB_IS_VIEW (self), NULL);

  if (GB_VIEW_GET_CLASS (self)->create_split)
    return GB_VIEW_GET_CLASS (self)->create_split (self);

  return NULL;
}

/**
 * gb_view_set_split_view:
 * @self: A #GbView.
 * @split_view: if the split should be enabled.
 *
 * Set a split view using GtkPaned style split with %GTK_ORIENTATION_VERTICAL.
 */
void
gb_view_set_split_view (GbView   *self,
                        gboolean  split_view)
{
  g_return_if_fail (GB_IS_VIEW (self));

  if (GB_VIEW_GET_CLASS (self)->set_split_view)
    GB_VIEW_GET_CLASS (self)->set_split_view (self, split_view);
}

/**
 * gb_view_get_controls:
 * @self: A #GbView.
 *
 * Gets the controls for the view.
 *
 * Returns: (transfer none) (nullable): A #GtkWidget.
 */
GtkWidget *
gb_view_get_controls (GbView *self)
{
  GbViewPrivate *priv = gb_view_get_instance_private (self);

  g_return_val_if_fail (GB_IS_VIEW (self), NULL);

  return GTK_WIDGET (priv->controls);
}

/**
 * gb_view_get_document:
 * @self: A #GbView.
 *
 * Gets the document for the view.
 *
 * Returns: (transfer none): A #GbDocument.
 */
GbDocument *
gb_view_get_document (GbView *self)
{
  g_return_val_if_fail (GB_IS_VIEW (self), NULL);

  if (GB_VIEW_GET_CLASS (self)->get_document)
    return GB_VIEW_GET_CLASS (self)->get_document (self);

  return NULL;
}

const gchar *
gb_view_get_title (GbView *self)
{
  GbDocument *document;

  if (GB_VIEW_GET_CLASS (self)->get_title)
    return GB_VIEW_GET_CLASS (self)->get_title (self);

  document = gb_view_get_document (self);

  return gb_document_get_title (document);
}

void
gb_view_set_back_forward_list (GbView             *self,
                               IdeBackForwardList *back_forward_list)
{
  g_return_if_fail (GB_IS_VIEW (self));
  g_return_if_fail (IDE_IS_BACK_FORWARD_LIST (back_forward_list));

  if (GB_VIEW_GET_CLASS (self)->set_back_forward_list)
    GB_VIEW_GET_CLASS (self)->set_back_forward_list (self, back_forward_list);
}

void
gb_view_navigate_to (GbView            *self,
                     IdeSourceLocation *location)
{
  g_return_if_fail (GB_IS_VIEW (self));
  g_return_if_fail (location != NULL);

  if (GB_VIEW_GET_CLASS (self)->navigate_to)
    GB_VIEW_GET_CLASS (self)->navigate_to (self, location);
}

gboolean
gb_view_get_modified (GbView *self)
{
  g_return_val_if_fail (GB_IS_VIEW (self), FALSE);

  if (GB_VIEW_GET_CLASS (self)->get_modified)
    return GB_VIEW_GET_CLASS (self)->get_modified (self);

  return FALSE;
}

static void
gb_view_notify (GObject    *object,
                GParamSpec *pspec)
{
  /*
   * XXX:
   *
   * This should get removed after 3.18 when path bar lands.
   * This also notifies of special-title after title is emitted.
   */
  if (pspec == properties [PROP_TITLE])
    g_object_notify_by_pspec (object, properties [PROP_SPECIAL_TITLE]);

  if (G_OBJECT_CLASS (gb_view_parent_class)->notify)
    G_OBJECT_CLASS (gb_view_parent_class)->notify (object, pspec);
}

static void
gb_view_destroy (GtkWidget *widget)
{
  GbView *self = (GbView *)widget;
  GbViewPrivate *priv = gb_view_get_instance_private (self);

  g_clear_object (&priv->controls);

  GTK_WIDGET_CLASS (gb_view_parent_class)->destroy (widget);
}

static void
gb_view_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GbView *self = GB_VIEW (object);

  switch (prop_id)
    {
    case PROP_CAN_SPLIT:
      g_value_set_boolean (value, gb_view_get_can_split (self));
      break;

    case PROP_DOCUMENT:
      g_value_set_object (value, gb_view_get_document (self));
      break;

    case PROP_MODIFIED:
      g_value_set_boolean (value, gb_view_get_modified (self));
      break;

    case PROP_SPECIAL_TITLE:
      g_value_set_string (value, gb_view_get_special_title (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, gb_view_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_view_class_init (GbViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = gb_view_get_property;
  object_class->notify = gb_view_notify;

  widget_class->destroy = gb_view_destroy;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/ui/gb-view.ui");
  gtk_widget_class_bind_template_child_private (widget_class, GbView, menu);

  properties [PROP_CAN_SPLIT] =
    g_param_spec_boolean ("can-split",
                          "Can Split",
                          "If the view can be split.",
                          FALSE,
                          (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DOCUMENT] =
    g_param_spec_object ("document",
                         "Document",
                         "The underlying document.",
                         GB_TYPE_DOCUMENT,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_MODIFIED] =
    g_param_spec_boolean ("modified",
                          "Modified",
                          "If the document has been modified.",
                          FALSE,
                          (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The view title.",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  /*
   * XXX:
   *
   * This property should be removed after 3.18 when path bar lands.
   */
  properties [PROP_SPECIAL_TITLE] =
    g_param_spec_string ("special-title",
                         "Special Title",
                         "The special title to be displayed in the document menu button.",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, properties);
}

static void
gb_view_init (GbView *self)
{
  GbViewPrivate *priv = gb_view_get_instance_private (self);
  GtkBox *controls;

  gtk_widget_init_template (GTK_WIDGET (self));

  controls = g_object_new (GTK_TYPE_BOX,
                           "orientation", GTK_ORIENTATION_HORIZONTAL,
                           "visible", TRUE,
                           NULL);
  priv->controls = g_object_ref_sink (controls);
}

static GObject *
gb_view_get_internal_child (GtkBuildable *buildable,
                            GtkBuilder   *builder,
                            const gchar  *childname)
{
  GbView *self = (GbView *)buildable;
  GbViewPrivate *priv = gb_view_get_instance_private (self);

  g_assert (GB_IS_VIEW (self));

  if (g_strcmp0 (childname, "controls") == 0)
    return G_OBJECT (priv->controls);

  return NULL;
}

static void
buildable_iface_init (GtkBuildableIface *iface)
{
  iface->get_internal_child = gb_view_get_internal_child;
}

/**
 * gb_view_get_menu:
 *
 * Returns: (transfer none): A #GMenu that may be modified.
 */
GMenu *
gb_view_get_menu (GbView *self)
{
  GbViewPrivate *priv = gb_view_get_instance_private (self);

  g_return_val_if_fail (GB_IS_VIEW (self), NULL);

  return priv->menu;
}

/*
 * XXX:
 *
 * This function is a hack in place for 3.18 until we get the path bar
 * which will provide a better view of file paths. It should be removed
 * after 3.18 when path bar lands. Also remove the "special-title"
 * property.
 */
const gchar *
gb_view_get_special_title (GbView *self)
{
  const gchar *ret = NULL;

  g_return_val_if_fail (GB_IS_VIEW (self), NULL);

  if (GB_VIEW_GET_CLASS (self)->get_special_title)
    ret = GB_VIEW_GET_CLASS (self)->get_special_title (self);

  if (ret == NULL)
    ret = gb_view_get_title (self);

  return ret;
}
