/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"

#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define TITLE                 gettext ("Picture Viewer")

struct _ViewerWindow
{
	GtkApplicationWindow parent_instance;
};

static void viewer_window_class_init    (ViewerWindowClass *window_class);
static void viewer_window_dispose       (GObject *object);
static void viewer_window_get_property  (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_window_init          (ViewerWindow *window);
static void viewer_window_realize       (GtkWidget *widget);
static void viewer_window_set_property  (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_window_size_allocate (GtkWidget *widget, int width, int height, int baseline);
static void viewer_window_unrealize     (GtkWidget *widget);

G_DEFINE_FINAL_TYPE (ViewerWindow, viewer_window, GTK_TYPE_APPLICATION_WINDOW);

GtkWidget *
viewer_window_new (GtkApplication *application)
{
	return g_object_new (VIEWER_TYPE_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}

static void
viewer_window_class_init (ViewerWindowClass *window_class)
{
	G_OBJECT_CLASS (window_class)->dispose = viewer_window_dispose;
	G_OBJECT_CLASS (window_class)->get_property = viewer_window_get_property;
	G_OBJECT_CLASS (window_class)->set_property = viewer_window_set_property;
	GTK_WIDGET_CLASS (window_class)->realize = viewer_window_realize;
	GTK_WIDGET_CLASS (window_class)->size_allocate = viewer_window_size_allocate;
	GTK_WIDGET_CLASS (window_class)->unrealize = viewer_window_unrealize;
}

static void
viewer_window_dispose (GObject *object)
{
	G_OBJECT_CLASS (viewer_window_parent_class)->dispose (object);
}

static void
viewer_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
viewer_window_init (ViewerWindow *window)
{
	gtk_window_set_title (GTK_WINDOW (window), TITLE);
}

static void
viewer_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (viewer_window_parent_class)->realize (widget);
}

static void
viewer_window_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
viewer_window_size_allocate (GtkWidget *widget, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (viewer_window_parent_class)->size_allocate (widget, width, height, baseline);
}

static void
viewer_window_unrealize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (viewer_window_parent_class)->unrealize (widget);
}
