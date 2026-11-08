/* Copyright (C) 2026 Taichi Murakami.
 * Viewer Window クラスを実装します。
 */

#include "header.h"
#include "viewer.h"

#define ACTION_FULLSCREEN     "fullscreen"
#define NOTIFY_SETTINGS       "notify::settings"
#define NOTIFY_STATE          "notify::state"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SETTINGS     "settings"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define TITLE                 gettext ("Picture Viewer")

typedef enum _ViewerWindowProperties
{
	PROPID_NULL,
	PROPID_SETTINGS,
} ViewerWindowProperties;

struct _ViewerWindow
{
	GtkApplicationWindow parent_instance;
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

static void     change_settings      (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     change_surface_state (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     connect_surface      (GtkWidget *widget);
static void     disconnect_surface   (GtkWidget *widget);
static gboolean get_property         (ViewerWindow *window, guint property_id, GValue *value);
static void     load_settings        (ViewerWindow *window);
static void     save_settings        (ViewerWindow *window);
static void     set_action_state     (GActionMap *action_map, const char *action_name, gboolean state);
static gboolean set_property         (ViewerWindow *window, guint property_id, const GValue *value);
static gboolean set_settings         (ViewerWindow *window, GSettings *settings);
static void     update_size          (ViewerWindow *window);
static void     update_surface_state (ViewerWindow *window, GdkSurface *surface);

G_DEFINE_FINAL_TYPE (ViewerWindow, viewer_window, GTK_TYPE_APPLICATION_WINDOW);
#define PSPEC_SETTINGS g_param_spec_object ("settings", "Settings", "Application settings.", G_TYPE_SETTINGS, G_PARAM_READWRITE)

GSettings *
viewer_window_get_settings (ViewerWindow *window)
{
	g_return_val_if_fail (VIEWER_IS_WINDOW (window), NULL);
	return window->settings;
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
	g_object_class_install_property (G_OBJECT_CLASS (window_class), PROPID_SETTINGS, PSPEC_SETTINGS);
}

static void
viewer_window_dispose (GObject *object)
{
	save_settings (VIEWER_WINDOW (object));
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
	g_signal_connect (window, NOTIFY_SETTINGS, G_CALLBACK (change_settings), NULL);
	gtk_window_set_title (GTK_WINDOW (window), TITLE);
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

static gboolean
set_property (ViewerWindow *window, guint property_id, const GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_SETTINGS:
		set_settings (window, g_value_get_object (value));
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
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
