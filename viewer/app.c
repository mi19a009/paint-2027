/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define ACTION_QUIT             "quit"
#define APPLICATION_FLAGS       G_APPLICATION_HANDLES_OPEN
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"
#define PROPERTY_SETTINGS       "settings"

typedef enum _ViewerApplicationProperties
{
	PROPID_NULL,
	PROPID_SETTINGS,
} ViewerApplicationProperties;

typedef struct _ViewerApplicationAccelEntry
{
	const gchar *detailed_action_name;
	const gchar *const *accels;
} ViewerApplicationAccelEntry;

typedef struct _ViewerApplicationActionEntry
{
	const char *name;
	void (*activate) (GSimpleAction *action, GVariant *parameter, gpointer user_data);
} ViewerApplicationActionEntry;

struct _ViewerApplication
{
	GtkApplication parent_instance;
	GSettings     *settings;
};

static void viewer_application_activate     (GApplication *application);
static void viewer_application_class_init   (ViewerApplicationClass *application_class);
static void viewer_application_dispose      (GObject *object);
static void viewer_application_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void viewer_application_init         (ViewerApplication *application);
static void viewer_application_open         (GApplication *application, GFile **files, int n_files, const char *hint);
static void viewer_application_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_application_startup      (GApplication *application);

static void        activate_quit      (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static GSettings  *create_settings    (GApplication *application);
static GtkWindow  *create_window      (ViewerApplication *application);
static GListModel *create_window_list (GtkApplication *application);
static gboolean    get_property       (ViewerApplication *application, guint property_id, GValue *value);
static void        init_accels        (GtkApplication *application);
static void        init_actions       (GApplication *application);
static void        init_settings      (ViewerApplication *application);
static void        open               (ViewerWindow *window, GFile *file);
static gboolean    set_property       (ViewerApplication *application, guint property_id, const GValue *value);
static gboolean    set_settings       (ViewerApplication *application, GSettings *settings);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
#define N_ACCEL_ENTRIES  G_N_ELEMENTS (ACCEL_ENTRIES)
#define N_ACTION_ENTRIES G_N_ELEMENTS (ACTION_ENTRIES)
#define PSPEC_SETTINGS   g_param_spec_object ("settings", "Settings", "Application settings.", G_TYPE_SETTINGS, G_PARAM_READWRITE)

static const char *ACCELS_CLOSE        [] = { "<Ctrl>w", NULL };
static const char *ACCELS_FULLSCREEN   [] = { "F11", NULL };
static const char *ACCELS_OPEN         [] = { "<Ctrl>o", NULL };
static const char *ACCELS_PRINT        [] = { "<Ctrl>p", NULL };
static const char *ACCELS_QUIT         [] = { "<Ctrl>q", NULL };
static const char *ACCELS_RESTORE_ZOOM [] = { "<Ctrl>0", NULL };
static const char *ACCELS_UNFULLSCREEN [] = { "Escape", NULL };
static const char *ACCELS_ZOOM_IN      [] = { "<Ctrl>plus", "<Ctrl>semicolon", NULL };
static const char *ACCELS_ZOOM_OUT     [] = { "<Ctrl>minus", NULL };

static const ViewerApplicationAccelEntry
ACCEL_ENTRIES [] =
{
	{ "window.close",          ACCELS_CLOSE        },
	{ "win.fullscreen",        ACCELS_FULLSCREEN   },
	{ "win.open",              ACCELS_OPEN         },
	{ "win.print",             ACCELS_PRINT        },
	{ "app.quit",              ACCELS_QUIT         },
	{ "win.restore-zoom",      ACCELS_RESTORE_ZOOM },
	{ "win.unfullscreen",      ACCELS_UNFULLSCREEN },
	{ "win.zoom-in",           ACCELS_ZOOM_IN      },
	{ "win.zoom-out",          ACCELS_ZOOM_OUT     },
};

static const ViewerApplicationActionEntry
ACTION_ENTRIES [] =
{
	{ ACTION_QUIT, activate_quit },
};

GSettings *
viewer_application_get_settings (ViewerApplication *application)
{
	g_return_val_if_fail (VIEWER_IS_APPLICATION (application), NULL);
	return application->settings;
}

ViewerApplication *
viewer_application_new (const gchar *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		PROPERTY_APPLICATION_ID, application_id,
		PROPERTY_FLAGS, APPLICATION_FLAGS,
		NULL);
}

void
viewer_application_set_settings (ViewerApplication *application, GSettings *settings)
{
	g_return_if_fail (VIEWER_IS_APPLICATION (application));

	if (set_settings (application, settings))
	{
		g_object_notify (G_OBJECT (application), PROPERTY_SETTINGS);
	}
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
	g_object_class_install_property (G_OBJECT_CLASS (application_class), PROPID_SETTINGS, PSPEC_SETTINGS);
}

static void
viewer_application_dispose (GObject *object)
{
	g_clear_object (&VIEWER_APPLICATION (object)->settings);
	G_OBJECT_CLASS (viewer_application_parent_class)->dispose (object);
}

static void
viewer_application_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	if (get_property (VIEWER_APPLICATION (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_application_init (ViewerApplication *application)
{
}

static void
viewer_application_open (GApplication *application, GFile **files, int n_files, const char *hint)
{
	GtkWindow *window;
	int n;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));
	n = 0;

	if (VIEWER_IS_WINDOW (window) && (viewer_window_get_file (VIEWER_WINDOW (window)) == NULL))
	{
		open (VIEWER_WINDOW (window), *files);
		files++;
		n++;
	}
	while (n < n_files)
	{
		window = create_window (VIEWER_APPLICATION (application));
		open (VIEWER_WINDOW (window), *files);
		files++;
		n++;
	}
}

static void
viewer_application_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	if (set_property (VIEWER_APPLICATION (object), property_id, value))
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
viewer_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (viewer_application_parent_class)->startup (application);
	init_actions (application);
	init_accels (GTK_APPLICATION (application));
	init_settings (VIEWER_APPLICATION (application));
}

static void
activate_quit (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GListModel *list;
	GtkWindow *window;
	guint n, n_items;
	list = create_window_list (GTK_APPLICATION (user_data));
	n_items = g_list_model_get_n_items (list);

	for (n = 0; n < n_items; n++)
	{
		window = g_list_model_get_item (list, n);
		gtk_window_close (window);
		g_object_unref (window);
	}

	g_object_unref (list);
}

static GSettings *
create_settings (GApplication *application)
{
	const char *schema_id;
	schema_id = g_application_get_application_id (application);
	return g_settings_new (schema_id);
}

static GtkWindow *
create_window (ViewerApplication *application)
{
	GtkWidget *window;
	window = viewer_window_new (GTK_APPLICATION (application));
	viewer_window_set_settings (VIEWER_WINDOW (window), application->settings);
	return GTK_WINDOW (window);
}

static GListModel *
create_window_list (GtkApplication *application)
{
	GListStore *store;
	GList *list;
	GtkWindow *window;
	store = g_list_store_new (GTK_TYPE_WINDOW);
	list = gtk_application_get_windows (GTK_APPLICATION (application));

	while (list != NULL)
	{
		window = list->data;

		if (VIEWER_IS_WINDOW (window))
		{
			g_list_store_append (store, window);
		}

		list = list->next;
	}

	return G_LIST_MODEL (store);
}

static gboolean
get_property (ViewerApplication *application, guint property_id, GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_SETTINGS:
		g_value_set_object (value, application->settings);
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static void
init_accels (GtkApplication *application)
{
	const ViewerApplicationAccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < N_ACCEL_ENTRIES; n++)
	{
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

static void
init_actions (GApplication *application)
{
	const ViewerApplicationActionEntry *src;
	GActionEntry *dest;
	GActionEntry entries [N_ACTION_ENTRIES];
	int n;
	memset (entries, 0, sizeof entries);
	src = ACTION_ENTRIES;
	dest = entries;

	for (n = 0; n < N_ACTION_ENTRIES; n++)
	{
		memcpy (dest++, src++, sizeof (ViewerApplicationActionEntry));
	}

	g_action_map_add_action_entries (G_ACTION_MAP (application), entries, N_ACTION_ENTRIES, application);
}

static void
init_settings (ViewerApplication *application)
{
	GSettings *settings;
	settings = create_settings (G_APPLICATION (application));
	viewer_application_set_settings (application, settings);
	g_object_unref (settings);
}

static void
open (ViewerWindow *window, GFile *file)
{
	viewer_window_set_file (window, file);
	gtk_window_present (GTK_WINDOW (window));
	viewer_window_load (window, file);
}

static gboolean
set_property (ViewerApplication *application, guint property_id, const GValue *value)
{
	gboolean invalid;
	invalid = FALSE;

	switch (property_id)
	{
	case PROPID_SETTINGS:
		set_settings (application, g_value_get_object (value));
		break;
	default:
		invalid = TRUE;
		break;
	}

	return invalid;
}

static gboolean
set_settings (ViewerApplication *application, GSettings *settings)
{
	gboolean changed;
	changed = application->settings != settings;

	if (changed)
	{
		if (application->settings != NULL)
		{
			g_object_unref (application->settings);
		}
		if (settings != NULL)
		{
			application->settings = g_object_ref (settings);
		}
		else
		{
			application->settings = NULL;
		}
	}

	return changed;
}
