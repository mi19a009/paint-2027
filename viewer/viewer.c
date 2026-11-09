/* Copyright (C) 2026 Taichi Murakami. */
#include "header.h"
#include "viewer.h"
#define CODESET "UTF-8"
#define DOMAIN  "paint"
#define FORMAT  "%d: %s\n"
#define LINK    "/proc/self/exe"
#define LOCALE  ""

static void  bindtext (void);
static char *getdir   (GError **error);

int
main (int argc, char **argv)
{
	ViewerApplication *application;
	int status;
	setlocale (LC_ALL, LOCALE);
	bindtext ();
	application = viewer_application_new (VIEWER_APPLICATION_ID);
	status = g_application_run (G_APPLICATION (application), argc, argv);
	g_object_unref (application);
	return status;
}

static void
bindtext (void)
{
	GError *error;
	char *dir;
	error = NULL;
	dir = getdir (&error);

	if (dir != NULL)
	{
		bindtextdomain (DOMAIN, dir);
		bind_textdomain_codeset (DOMAIN, CODESET);
		textdomain (DOMAIN);
		g_free (dir);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, FORMAT, error->code, error->message);
		g_error_free (error);
	}
}

static char *
getdir (GError **error)
{
	char *path, *dir;
	path = g_file_read_link (LINK, error);

	if (path != NULL)
	{
		dir = g_path_get_dirname (path);
		g_free (path);
	}
	else
	{
		dir = NULL;
	}

	return dir;
}
