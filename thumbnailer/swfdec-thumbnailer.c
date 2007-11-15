/* Swfdec Thumbnailer
 * Copyright (C) 2003, 2004 Bastien Nocera <hadess@hadess.net>
 *               2007 Pekka Lampila <pekka.lampila@iki.fi>
 *               2007 Benjamin Otte <otte@gnome.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
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
is_image_interesting (cairo_surface_t *surface, int x, int y, int width,
    int height)
{
  /* We're gonna assume 8-bit samples. If anyone uses anything different,
   * it doesn't really matter cause it's gonna be ugly anyways */
  int stride, rowcount, num_samples, i, j;
  guchar *buffer;
  float x_bar, variance;

  g_return_val_if_fail (surface != NULL, FALSE);
  g_return_val_if_fail (x + width <= cairo_image_surface_get_width (surface),
      FALSE);
  g_return_val_if_fail (y + height <= cairo_image_surface_get_height (surface),
      FALSE);

  stride = cairo_image_surface_get_stride (surface);
  rowcount = cairo_image_surface_get_height (surface);
  buffer = cairo_image_surface_get_data (surface);

  x *= stride / cairo_image_surface_get_width (surface);
  width *= stride / cairo_image_surface_get_width (surface);

  num_samples = width * height;

  /* Iterate through the image to calculate x-bar */
  x_bar = 0.0f;
  for (i = y; i < y + height; i++) {
    for (j = x; j < x + width; j++) {
      x_bar += (float) buffer[i * stride + j];
    }
  }
  x_bar /= ((float) num_samples);

  /* Calculate the variance */
  variance = 0.0f;
  for (i = y; i < y + height; i++) {
    for (j = x; j < x + width; j++) {
      float tmp = ((float) buffer[i * stride + j] - x_bar);
      variance += tmp * tmp;
    }
  }
  variance /= ((float) (num_samples - 1));

  return (variance > BORING_IMAGE_VARIANCE);
}

static gboolean
advance (SwfdecPlayer *player, GTimer *timer, gulong advance_msecs)
{
  static const gulong max_runtime = 15 * 1000; /* 15 seconds per file */
  gulong elapsed;

  elapsed = (gulong) (g_timer_elapsed (timer, NULL) * 1000);
  if (elapsed >= max_runtime)
    return FALSE;
    
  swfdec_player_set_maximum_runtime (player, max_runtime - elapsed);
  swfdec_player_advance (player, advance_msecs);
  return TRUE;
}

int
main (int argc, char **argv)
{
  GOptionContext *context;
  GError *err;
  SwfdecPlayer *player;
  SwfdecLoader *loader;
  guint width, height;
  double scale, scaled_size, x, y, w, h;
  guint try;
  cairo_surface_t *surface;
  cairo_t *cr;
  int size = 128;
  char **filenames = NULL;
  GTimer *timer = NULL;
  gboolean time_left;
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
  swfdec_init ();

  // read command line params
  context = g_option_context_new ("Create a thumbnail for Flash file");
  g_option_context_add_main_entries (context, entries, NULL);

  if (g_option_context_parse (context, &argc, &argv, &err) == FALSE) {
    g_printerr ("Couldn't parse command-line options: %s\n", err->message);
    g_error_free (err);
    return 1;
  }

  if (filenames == NULL || g_strv_length (filenames) != 2) {
    g_printerr ("One input and one output filename required\n");
    return 1;
  }

  loader = swfdec_file_loader_new (filenames[0]);
  if (loader->error) {
    g_printerr ("Error loading %s: %s\n", filenames[0], loader->error);
    g_object_unref (loader);
    return 1;
  }

  timer = g_timer_new ();
  player = swfdec_player_new (NULL);
  swfdec_player_set_loader (player, loader);
  g_timer_start (timer);

  /* Skip the first 10 seconds */
  /* Cheat: We do this in one call so that the max runtime kicks in */
  time_left = advance (player, timer, 10 * 1000);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, size, size);
  cr = cairo_create (surface);

  // get image size
  swfdec_player_get_default_size (player, &width, &height);
  if (width == 0 || height == 0) {
    /* force a size if the player doesn't have a default one */
    swfdec_player_set_size (player, size, size);
    width = height = size;
  }

  // determine amount of scaling to apply
  if (width > 2 * height) {
    scale = 0.5 * size / height;
  } else  if (height > 2 * width) {
    scale = 0.5 * size / width;
  } else {
    scale = (double) size / MAX (width, height);
  }

  // determine necessary translation to get image centered
  scaled_size = size / scale;
  x = (width - scaled_size) / 2;
  y = (height - scaled_size) / 2;
  cairo_scale (cr, scale, scale);
  cairo_translate (cr, -x, -y);

  // compute part to render
  x = MAX (x, 0);
  y = MAX (y, 0);
  w = MIN (width, scaled_size);
  h = MIN (height, scaled_size);

  // render the image
  swfdec_player_render (player, cr, x, y, w, h);

  for (try = 0; try < 10 && time_left; try++)
  {
    if (is_image_interesting (surface, (size - w * scale) / 2,
	  (size - h * scale) / 2, w * scale, h * scale))
	break;
    time_left = advance (player, timer, 1000);
    swfdec_player_render (player, cr, x, y, w, h);
  }

  cairo_destroy (cr);
  g_object_unref (player);
  g_timer_destroy (timer);

  cairo_surface_write_to_png (surface, filenames[1]);
  cairo_surface_destroy (surface);

  return 0;
}
