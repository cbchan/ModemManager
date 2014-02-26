/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2014 Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef MM_MODEM_HELPERS_CINTERION_H
#define MM_MODEM_HELPERS_CINTERION_H

#include "glib.h"

/*****************************************************************************/
/* ^SCFG test parser */

gboolean mm_cinterion_parse_scfg_3g_test (const gchar *response,
                                          GArray **supported_bands,
                                          GError **error);

/*****************************************************************************/
/* ^SCFG response parser */

gboolean mm_cinterion_parse_scfg_3g_response (const gchar *response,
                                              GArray **bands,
                                              GError **error);

#endif  /* MM_MODEM_HELPERS_CINTERION_H */