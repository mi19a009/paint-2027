/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "paint.h"
#define ALERT_DIALOG_FORMAT "%d: %s"
#define BITS_PER_SAMPLE     8
#define LINK                "/proc/self/exe"
#define TEXTDOMAIN          "paint"
#define TEXTDOMAIN_CODESET  "UTF-8"
#define TEXTDOMAIN_FORMAT   "%d: %s\n"
#define RGBA(r,g,b,a) (((r) & 0xff) | (((g) & 0xff) << 8) | (((b) & 0xff) << 16) | (((a) & 0xff) << 24))
#define GET_X(n)      ((n) & 0xff)
#define GET_Y(n)      (((n) >> 8) & 0xff)
#define GET_Z(n)      (((n) >> 16) & 0xff)
#define GET_W(n)      (((n) >> 24) & 0xff)

static char *getdir (GError **error);

void
paint_bind_textdomain (void)
{
	GError *error;
	char *dir;
	error = NULL;
	dir = getdir (&error);

	if (dir != NULL)
	{
		bindtextdomain (TEXTDOMAIN, dir);
		bind_textdomain_codeset (TEXTDOMAIN, TEXTDOMAIN_CODESET);
		textdomain (TEXTDOMAIN);
		g_free (dir);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, TEXTDOMAIN_FORMAT, error->code, error->message);
		g_error_free (error);
	}
}

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
	dialog = gtk_alert_dialog_new (ALERT_DIALOG_FORMAT, error->code, error->message);
	gtk_alert_dialog_show (dialog, window);
	g_object_unref (dialog);
}

static char *
getdir (GError **error)
{
	char *path, *dir;
	path = g_file_read_link (LINK, error);

	if (path != NULL)
	{
		dir = g_path_get_dirname (path);
		g_free (path);
	}
	else
	{
		dir = NULL;
	}

	return dir;
}
