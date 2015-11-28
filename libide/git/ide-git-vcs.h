/* ide-git-vcs.h
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

#ifndef IDE_GIT_VCS_H
#define IDE_GIT_VCS_H

#include <libgit2-glib/ggit.h>

#include "ide-vcs.h"

G_BEGIN_DECLS

#define IDE_TYPE_GIT_VCS (ide_git_vcs_get_type())

G_DECLARE_FINAL_TYPE (IdeGitVcs, ide_git_vcs, IDE, GIT_VCS, IdeObject)

GgitRepository *ide_git_vcs_get_repository (IdeGitVcs *self);

G_END_DECLS

#endif /* IDE_GIT_VCS_H */
