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

static SwfdecPlayer *
load_file (const char *filename)
{
  SwfdecPlayer *player;
  SwfdecLoader *loader;

  g_return_val_if_fail (filename != NULL, NULL);

  loader = swfdec_file_loader_new (filename);
  if (loader->error) {
    g_printerr ("Error loading %s: %s\n", filename, loader->error);
    g_object_unref (loader);
    return NULL;
  }

  player = swfdec_player_new (NULL);
  swfdec_player_set_loader (player, loader);

  return player;
}

static gboolean
seek_and_resize (SwfdecPlayer *player, int target_width, int target_height)
{
  int w, h;
  guint i, msecs;

  g_return_val_if_fail (SWFDEC_IS_PLAYER (player), FALSE);
  g_return_val_if_fail (target_width > 0, FALSE);
  g_return_val_if_fail (target_height > 0, FALSE);

  for (i = 0; i < 5; i++) {
    msecs = swfdec_player_get_next_event (player);
    swfdec_player_advance (player, msecs);
    swfdec_player_get_image_size (player, &w, &h);
    if (w != 0 && h != 0)
      break;
  }

  swfdec_player_set_size (player, target_width, target_height);

  for (i = 0; i < 10; i++) {
    msecs = swfdec_player_get_next_event (player);
    swfdec_player_advance (player, msecs);
  }

  return TRUE;
}

static gboolean
generate_image (SwfdecPlayer *player, const char *filename, int target_width,
    int target_height)
{
  int w, h;
  cairo_surface_t *surface;
  cairo_t *cr;

  g_return_val_if_fail (SWFDEC_IS_PLAYER (player), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);
  g_return_val_if_fail (target_width > 0, FALSE);
  g_return_val_if_fail (target_height > 0, FALSE);

  swfdec_player_get_size (player, &w, &h);

  g_return_val_if_fail (w > 0, FALSE);
  g_return_val_if_fail (h > 0, FALSE);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, target_width,
      target_height);

  cr = cairo_create (surface);
  cairo_scale (cr, w / target_width, h / target_height);
  swfdec_player_render (player, cr, 0, 0, w, h);
  cairo_destroy (cr);

  cairo_surface_write_to_png (surface, filename);
  cairo_surface_destroy (surface);

  return TRUE;
}

int
main (int argc, char **argv)
{
  SwfdecPlayer *player;

  swfdec_init ();

  if ((player = load_file ("test.swf")) == NULL)
    return 1;

  if (seek_and_resize (player, 100, 100) == FALSE) {
    g_object_unref (player);
    return 2;
  }

  if (generate_image (player, "test.png", 100, 100) == FALSE) {
    g_object_unref (player);
    return 3;
  }

  g_object_unref (player);

  return 0;
}
