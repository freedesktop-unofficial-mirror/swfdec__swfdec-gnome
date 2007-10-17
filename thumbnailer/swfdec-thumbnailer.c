/* Swfdec Thumbnailer
 * Copyright (C) 2003, 2004 Bastien Nocera <hadess@hadess.net>
 *                     2007 Pekka Lampila <pekka.lampila@iki.fi>
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

#define BORING_IMAGE_VARIANCE 256.0		/* Tweak this if necessary */

/* Copied from totem-video-thumbnailer.c and ported from GdkPixbuf to cairo */
/* This function attempts to detect images that are mostly solid images
 * It does this by calculating the statistical variance of the
 * black-and-white image */
static gboolean
is_image_interesting (cairo_surface_t *surface)
{
  /* We're gonna assume 8-bit samples. If anyone uses anything different,
   * it doesn't really matter cause it's gonna be ugly anyways */
  int rowstride = cairo_image_surface_get_stride (surface);
  int height = cairo_image_surface_get_height (surface);
  guchar* buffer = cairo_image_surface_get_data (surface);
  int num_samples = (rowstride * height);
  int i;
  float x_bar = 0.0f;
  float variance = 0.0f;

  /* FIXME: If this proves to be a performance issue, this function
   * can be modified to perhaps only check 3 rows. I doubt this'll
   * be a problem though. */

  /* Iterate through the image to calculate x-bar */
  for (i = 0; i < num_samples; i++) {
    x_bar += (float) buffer[i];
  }
  x_bar /= ((float) num_samples);

  /* Calculate the variance */
  for (i = 0; i < num_samples; i++) {
    float tmp = ((float) buffer[i] - x_bar);
    variance += tmp * tmp;
  }
  variance /= ((float) (num_samples - 1));

  return (variance > BORING_IMAGE_VARIANCE);
}


int
main (int argc, char **argv)
{
  GOptionContext *context;
  GError *err;
  SwfdecPlayer *player;
  SwfdecLoader *loader;
  int width, height;
  float ratio, offset_x, offset_y;
  guint try, i, msecs, total;
  cairo_surface_t *surface;
  cairo_t *cr;
  int size = 128;
  char **filenames = NULL;
  const GOptionEntry entries[] = {
    {
      "size", 's', 0, G_OPTION_ARG_INT, &size,
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

  // skip the start
  for (i = 0, total = 0; i < 1000 && total < 10000; i++) {
    msecs = swfdec_player_get_next_event (player);
    if (msecs == -1)
      break;
    swfdec_player_advance (player, msecs);
    total += msecs;
  }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, size, size);
  cr = cairo_create (surface);

  // render the image
  try = 1;
  do {
    for (i = 0, total = 0; i < 100 && total < 1000; i++) {
      msecs = swfdec_player_get_next_event (player);
      if (msecs == -1)
	break;
      swfdec_player_advance (player, msecs);
      total += msecs;
    }

    swfdec_player_get_image_size (player, &width, &height);
    g_return_val_if_fail (width > 0 && height > 0, 1);

    ratio = MIN (size / (float)width, size / (float)height);
    offset_x = (width * ratio - size) / 2 / ratio;
    offset_y = (height * ratio - size) / 2 / ratio;

    cairo_scale (cr, ratio, ratio);
    cairo_translate (cr, -offset_x, -offset_y);
    swfdec_player_render (player, cr, offset_x, offset_y, size / ratio,
	size / ratio);
  } while (msecs != -1 && !is_image_interesting (surface) && try++ < 10);

  cairo_surface_write_to_png (surface, filenames[1]);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  g_object_unref (player);

  return 0;
}
