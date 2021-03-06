/* gb-greeter-window.h
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

#ifndef GB_GREETER_WINDOW_H
#define GB_GREETER_WINDOW_H

#include <gtk/gtk.h>
#include <ide.h>

G_BEGIN_DECLS

#define GB_TYPE_GREETER_WINDOW (gb_greeter_window_get_type())

G_DECLARE_FINAL_TYPE (GbGreeterWindow, gb_greeter_window, GB, GREETER_WINDOW, GtkApplicationWindow)

IdeRecentProjects *gb_greeter_window_get_recent_projects (GbGreeterWindow   *self);
void               gb_greeter_window_set_recent_projects (GbGreeterWindow   *self,
                                                          IdeRecentProjects *recent_projects);

G_END_DECLS

#endif /* GB_GREETER_WINDOW_H */
