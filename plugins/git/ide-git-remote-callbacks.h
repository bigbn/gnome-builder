/* ide-git-remote-callbacks.h
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

#ifndef IDE_GIT_REMOTE_CALLBACKS_H
#define IDE_GIT_REMOTE_CALLBACKS_H

#include <libgit2-glib/ggit.h>

#include "ide-progress.h"

G_BEGIN_DECLS

#define IDE_TYPE_GIT_REMOTE_CALLBACKS (ide_git_remote_callbacks_get_type())

G_DECLARE_FINAL_TYPE (IdeGitRemoteCallbacks, ide_git_remote_callbacks,
                      IDE, GIT_REMOTE_CALLBACKS, GgitRemoteCallbacks)

GgitRemoteCallbacks *ide_git_remote_callbacks_new          (void);
gdouble              ide_git_remote_callbacks_get_fraction (IdeGitRemoteCallbacks *self);
IdeProgress         *ide_git_remote_callbacks_get_progress (IdeGitRemoteCallbacks *self);

G_END_DECLS

#endif /* IDE_GIT_REMOTE_CALLBACKS_H */
