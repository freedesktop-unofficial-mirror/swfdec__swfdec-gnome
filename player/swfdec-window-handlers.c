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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "swfdec-window.h"

void
menu_file_open (GtkAction *action, SwfdecWindow *window)
{
}

void
menu_file_play (GtkAction *action, SwfdecWindow *window)
{
}

void
menu_file_close (GtkAction *action, SwfdecWindow *window)
{
  gtk_widget_destroy (window->window);
}

void
menu_help_about (GtkAction *action, SwfdecWindow *window)
{
  static const char *authors[] = {
    "Benjamin Otte <otte@gnome.org>",
    "Pekka Lampila <pekka.lampila@iki.fi>",
    NULL,
  };
  static const char *artists[] = {
    "Cristian Grada <krigenator@gmail.com>",
    NULL
  };

  gtk_show_about_dialog (NULL, 
      "logo-icon-name", "swfdec",
      "authors", authors,
      "artists", artists,
      "comments", "Play Adobe Flash files",
      "version", VERSION,
      "website", "http://swfdec.freedesktop.org/",
      NULL);
}

gboolean
main_window_destroy_cb (GtkWidget *widget, GdkEvent *event, SwfdecWindow *window)
{
  return FALSE;
}
