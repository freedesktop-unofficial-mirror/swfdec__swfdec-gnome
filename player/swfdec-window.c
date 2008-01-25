/* Swfdec Player
 * Copyright (C) 2007 Benjamin Otte <otte@gnome.org>
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

#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include "swfdec-window.h"

G_DEFINE_TYPE (SwfdecWindow, swfdec_window, G_TYPE_OBJECT)

/* global list of windows */
static GSList *windows = NULL;
/* the default settings */
static const SwfdecWindowSettings default_settings = { FALSE, TRUE };

static void
swfdec_window_dispose (GObject *object)
{
  SwfdecWindow *window = SWFDEC_WINDOW (object);

  if (window->builder) {
    g_object_unref (window->builder);
    window->builder = NULL;
  }
  if (window->player) {
    g_object_unref (window->player);
    window->player = NULL;
  }

  G_OBJECT_CLASS (swfdec_window_parent_class)->dispose (object);
}

static void
swfdec_window_finalize (GObject *object)
{
  G_OBJECT_CLASS (swfdec_window_parent_class)->finalize (object);

  windows = g_slist_remove (windows, object);
  if (windows == NULL)
    gtk_main_quit ();
}

static void
swfdec_window_class_init (SwfdecWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = swfdec_window_dispose;
  object_class->finalize = swfdec_window_finalize;
}

static void
swfdec_window_init (SwfdecWindow *window)
{
  window->settings = default_settings;

  windows = g_slist_prepend (windows, window);
}

static void
swfdec_window_player_aborted (SwfdecPlayer *player, GParamSpec *pspec, SwfdecWindow *window)
{
  if (swfdec_as_context_is_aborted (SWFDEC_AS_CONTEXT (player)))
    swfdec_window_error (window, _("Broken Flash file, playback aborted."));
}

static void
swfdec_window_player_next_event (SwfdecPlayer *player, GParamSpec *pspec, SwfdecWindow *window)
{
#if 0
  gboolean eof, error;

  g_object_get (window->loader, "error", &error, "eof", &eof, NULL);
  if (error) {
    swfdec_window_error (window, _("Error loading <i>%s</i>."), 
	swfdec_loader_get_filename (window->loader));
  } else if (!swfdec_player_is_initialized (player) && 
      swfdec_player_get_next_event (player) < 0 &&
      eof) {
    swfdec_window_error (window, _("<i>%s</i> is not a Flash file."), 
	swfdec_loader_get_filename (window->loader));
  }
#endif
}

static void
swfdec_window_player_initialized (SwfdecPlayer *player, GParamSpec *pspec, SwfdecWindow *window)
{
  static const char *mime[2] = { "swfdec-player", NULL };
  GtkRecentData data = { NULL, NULL, (char *) "application/x-shockwave-flash",
    (char *) g_get_application_name (), g_strjoin (" ", g_get_prgname (), "%u", NULL), 
    (char **) mime, FALSE };

  if (swfdec_player_is_initialized (player)) {
    gtk_recent_manager_add_full (gtk_recent_manager_get_default (),
	swfdec_url_get_url (swfdec_player_get_url (window->player)),
	&data);
    g_signal_handlers_disconnect_by_func (player, swfdec_window_player_next_event, window);
  }
  g_free (data.app_exec);
}

/**
 * swfdec_window_set_url:
 * @window: the window that should show the given URL
 * @url: URL to show. Must be a valid file:// or http:// URL in UTF-8.
 *
 * Sets the URL of @window to be  @url, if no URL was set on @window before.
 *
 * Returns: %TRUE if the URL could be set, %FALSE if the window already shows a 
 *          movie.
 **/
gboolean
swfdec_window_set_url (SwfdecWindow *window, const char *url)
{
  SwfdecWindowSettings settings;
  SwfdecURL *u;
  GObject *o;
  char *s;

  g_return_val_if_fail (SWFDEC_IS_WINDOW (window), FALSE);
  g_return_val_if_fail (url != NULL, FALSE);

  if (window->player || window->error)
    return FALSE;

  window->player = swfdec_gtk_player_new (NULL);
  g_signal_connect (window->player, "notify::aborted", 
      G_CALLBACK (swfdec_window_player_aborted), window);
  g_signal_connect (window->player, "notify::initialized", 
      G_CALLBACK (swfdec_window_player_initialized), window);
  g_signal_connect (window->player, "notify::next-event", 
      G_CALLBACK (swfdec_window_player_next_event), window);
  u = swfdec_url_new_from_input (url);
  swfdec_player_set_url (window->player, u);
  s = swfdec_url_format_for_display (u);
  swfdec_url_free (u);
  swfdec_gtk_player_set_audio_enabled (SWFDEC_GTK_PLAYER (window->player), 
      window->settings.sound);
  o = gtk_builder_get_object (window->builder, "player-widget");
  swfdec_gtk_widget_set_player (SWFDEC_GTK_WIDGET (o), window->player);
  gtk_window_set_title (GTK_WINDOW (window->window), s);
  g_free (s);
  /* do this at the end to not get lag */
  swfdec_gtk_player_set_playing (SWFDEC_GTK_PLAYER (window->player), 
      window->settings.playing);

  return TRUE;
}

/**
 * swfdec_window_error:
 * @window: a window
 * @format: an error message to display for the user in printf-style format
 * @...: arguments for the function
 *
 * Shows the given error message to the user and aborts playback.
 * This function may be called at any time, no matter the state of the window 
 * object.
 **/
void
swfdec_window_error (SwfdecWindow *window, const char *format, ...)
{
  char *markup, *msg;
  va_list varargs;

  g_return_if_fail (SWFDEC_IS_WINDOW (window));
  g_return_if_fail (msg != NULL);

  /* NB: This function can be called during the construction process (like when
   * the UI file isn't found, so a window object may not even exist. */

  if (window->error)
    return;
  window->error = TRUE;
  va_start (varargs, format);
  msg = g_strdup_vprintf (format, varargs);
  va_end (varargs);

  if (window->window == NULL) {
    g_printerr ("%s\n", msg);
    g_free (msg);
    return;
  }
  /* Translators: This is used to markup error message. */
  markup = g_strdup_printf ("<b>%s</b>", msg);
  gtk_label_set_label (GTK_LABEL (gtk_builder_get_object (window->builder, 
	  "player-error-label")), markup);
  g_free (markup);
  g_free (msg);

  gtk_widget_show (GTK_WIDGET (gtk_builder_get_object (window->builder, 
	  "player-error-area")));
}

static void
swfdec_window_add_recent_filter (GtkRecentChooser *chooser)
{
  GtkRecentFilter *filter;

  filter = gtk_recent_filter_new ();
  gtk_recent_filter_add_group (filter, "swfdec-player");
  gtk_recent_chooser_set_filter (chooser, filter);
}

#define BUILDER_FILE DATADIR G_DIR_SEPARATOR_S "swfdec-gnome" G_DIR_SEPARATOR_S "swfdec-player.ui"
/**
 * swfdec_window_new:
 * @uri: a valid UTF-8 encoded URI.
 *
 * Opens a new window. If a URI was specified, it will be shown. Otherwise the
 * window will appear empty.
 *
 * Returns: The window that was just openend. You don't own a reference to it.
 **/
SwfdecWindow *
swfdec_window_new (const char *url)
{
  GError *error = NULL;
  SwfdecWindow *window;

  window = g_object_new (SWFDEC_TYPE_WINDOW, NULL);
  window->builder = gtk_builder_new ();
  gtk_builder_set_translation_domain (window->builder, GETTEXT_PACKAGE);
  if (!gtk_builder_add_from_file (window->builder, BUILDER_FILE, &error)) {
    swfdec_window_error (window, error->message);
    return window;
  }
  gtk_builder_connect_signals (window->builder, window);
  swfdec_window_add_recent_filter (GTK_RECENT_CHOOSER (
	gtk_builder_get_object (window->builder, "recent")));
  window->window = GTK_WIDGET (gtk_builder_get_object (window->builder, "player-window"));
  if (window->window == NULL) {
    swfdec_window_error (window, _("Broken user interface definition file"));
    return window;
  }
  g_object_weak_ref (G_OBJECT (window->window), (GWeakNotify) g_object_unref, window);
  if (url != NULL) {
    swfdec_window_set_url (window, url);
  }
  swfdec_window_set_settings (window, &window->settings);

  return window;
}

void
swfdec_window_set_settings (SwfdecWindow *window, const SwfdecWindowSettings *settings)
{
  SwfdecWindowSettings *org;

  g_return_if_fail (SWFDEC_IS_WINDOW (window));
  g_return_if_fail (settings != NULL);

  if (window->settings.playing != settings->playing) {
    GtkToggleAction *action = GTK_TOGGLE_ACTION (gtk_builder_get_object (window->builder, "play"));
    gtk_toggle_action_set_active (action, settings->playing);
    g_assert (window->settings.playing == settings->playing);
  }
  if (window->settings.sound != settings->sound) {
    GtkToggleAction *action = GTK_TOGGLE_ACTION (gtk_builder_get_object (window->builder, "mute"));
    gtk_toggle_action_set_active (action, !settings->sound);
    g_assert (window->settings.sound == settings->sound);
  }
}

