/* gb-recent-project-row.h
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

#ifndef GB_RECENT_PROJECT_ROW_H
#define GB_RECENT_PROJECT_ROW_H

#include <gtk/gtk.h>
#include <ide.h>

G_BEGIN_DECLS

#define GB_TYPE_RECENT_PROJECT_ROW (gb_recent_project_row_get_type())

G_DECLARE_FINAL_TYPE (GbRecentProjectRow, gb_recent_project_row,
                      GB, RECENT_PROJECT_ROW, GtkListBoxRow)

GtkWidget      *gb_recent_project_row_new              (IdeProjectInfo     *project_info);
IdeProjectInfo *gb_recent_project_row_get_project_info (GbRecentProjectRow *self);
gboolean        gb_recent_project_row_get_selected     (GbRecentProjectRow *self);
void            gb_recent_project_row_set_selected     (GbRecentProjectRow *self,
                                                        gboolean            selected);

G_END_DECLS

#endif /* GB_RECENT_PROJECT_ROW_H */
