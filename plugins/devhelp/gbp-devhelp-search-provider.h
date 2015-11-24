/* gbp-devhelp-search-provider.h
 *
 * Copyright (C) 2015 Erick Pérez Castellanos <erick.red@gmail.com>
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

#ifndef GBP_DEVHELP_SEARCH_PROVIDER_H
#define GBP_DEVHELP_SEARCH_PROVIDER_H

#include "ide-search-provider.h"

G_BEGIN_DECLS

#define GBP_TYPE_DEVHELP_SEARCH_PROVIDER (gbp_devhelp_search_provider_get_type())

G_DECLARE_FINAL_TYPE (GbpDevhelpSearchProvider, gbp_devhelp_search_provider, GBP, DEVHELP_SEARCH_PROVIDER, IdeObject)

G_END_DECLS

#endif /* GBP_DEVHELP_SEARCH_PROVIDER_H */
