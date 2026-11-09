/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#define PAINT_TYPE_PRINT_OPERATION (paint_print_operation_get_type ())

G_DECLARE_FINAL_TYPE (PaintPrintOperation, paint_print_operation, PAINT, PRINT_OPERATION, GtkPrintOperation);

/* Paint */
void       paint_bind_textdomain          (void);
GdkPixbuf *paint_create_pixbuf_from_file  (GFile *file, GError **error);
GdkPixbuf *paint_create_pixbuf_from_image (cairo_surface_t *image);
void       paint_show_error               (GtkWindow *window, const GError *error);

/* Paint Print Operation */
GdkPixbuf         *paint_print_operation_get_pixbuf (PaintPrintOperation *operation);
GtkPrintOperation *paint_print_operation_new        (void);
void               paint_print_operation_set_pixbuf (PaintPrintOperation *operation, GdkPixbuf *pixbuf);
