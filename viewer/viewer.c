/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define LOCALE ""

int
main (int argc, char **argv)
{
	ViewerApplication *application;
	int status;
	setlocale (LC_ALL, LOCALE);
	paint_bind_textdomain ();
	application = viewer_application_new (VIEWER_APPLICATION_ID);
	status = g_application_run (G_APPLICATION (application), argc, argv);
	g_object_unref (application);
	return status;
}
