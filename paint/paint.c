/* Copyright (C) 2026 Taichi Murakami. */

#include "header.h"
#include "paint.h"

#define BITS_PER_SAMPLE 8
#define ERROR_FORMAT "%d: %s"
#define RGBA(r,g,b,a) (((r) & 0xff) | (((g) & 0xff) << 8) | (((b) & 0xff) << 16) | (((a) & 0xff) << 24))
#define GET_X(n) ((n) & 0xff)
#define GET_Y(n) (((n) >> 8) & 0xff)
#define GET_Z(n) (((n) >> 16) & 0xff)
#define GET_W(n) (((n) >> 24) & 0xff)

GdkPixbuf *
paint_create_pixbuf_from_file (GFile *file, GError **error)
{
	GFileInputStream *stream;
	GdkPixbuf *pixbuf;
	stream = g_file_read (file, NULL, error);

	if (stream != NULL)
	{
		pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, error);
		g_object_unref (stream);
	}
	else
	{
		pixbuf = NULL;
	}

	return pixbuf;
}

GdkPixbuf *
paint_create_pixbuf_from_image (cairo_surface_t *image)
{
	const guchar *data;
	const guint *src;
	guchar *pixels;
	guint *dest;
	GdkPixbuf *pixbuf;
	int width, height, stride, rowstride, y, x;
	guint value;
	pixbuf = NULL;
	data = cairo_image_surface_get_data (image);

	if (data != NULL)
	{
		width = cairo_image_surface_get_width (image);
		height = cairo_image_surface_get_height (image);
		stride = cairo_image_surface_get_stride (image);
		pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, BITS_PER_SAMPLE, width, height);
		pixels = gdk_pixbuf_get_pixels (pixbuf);
		rowstride = gdk_pixbuf_get_rowstride (pixbuf);

		for (y = 0; y < height; y++)
		{
			src = (guint *) (data + (y * stride));
			dest = (guint *) (pixels + (y * rowstride));

			for (x = 0; x < width; x++)
			{
				value = *(src++);
				*(dest++) = RGBA (GET_Z (value), GET_Y (value), GET_X (value), GET_W (value));
			}
		}
	}

	return pixbuf;
}

void
paint_show_error (GtkWindow *window, const GError *error)
{
	GtkAlertDialog *dialog;
	dialog = gtk_alert_dialog_new (ERROR_FORMAT, error->code, error->message);
	gtk_alert_dialog_show (dialog, window);
	g_object_unref (dialog);
}
