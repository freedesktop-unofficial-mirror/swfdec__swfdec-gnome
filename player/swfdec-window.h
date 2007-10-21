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

#include <gtk/gtk.h>
#include <libswfdec/swfdec.h>
#include <libswfdec-gtk/swfdec-gtk.h>

#ifndef __SWFDEC_WINDOW_H__
#define __SWFDEC_WINDOW_H__


typedef struct _SwfdecWindow SwfdecWindow;
typedef struct _SwfdecWindowSettings SwfdecWindowSettings;
typedef struct _SwfdecWindowClass SwfdecWindowClass;

#define SWFDEC_TYPE_WINDOW                    (swfdec_window_get_type())
#define SWFDEC_IS_WINDOW(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SWFDEC_TYPE_WINDOW))
#define SWFDEC_IS_WINDOW_CLASS(klass)         (G_TYPE_CHECK_CLASS_TYPE ((klass), SWFDEC_TYPE_WINDOW))
#define SWFDEC_WINDOW(obj)                    (G_TYPE_CHECK_INSTANCE_CAST ((obj), SWFDEC_TYPE_WINDOW, SwfdecWindow))
#define SWFDEC_WINDOW_CLASS(klass)            (G_TYPE_CHECK_CLASS_CAST ((klass), SWFDEC_TYPE_WINDOW, SwfdecWindowClass))
#define SWFDEC_WINDOW_GET_CLASS(obj)          (G_TYPE_INSTANCE_GET_CLASS ((obj), SWFDEC_TYPE_WINDOW, SwfdecWindowClass))

struct _SwfdecWindowSettings
{
  gboolean		playing;	/* TRUE if this window should be playing automagically */
  gboolean		sound;		/* TRUE if sund is active */
};

struct _SwfdecWindow
{
  GObject		object;

  gboolean		error;		/* TRUE if we're in error */
  GtkBuilder *		builder;	/* builder instance to load from */
  GtkWidget *		window;		/* the toplevel window */
  SwfdecPlayer *	player;		/* the player we show or NULL if not initialized yet */
  SwfdecLoader *	loader;		/* the loader we use to load the content or NULL if not initialized yet */
  SwfdecWindowSettings	settings;	/* the settings that apply to this window */
};

struct _SwfdecWindowClass
{
  GObjectClass		object_class;
};

SwfdecWindow *	swfdec_window_new		(const char *			url);

gboolean	swfdec_window_set_url		(SwfdecWindow *			window,
						 const char *			url);
void		swfdec_window_error		(SwfdecWindow *			window,
						 const char *			msg);
void		swfdec_window_set_settings	(SwfdecWindow *			window,
						 const SwfdecWindowSettings *	settings);


G_END_DECLS
#endif
