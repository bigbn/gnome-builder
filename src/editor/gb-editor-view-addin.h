/* gb-editor-view-addin.h
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

#ifndef GB_EDITOR_VIEW_ADDIN_H
#define GB_EDITOR_VIEW_ADDIN_H

#include "gb-editor-view.h"

G_BEGIN_DECLS

#define GB_TYPE_EDITOR_VIEW_ADDIN (gb_editor_view_addin_get_type ())

G_DECLARE_INTERFACE (GbEditorViewAddin, gb_editor_view_addin, GB, EDITOR_VIEW_ADDIN, GObject)

struct _GbEditorViewAddinInterface
{
  GTypeInterface parent;

  void (*load)             (GbEditorViewAddin *self,
                            GbEditorView      *view);
  void (*unload)           (GbEditorViewAddin *self,
                            GbEditorView      *view);
  void (*language_changed) (GbEditorViewAddin *self,
                            const gchar       *language_id);
};

G_END_DECLS

#endif /* GB_EDITOR_VIEW_ADDIN_H */
