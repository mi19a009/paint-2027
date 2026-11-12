/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define ABOUT_COPYRIGHT      "Copyright © 2026 Taichi Murakami."
#define ABOUT_LICENSE_TYPE   GTK_LICENSE_MIT_X11
#define ABOUT_LOGO_ICON_NAME "viewer"
#define ABOUT_PROGRAM_NAME   gettext ("Picture Viewer")
#define ABOUT_WEBSITE        "https://github.com/mi19a009"

static const char *
ABOUT_AUTHORS [] = { "Taichi Murakami", NULL };

GtkWidget *
viewer_about_dialog_new (void)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), ABOUT_AUTHORS);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), ABOUT_COPYRIGHT);
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (dialog), ABOUT_LICENSE_TYPE);
	gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (dialog), ABOUT_LOGO_ICON_NAME);
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), ABOUT_PROGRAM_NAME);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), ABOUT_WEBSITE);
	return dialog;
}
