/* Copyright (C) 2026 Taichi Murakami. */

#include <gtk/gtk.h>

GdkPixbuf *paint_create_pixbuf_from_file  (GFile *file, GError **error);
GdkPixbuf *paint_create_pixbuf_from_image (cairo_surface_t *image);
void       paint_show_error               (GtkWindow *window, const GError *error);
