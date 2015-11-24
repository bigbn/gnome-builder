/* gbp-devhelp-search-result.h
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

#ifndef GBP_DEVHELP_SEARCH_RESULT_H
#define GBP_DEVHELP_SEARCH_RESULT_H

#include "ide-search-result.h"

G_BEGIN_DECLS

#define GBP_TYPE_DEVHELP_SEARCH_RESULT (gbp_devhelp_search_result_get_type())

G_DECLARE_FINAL_TYPE (GbpDevhelpSearchResult, gbp_devhelp_search_result, GBP, DEVHELP_SEARCH_RESULT, IdeSearchResult)

G_END_DECLS

#endif /* GBP_DEVHELP_SEARCH_RESULT_H */
