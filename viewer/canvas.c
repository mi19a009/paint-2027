/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define NOTIFY_ZOOM   "notify::zoom"
#define PROPERTY_ZOOM "zoom"

typedef enum _ViewerCanvasProperties
{
	PROPID_NULL,
	PROPID_ZOOM,
} ViewerCanvasProperties;

struct _ViewerCanvas
{
	GtkDrawingArea   parent_instance;
	cairo_surface_t *image;
	gint             zoom;
};

static void viewer_canvas_class_init   (ViewerCanvasClass *canvas_class);
static void viewer_canvas_dispose      (GObject *object);
static void viewer_canvas_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_canvas_init         (ViewerCanvas *canvas);
static void viewer_canvas_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void             change_zoom         (GObject *object, GParamSpec *pspec, gpointer user_data);
static cairo_surface_t *create_image        (GdkPixbuf *pixbuf);
static void             draw                (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static gboolean         get_property        (ViewerCanvas *canvas, guint property_id, GValue *value);
static gboolean         set_property        (ViewerCanvas *canvas, guint property_id, const GValue *value);
static void             update_content_size (ViewerCanvas *canvas);

G_DEFINE_FINAL_TYPE (ViewerCanvas, viewer_canvas, GTK_TYPE_DRAWING_AREA);
#define PSPEC_ZOOM g_param_spec_int ("zoom", "Zoom", "Zoom percent.", VIEWER_CANVAS_MINIMUM_ZOOM, VIEWER_CANVAS_MAXIMUM_ZOOM, VIEWER_CANVAS_DEFAULT_ZOOM, G_PARAM_READWRITE)

gint
viewer_canvas_get_zoom (ViewerCanvas *canvas)
{
	g_return_val_if_fail (VIEWER_IS_CANVAS (canvas), 0);
	return canvas->zoom;
}

void
viewer_canvas_load (ViewerCanvas *canvas, GFile *file, GError **error)
{
	GdkPixbuf *pixbuf;
	g_return_if_fail (VIEWER_IS_CANVAS (canvas));
	pixbuf = paint_create_pixbuf_from_file (file, error);

	if (canvas->image != NULL)
	{
		cairo_surface_destroy (canvas->image);
	}
	if (pixbuf != NULL)
	{
		canvas->image = create_image (pixbuf);
		g_object_unref (pixbuf);
	}
	else
	{
		canvas->image = NULL;
	}

	update_content_size (canvas);
	gtk_widget_queue_draw (GTK_WIDGET (canvas));
}

GtkWidget *
viewer_canvas_new (void)
{
	return g_object_new (VIEWER_TYPE_CANVAS, NULL);
}

void
viewer_canvas_set_zoom (ViewerCanvas *canvas, gint zoom)
{
	g_return_if_fail (VIEWER_IS_CANVAS (canvas));
	zoom = CLAMP (zoom, VIEWER_CANVAS_MINIMUM_ZOOM, VIEWER_CANVAS_MAXIMUM_ZOOM);

	if (canvas->zoom != zoom)
	{
		canvas->zoom = zoom;
		g_object_notify (G_OBJECT (canvas), PROPERTY_ZOOM);
	}
}

GdkPixbuf *
viewer_canvas_to_pixbuf (ViewerCanvas *canvas)
{
	g_return_val_if_fail (VIEWER_IS_CANVAS (canvas), NULL);
	return paint_create_pixbuf_from_image (canvas->image);
}

static void
viewer_canvas_class_init (ViewerCanvasClass *canvas_class)
{
	G_OBJECT_CLASS (canvas_class)->dispose = viewer_canvas_dispose;
	G_OBJECT_CLASS (canvas_class)->get_property = viewer_canvas_get_property;
	G_OBJECT_CLASS (canvas_class)->set_property = viewer_canvas_set_property;
	g_object_class_install_property (G_OBJECT_CLASS (canvas_class), PROPID_ZOOM, PSPEC_ZOOM);
}

static void
viewer_canvas_dispose (GObject *object)
{
	g_clear_pointer (&VIEWER_CANVAS (object)->image, cairo_surface_destroy);
	G_OBJECT_CLASS (viewer_canvas_parent_class)->dispose (object);
}

static void
viewer_canvas_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	if (get_property (VIEWER_CANVAS (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_canvas_init (ViewerCanvas *canvas)
{
	canvas->zoom = VIEWER_CANVAS_DEFAULT_ZOOM;
	g_signal_connect (canvas, NOTIFY_ZOOM, G_CALLBACK (change_zoom), NULL);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (canvas), draw, NULL, NULL);
}

static void
viewer_canvas_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	if (set_property (VIEWER_CANVAS (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
change_zoom (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	update_content_size (VIEWER_CANVAS (object));
	gtk_widget_queue_draw (GTK_WIDGET (object));
}

static cairo_surface_t *
create_image (GdkPixbuf *pixbuf)
{
	cairo_t *cairo;
	cairo_surface_t *image;
	int width, height;
	width = gdk_pixbuf_get_width (pixbuf);
	height = gdk_pixbuf_get_height (pixbuf);
	image = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	cairo = cairo_create (image);
	cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
	cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
	gdk_cairo_set_source_pixbuf (cairo, pixbuf, 0, 0);
	cairo_paint (cairo);
	cairo_destroy (cairo);
	return image;
}

static void
draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	cairo_surface_t *image;
	double scale;
	int zoom;
	image = VIEWER_CANVAS (area)->image;

	if (image != NULL)
	{
		zoom = VIEWER_CANVAS (area)->zoom;

		if (zoom != VIEWER_CANVAS_DEFAULT_ZOOM)
		{
			scale = zoom / (double) VIEWER_CANVAS_DEFAULT_ZOOM;
			cairo_scale (cairo, scale, scale);
		}

		cairo_set_source_surface (cairo, image, 0, 0);
		cairo_paint (cairo);
	}
}

static gboolean
get_property (ViewerCanvas *canvas, guint property_id, GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_ZOOM:
		g_value_set_int (value, canvas->zoom);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static gboolean
set_property (ViewerCanvas *canvas, guint property_id, const GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_ZOOM:
		canvas->zoom = g_value_get_int (value);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static void
update_content_size (ViewerCanvas *canvas)
{
	float scale;
	int width, height;

	if (canvas->image != NULL)
	{
		width = cairo_image_surface_get_width (canvas->image);
		height = cairo_image_surface_get_height (canvas->image);

		if (canvas->zoom != VIEWER_CANVAS_DEFAULT_ZOOM)
		{
			scale = canvas->zoom / (float) VIEWER_CANVAS_DEFAULT_ZOOM;
			width = (int) (width * scale);
			height = (int) (height * scale);
		}
	}
	else
	{
		width = 0;
		height = 0;
	}

	gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (canvas), width);
	gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (canvas), height);
}
