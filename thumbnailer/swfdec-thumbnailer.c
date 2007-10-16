/* Swfdec Thumbnailer
 * Copyright (C) 2007 Pekka Lampila <pekka.lampila@iki.fi>
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

#include <libswfdec/swfdec.h>

int
main (int argc, char **argv)
{
  GOptionContext *context;
  GError *err;
  SwfdecPlayer *player;
  SwfdecLoader *loader;
  int width, height;
  guint i, msecs, total;
  cairo_surface_t *surface;
  cairo_t *cr;
  int size_output = 128;
  char **filenames = NULL;
  const GOptionEntry entries[] = {
    {
      "size", 's', 0, G_OPTION_ARG_INT, &size_output,
      "Size of the thumbnail in pixels (default 128)", NULL
    },
    {
      G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &filenames,
      NULL, "<INPUT FILE> <OUTPUT FILE>"
    },
    {
      NULL
    }
  };

  // init
  context = g_option_context_new ("Create a thumbnail for Flash file");
  g_option_context_add_main_entries (context, entries, NULL);
  g_type_init ();

  // read command line params
  if (g_option_context_parse (context, &argc, &argv, &err) == FALSE) {
    g_printerr ("Couldn't parse command-line options: %s\n", err->message);
    g_error_free (err);
    return 1;
  }

  if (filenames == NULL || g_strv_length (filenames) != 2) {
    g_printerr ("One input and one output filename required\n");
    return 1;
  }

  // init swfdec
  swfdec_init ();

  loader = swfdec_file_loader_new (filenames[0]);
  if (loader->error) {
    g_printerr ("Error loading %s: %s\n", filenames[0], loader->error);
    g_object_unref (loader);
    return 1;
  }

  player = swfdec_player_new (NULL);
  swfdec_player_set_loader (player, loader);

  // get the image size of the file
  for (i = 0; i < 5; i++) {
    msecs = swfdec_player_get_next_event (player);
    swfdec_player_advance (player, msecs);
    swfdec_player_get_image_size (player, &width, &height);
    if (width > 0 && height > 0)
      break;
  }

  // resize player to target size
  swfdec_player_set_size (player, size_output, size_output);

  // advance some more
  total = 0;
  for (i = 0; i < 250 && total < 10000; i++) {
    msecs = swfdec_player_get_next_event (player);
    swfdec_player_advance (player, msecs);
    total += msecs;
  }

  // render the image
  swfdec_player_get_size (player, &width, &height);
  g_return_val_if_fail (width > 0 && height > 0, 1);

  surface =
    cairo_image_surface_create (CAIRO_FORMAT_ARGB32, size_output, size_output);

  cr = cairo_create (surface);
  cairo_scale (cr, width / size_output, height / size_output);
  swfdec_player_render (player, cr, 0, 0, width, height);
  cairo_destroy (cr);

  cairo_surface_write_to_png (surface, filenames[1]);
  cairo_surface_destroy (surface);

  g_object_unref (player);

  return 0;
}
