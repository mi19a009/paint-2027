/* Copyright (C) 2026 Taichi Murakami.
 * Viewer Window クラスを実装します。
 */

#include "header.h"
#include "paint.h"
#include "viewer.h"

#define ACTION_FULLSCREEN     "fullscreen"
#define NOTIFY_FILE           "notify::file"
#define NOTIFY_SETTINGS       "notify::settings"
#define NOTIFY_STATE          "notify::state"
#define NOTIFY_ZOOM           "notify::zoom"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_FILE         "file"
#define PROPERTY_SETTINGS     "settings"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define TITLE                 gettext ("Picture Viewer")
#define TITLE_FORMAT          "%s - %s"
#define TITLE_FORMAT_ZOOM     "%d%% %s - %s"
#define TITLE_MAX             128

typedef enum _ViewerWindowProperties
{
	PROPID_NULL,
	PROPID_FILE,
	PROPID_SETTINGS,
} ViewerWindowProperties;

struct _ViewerWindow
{
	GtkApplicationWindow parent_instance;
	GtkWidget           *canvas;
	GFile               *file;
	GSettings           *settings;
	gint                 width;
	gint                 height;
	guchar               maximized;
	guchar               fullscreen;
};

static void viewer_window_class_init    (ViewerWindowClass *window_class);
static void viewer_window_dispose       (GObject *object);
static void viewer_window_get_property  (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_window_init          (ViewerWindow *window);
static void viewer_window_realize       (GtkWidget *widget);
static void viewer_window_set_property  (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_window_size_allocate (GtkWidget *widget, int width, int height, int baseline);
static void viewer_window_unrealize     (GtkWidget *widget);

static void     change_file          (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     change_settings      (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     change_surface_state (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     change_canvas_zoom   (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     connect_surface      (GtkWidget *widget);
static void     disconnect_surface   (GtkWidget *widget);
static gboolean get_property         (ViewerWindow *window, guint property_id, GValue *value);
static void     init_canvas          (ViewerWindow *window);
static void     load_settings        (ViewerWindow *window);
static void     save_settings        (ViewerWindow *window);
static void     set_action_state     (GActionMap *action_map, const char *action_name, gboolean state);
static gboolean set_file             (ViewerWindow *window, GFile *file);
static gboolean set_property         (ViewerWindow *window, guint property_id, const GValue *value);
static gboolean set_settings         (ViewerWindow *window, GSettings *settings);
static void     update_size          (ViewerWindow *window);
static void     update_surface_state (ViewerWindow *window, GdkSurface *surface);
static void     update_title (ViewerWindow *window);

G_DEFINE_FINAL_TYPE (ViewerWindow, viewer_window, GTK_TYPE_APPLICATION_WINDOW);
#define PSPEC_FILE     g_param_spec_object ("file",     "File",     "Document file.",        G_TYPE_SETTINGS, G_PARAM_READWRITE)
#define PSPEC_SETTINGS g_param_spec_object ("settings", "Settings", "Application settings.", G_TYPE_SETTINGS, G_PARAM_READWRITE)

GFile *
viewer_window_get_file (ViewerWindow *window)
{
	g_return_val_if_fail (VIEWER_IS_WINDOW (window), NULL);
	return window->file;
}

GSettings *
viewer_window_get_settings (ViewerWindow *window)
{
	g_return_val_if_fail (VIEWER_IS_WINDOW (window), NULL);
	return window->settings;
}

void
viewer_window_load (ViewerWindow *window, GFile *file)
{
	GError *error;
	error = NULL;
	g_return_if_fail (VIEWER_IS_WINDOW (window));
	viewer_canvas_load (VIEWER_CANVAS (window->canvas), file, &error);

	if (error != NULL)
	{
		paint_show_error (GTK_WINDOW (window), error);
		g_error_free (error);
	}
}

GtkWidget *
viewer_window_new (GtkApplication *application)
{
	return g_object_new (VIEWER_TYPE_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}

void
viewer_window_set_file (ViewerWindow *window, GFile *file)
{
	g_return_if_fail (VIEWER_IS_WINDOW (window));

	if (set_file (window, file))
	{
		g_object_notify (G_OBJECT (window), PROPERTY_FILE);
	}
}

void
viewer_window_set_settings (ViewerWindow *window, GSettings *settings)
{
	g_return_if_fail (VIEWER_IS_WINDOW (window));

	if (set_settings (window, settings))
	{
		g_object_notify (G_OBJECT (window), PROPERTY_SETTINGS);
	}
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
	g_object_class_install_property (G_OBJECT_CLASS (window_class), PROPID_FILE, PSPEC_FILE);
	g_object_class_install_property (G_OBJECT_CLASS (window_class), PROPID_SETTINGS, PSPEC_SETTINGS);
}

static void
viewer_window_dispose (GObject *object)
{
	save_settings (VIEWER_WINDOW (object));
	g_clear_object (&VIEWER_WINDOW (object)->file);
	g_clear_object (&VIEWER_WINDOW (object)->settings);
	G_OBJECT_CLASS (viewer_window_parent_class)->dispose (object);
}

static void
viewer_window_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	if (get_property (VIEWER_WINDOW (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_window_init (ViewerWindow *window)
{
	g_signal_connect (window, NOTIFY_FILE, G_CALLBACK (change_file), NULL);
	g_signal_connect (window, NOTIFY_SETTINGS, G_CALLBACK (change_settings), NULL);
	init_canvas (window);
	update_title (window);
}

static void
viewer_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (viewer_window_parent_class)->realize (widget);
	connect_surface (widget);
}

static void
viewer_window_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	if (set_property (VIEWER_WINDOW (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_window_size_allocate (GtkWidget *widget, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (viewer_window_parent_class)->size_allocate (widget, width, height, baseline);
	update_size (VIEWER_WINDOW (widget));
}

static void
viewer_window_unrealize (GtkWidget *widget)
{
	disconnect_surface (widget);
	GTK_WIDGET_CLASS (viewer_window_parent_class)->unrealize (widget);
}

static void
change_file (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	update_title (VIEWER_WINDOW (object));
}

static void
change_settings (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	load_settings (VIEWER_WINDOW (object));
}

static void
change_surface_state (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	update_surface_state (VIEWER_WINDOW (user_data), GDK_SURFACE (object));
}

static void
change_canvas_zoom (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	update_title (VIEWER_WINDOW (user_data));
}

static void
connect_surface (GtkWidget *widget)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (widget));
	g_signal_connect (surface, NOTIFY_STATE, G_CALLBACK (change_surface_state), widget);
}

static void
disconnect_surface (GtkWidget *widget)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (GTK_NATIVE (widget));
	g_signal_handlers_disconnect_by_func (surface, change_surface_state, widget);
}

static gboolean
get_property (ViewerWindow *window, guint property_id, GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_FILE:
		g_value_set_object (value, window->file);
		break;
	case PROPID_SETTINGS:
		g_value_set_object (value, window->settings);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static void
init_canvas (ViewerWindow *window)
{
	GtkWidget *scrolled, *canvas;
	scrolled = gtk_scrolled_window_new ();
	canvas = viewer_canvas_new ();
	window->canvas = canvas;
	g_signal_connect (canvas, NOTIFY_ZOOM, G_CALLBACK (change_canvas_zoom), window);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolled), canvas);
	gtk_window_set_child (GTK_WINDOW (window), scrolled);
}

static void
load_settings (ViewerWindow *window)
{
	GSettings *settings;
	settings = window->settings;

	if (settings != NULL)
	{
		window->width = g_settings_get_int (settings, SETTINGS_WIDTH);
		window->height = g_settings_get_int (settings, SETTINGS_HEIGHT);
		window->maximized = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
		gtk_window_set_default_size (GTK_WINDOW (window), window->width, window->height);

		if (window->maximized)
		{
			gtk_window_maximize (GTK_WINDOW (window));
		}
		else
		{
			gtk_window_unmaximize (GTK_WINDOW (window));
		}
	}
}

static void
save_settings (ViewerWindow *window)
{
	GSettings *settings;
	settings = window->settings;

	if (settings != NULL)
	{
		g_settings_set_int (settings, SETTINGS_WIDTH, window->width);
		g_settings_set_int (settings, SETTINGS_HEIGHT, window->height);
		g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, window->maximized);
	}
}

static void
set_action_state (GActionMap *action_map, const char *action_name, gboolean state)
{
	GAction *action;
	action = g_action_map_lookup_action (action_map, action_name);

	if (G_IS_SIMPLE_ACTION (action))
	{
		g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (state));
	}
}

static gboolean
set_file (ViewerWindow *window, GFile *file)
{
	gboolean changed;
	changed = window->file != file;

	if (changed)
	{
		if (window->file != NULL)
		{
			g_object_unref (window->file);
		}
		if (file != NULL)
		{
			window->file = g_object_ref (file);
		}
		else
		{
			window->file = NULL;
		}
	}

	return changed;
}

static gboolean
set_property (ViewerWindow *window, guint property_id, const GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_FILE:
		set_file (window, g_value_get_object (value));
		break;
	case PROPID_SETTINGS:
		set_settings (window, g_value_get_object (value));
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static gboolean
set_settings (ViewerWindow *window, GSettings *settings)
{
	gboolean changed;
	changed = window->settings != settings;

	if (changed)
	{
		if (window->settings != NULL)
		{
			g_object_unref (window->settings);
		}
		if (settings != NULL)
		{
			window->settings = g_object_ref (settings);
		}
		else
		{
			window->settings = NULL;
		}
	}

	return changed;
}

static void
update_size (ViewerWindow *window)
{
	if (!window->maximized && !window->fullscreen)
	{
		gtk_window_get_default_size (GTK_WINDOW (window), &window->width, &window->height);
	}
}

static void
update_surface_state (ViewerWindow *window, GdkSurface *surface)
{
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	window->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
	window->fullscreen = (state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
	set_action_state (G_ACTION_MAP (window), ACTION_FULLSCREEN, window->fullscreen);
}

static void
update_title (ViewerWindow *window)
{
	GFile *file;
	char *base;
	int zoom;
	char title [TITLE_MAX];
	file = window->file;

	if (file != NULL)
	{
		base = g_file_get_basename (file);
		zoom = viewer_canvas_get_zoom (VIEWER_CANVAS (window->canvas));

		if (zoom != VIEWER_CANVAS_DEFAULT_ZOOM)
		{
			g_snprintf (title, TITLE_MAX, TITLE_FORMAT_ZOOM, zoom, base, TITLE);
		}
		else
		{
			g_snprintf (title, TITLE_MAX, TITLE_FORMAT, base, TITLE);
		}

		g_free (base);
	}
	else
	{
		strncpy (title, TITLE, TITLE_MAX);
	}

	gtk_window_set_title (GTK_WINDOW (window), title);
}
