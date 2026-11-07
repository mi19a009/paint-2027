/* Copyright (C) 2026 Taichi Murakami. */

#include <gtk/gtk.h>

#define VIEWER_APPLICATION_ID       "com.github.mi19a009.PictureViewer"
#define VIEWER_TYPE_APPLICATION (viewer_application_get_type ())
#define VIEWER_TYPE_WINDOW      (viewer_window_get_type      ())

G_DECLARE_FINAL_TYPE (ViewerApplication, viewer_application, VIEWER, APPLICATION, GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerWindow,      viewer_window,      VIEWER, WINDOW,      GtkApplicationWindow);

/* Viewer Application */
ViewerApplication *viewer_application_new (const gchar *application_id);

/* Viewer Window */
GtkWidget *viewer_window_new (GtkApplication *application);
