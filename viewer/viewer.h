/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "paint/paint.h"
#define VIEWER_APPLICATION_ID       "com.github.mi19a009.PictureViewer"
#define VIEWER_CANVAS_DEFAULT_ZOOM  100
#define VIEWER_CANVAS_MAXIMUM_ZOOM  1000
#define VIEWER_CANVAS_MINIMUM_ZOOM  10
#define VIEWER_VIEW_DIALOG_TEMPLATE "/com/github/mi19a009/PictureViewer/view.ui"
#define VIEWER_TYPE_APPLICATION     (viewer_application_get_type ())
#define VIEWER_TYPE_CANVAS          (viewer_canvas_get_type      ())
#define VIEWER_TYPE_VIEW_DIALOG     (viewer_view_dialog_get_type ())
#define VIEWER_TYPE_WINDOW          (viewer_window_get_type      ())

G_DECLARE_FINAL_TYPE (ViewerApplication, viewer_application, VIEWER, APPLICATION, GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerCanvas,      viewer_canvas,      VIEWER, CANVAS,      GtkDrawingArea);
G_DECLARE_FINAL_TYPE (ViewerViewDialog,  viewer_view_dialog, VIEWER, VIEW_DIALOG, GtkWindow);
G_DECLARE_FINAL_TYPE (ViewerWindow,      viewer_window,      VIEWER, WINDOW,      GtkApplicationWindow);

/* Viewer About Dialog */
GtkWidget *viewer_about_dialog_new (void);

/* Viewer Application */
GSettings         *viewer_application_get_settings (ViewerApplication *application);
ViewerApplication *viewer_application_new          (const gchar *application_id);
void               viewer_application_set_settings (ViewerApplication *application, GSettings *settings);

/* Viewer Canvas */
gint       viewer_canvas_get_zoom  (ViewerCanvas *canvas);
void       viewer_canvas_load      (ViewerCanvas *canvas, GFile *file, GError **error);
GtkWidget *viewer_canvas_new       (void);
void       viewer_canvas_set_zoom  (ViewerCanvas *canvas, gint zoom);
GdkPixbuf *viewer_canvas_to_pixbuf (ViewerCanvas *canvas);

/* Viewer View Dialog */
gint       viewer_view_dialog_get_zoom (ViewerViewDialog *dialog);
GtkWidget *viewer_view_dialog_new      (void);
void       viewer_view_dialog_set_zoom (ViewerViewDialog *dialog, gint zoom);

/* Viewer Window */
GFile     *viewer_window_get_file     (ViewerWindow *window);
GSettings *viewer_window_get_settings (ViewerWindow *window);
void       viewer_window_load         (ViewerWindow *window, GFile *file);
GtkWidget *viewer_window_new          (GtkApplication *application);
void       viewer_window_set_file     (ViewerWindow *window, GFile *file);
void       viewer_window_set_settings (ViewerWindow *window, GSettings *settings);
