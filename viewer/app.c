/* Copyright (C) 2026 Taichi Murakami. */

#include "header.h"
#include "viewer.h"

#define APPLICATION_FLAGS       G_APPLICATION_HANDLES_OPEN
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"

struct _ViewerApplication
{
	GtkApplication parent_instance;
};

static void viewer_application_activate     (GApplication *application);
static void viewer_application_class_init   (ViewerApplicationClass *application_class);
static void viewer_application_dispose      (GObject *object);
static void viewer_application_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_application_init         (ViewerApplication *application);
static void viewer_application_open         (GApplication *application, GFile **files, int n_files, const char *hint);
static void viewer_application_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_application_startup      (GApplication *application);

static GtkWindow  *create_window (ViewerApplication *application);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);

ViewerApplication *
viewer_application_new (const gchar *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		PROPERTY_APPLICATION_ID, application_id,
		PROPERTY_FLAGS, APPLICATION_FLAGS,
		NULL);
}

static void
viewer_application_activate (GApplication *application)
{
	GtkWindow *window;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));

	if (window == NULL)
	{
		window = create_window (VIEWER_APPLICATION (application));
	}

	gtk_window_present (window);
}

static void
viewer_application_class_init (ViewerApplicationClass *application_class)
{
	G_OBJECT_CLASS (application_class)->dispose = viewer_application_dispose;
	G_OBJECT_CLASS (application_class)->get_property = viewer_application_get_property;
	G_OBJECT_CLASS (application_class)->set_property = viewer_application_set_property;
	G_APPLICATION_CLASS (application_class)->activate = viewer_application_activate;
	G_APPLICATION_CLASS (application_class)->open = viewer_application_open;
	G_APPLICATION_CLASS (application_class)->startup = viewer_application_startup;
}

static void
viewer_application_dispose (GObject *object)
{
	G_OBJECT_CLASS (viewer_application_parent_class)->dispose (object);
}

static void
viewer_application_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
viewer_application_init (ViewerApplication *application)
{
}

static void
viewer_application_open (GApplication *application, GFile **files, int n_files, const char *hint)
{
}

static void
viewer_application_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void
viewer_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (viewer_application_parent_class)->startup (application);
}

static GtkWindow *
create_window (ViewerApplication *application)
{
	GtkWidget *window;
	window = viewer_window_new (GTK_APPLICATION (application));
	return GTK_WINDOW (window);
}
