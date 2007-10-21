/* Swfdec
 * Copyright (C) 2007 Benjamin Otte <otte@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __SWFDEC_WINDOW_H__
#define __SWFDEC_WINDOW_H__

#include <gtk/gtk.h>
#include <libswfdec/swfdec.h>
#include <libswfdec-gtk/swfdec-gtk.h>

typedef struct _SwfdecWindow SwfdecWindow;


SwfdecWindow *	swfdec_window_new		(const char *	url);

gboolean	swfdec_window_set_url		(SwfdecWindow *	window,
						 const char *	url);
void		swfdec_window_error		(SwfdecWindow *	window,
						 const char *	msg);

#endif
