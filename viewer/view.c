/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define PROPERTY_VALUE  "value"
#define PROPERTY_ZOOM   "zoom"
#define SIGNAL_RESPONSE "response"

typedef enum _ViewerViewDialogProperties
{
	PROPID_NULL,
	PROPID_ZOOM,
} ViewerViewDialogProperties;

struct _ViewerViewDialog
{
	GtkWindow      parent_instance;
	GtkAdjustment *zoom_adjustment;
	gint           zoom;
};

static void viewer_view_dialog_class_init   (ViewerViewDialogClass *dialog_class);
static void viewer_view_dialog_dispose      (GObject *object);
static void viewer_view_dialog_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_view_dialog_init         (ViewerViewDialog *dialog);
static void viewer_view_dialog_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void     click_accept_button (GtkButton *button, ViewerViewDialog *dialog);
static gboolean get_property        (ViewerViewDialog *dialog, guint property_id, GValue *value);
static gboolean set_property        (ViewerViewDialog *dialog, guint property_id, const GValue *value);

G_DEFINE_FINAL_TYPE (ViewerViewDialog, viewer_view_dialog, GTK_TYPE_WINDOW);
#define PSPEC_ZOOM g_param_spec_int ("zoom", "Zoom", "Canvas zoom.", VIEWER_CANVAS_MINIMUM_ZOOM, VIEWER_CANVAS_MAXIMUM_ZOOM, VIEWER_CANVAS_DEFAULT_ZOOM, G_PARAM_READWRITE)

gint
viewer_view_dialog_get_zoom (ViewerViewDialog *dialog)
{
	return dialog->zoom;
}

GtkWidget *
viewer_view_dialog_new (void)
{
	return g_object_new (VIEWER_TYPE_VIEW_DIALOG, NULL);
}

void
viewer_view_dialog_set_zoom (ViewerViewDialog *dialog, gint zoom)
{
	zoom = CLAMP (zoom, VIEWER_CANVAS_MINIMUM_ZOOM, VIEWER_CANVAS_MAXIMUM_ZOOM);

	if (dialog->zoom != zoom)
	{
		dialog->zoom = zoom;
		g_object_notify (G_OBJECT (dialog), PROPERTY_ZOOM);
	}
}

static void
viewer_view_dialog_class_init (ViewerViewDialogClass *dialog_class)
{
	g_signal_new (SIGNAL_RESPONSE, VIEWER_TYPE_VIEW_DIALOG, G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
	G_OBJECT_CLASS (dialog_class)->dispose = viewer_view_dialog_dispose;
	G_OBJECT_CLASS (dialog_class)->get_property = viewer_view_dialog_get_property;
	G_OBJECT_CLASS (dialog_class)->set_property = viewer_view_dialog_set_property;
	g_object_class_install_property (G_OBJECT_CLASS (dialog_class), PROPID_ZOOM, PSPEC_ZOOM);
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (dialog_class), VIEWER_VIEW_DIALOG_TEMPLATE);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (dialog_class), click_accept_button);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (dialog_class), ViewerViewDialog, zoom_adjustment);
}

static void
viewer_view_dialog_dispose (GObject *object)
{
	gtk_widget_dispose_template (GTK_WIDGET (object), VIEWER_TYPE_VIEW_DIALOG);
	G_OBJECT_CLASS (viewer_view_dialog_parent_class)->dispose (object);
}

static void
viewer_view_dialog_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	if (get_property (VIEWER_VIEW_DIALOG (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_view_dialog_init (ViewerViewDialog *dialog)
{
	gtk_widget_init_template (GTK_WIDGET (dialog));
	g_object_bind_property (dialog->zoom_adjustment, PROPERTY_VALUE, dialog, PROPERTY_ZOOM, G_BINDING_BIDIRECTIONAL);
}

static void
viewer_view_dialog_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	if (set_property (VIEWER_VIEW_DIALOG (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
click_accept_button (GtkButton *button, ViewerViewDialog *dialog)
{
	g_signal_emit_by_name (dialog, SIGNAL_RESPONSE);
}

static gboolean
get_property (ViewerViewDialog *dialog, guint property_id, GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_ZOOM:
		g_value_set_int (value, dialog->zoom);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static gboolean
set_property (ViewerViewDialog *dialog, guint property_id, const GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_ZOOM:
		dialog->zoom = g_value_get_int (value);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}
