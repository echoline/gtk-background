#include "launcher.h"
#include "sensor.h"
//#include "drag.h"
#include "bubble.h"
#include <vte/vte.h>

static void
list_save(GtkTreeModel *store)
{
	gchar *conf = g_strconcat(g_get_user_config_dir(), "/gtk-background.conf",
				NULL);
	gchar *text;
	gchar *contents = g_strdup("");
	gchar *tmp;
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter_first(store, &iter)) do {
		gtk_tree_model_get(store, &iter, 0, &text, -1);
		tmp = g_strconcat(contents, text, "\n", NULL);
		g_free(text);
		g_free(contents);
		contents = tmp;
	} while (gtk_tree_model_iter_next(store, &iter));

	if (!g_file_set_contents(conf, contents, -1, NULL)) {
		fprintf(stderr, "file write error\n");
	}
	g_free(contents);
	g_free(conf);
}

static void
list_load(GtkListStore *store)
{
	gchar *conf = g_strconcat(g_get_user_config_dir(), "/gtk-background.conf",
				NULL);
	gchar *contents;
	gchar **lines;
	guint i = 0;
	GtkTreeIter iter;
	if (g_file_get_contents(conf, &contents, NULL, NULL)) {
		lines = g_strsplit(contents, "\n", 0);
		while (lines[i] != NULL) {
			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, 0, lines[i], -1);
			i++;
		}
		g_strfreev(lines);
		g_free(contents);
	}
	g_free(conf);
}

static gboolean
list_check (GtkTreeModel *store, const gchar *text, GtkTreeIter *iter)
{
	gchar *tmp;

	if (gtk_tree_model_get_iter_first(store, iter)) do {
		gtk_tree_model_get (store, iter, 0, &tmp, -1);
		if (!g_ascii_strcasecmp(tmp, text)) {
			g_free (tmp);
			return TRUE;
		}
		g_free (tmp);
	} while (gtk_tree_model_iter_next (store, iter));

	return FALSE;
}

static gboolean
watch_cb (GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
	VteTerminal *term = VTE_TERMINAL (user_data);
	gint outfd = g_io_channel_unix_get_fd (channel);
	gint ptyfd = vte_pty_get_fd (vte_terminal_get_pty_object (term));
	char c;

	while (read (outfd, &c, 1) == 1)
		write (ptyfd, &c, 1);

	if (condition & G_IO_HUP)
		return FALSE;

	return TRUE;
}

static void
activate_cb (GtkEntry *entry, gpointer data)
{
	gint argc;
	gchar **argv;
	const gchar *text = gtk_entry_get_text (entry);
	gchar *command;
	GtkListStore *store = GTK_LIST_STORE (data);
	GtkTreeIter iter;
	gint input, output;
//	GIOChannel *channel;
//	GSource *source;
//	GtkWidget *drag;
//	GtkWidget *bubble;
//	GtkWidget *term;
//	GtkWidget *bg;
//	GdkRGBA fgcol = {0, 0, 0, 1};
//	GdkRGBA bgcol = {1, 1, 1, 1};
//	VtePty *pty;

	if (list_check (GTK_TREE_MODEL (store), text, &iter))
	{
		gtk_list_store_remove (store, &iter);
	}

	gtk_list_store_prepend (store, &iter);
	gtk_list_store_set (store, &iter, 0, text, -1);
	list_save (GTK_TREE_MODEL (store));

	if (!g_shell_parse_argv(text, &argc, &argv, NULL) ||
		!g_spawn_async_with_pipes(NULL, argv, NULL,
					G_SPAWN_SEARCH_PATH |
					G_SPAWN_STDERR_TO_DEV_NULL,
					NULL, NULL, NULL,
					&input, &output, NULL, NULL))
	{
		command = g_strconcat ("xdg-open ", text, NULL);

		if (g_shell_parse_argv(command, &argc, &argv, NULL))
			g_spawn_async_with_pipes(NULL, argv, NULL,
					G_SPAWN_SEARCH_PATH |
					G_SPAWN_STDERR_TO_DEV_NULL,
					NULL, NULL, NULL, NULL,
					NULL, NULL, NULL);

		g_free(command);
	} else if (output) {
/*		term = vte_terminal_new ();
		pty = vte_pty_new (VTE_PTY_DEFAULT, NULL);
		vte_terminal_set_pty_object (VTE_TERMINAL (term), pty);
		gtk_widget_set_has_window (term, FALSE);

		channel = g_io_channel_unix_new (output);

		drag = gtk_drag_new ();
		bubble = gtk_bubble_new ();
		bg = gtk_widget_get_toplevel (GTK_WIDGET (entry));
		bg = gtk_bin_get_child (GTK_BIN (bg));
		vte_terminal_set_colors_rgba (VTE_TERMINAL (term),
						&fgcol, &bgcol, NULL, 0);
		gtk_container_add (GTK_CONTAINER (drag), bubble);
		gtk_container_add (GTK_CONTAINER (bubble), term);
		gtk_fixed_put (GTK_FIXED (bg), drag, 25, 100);
		gtk_widget_set_size_request (bubble, 640, 320);
		gtk_widget_show_all (drag);

		if (!g_io_add_watch (channel, G_IO_OUT|G_IO_HUP, watch_cb,
					term)) {
			g_error ("g_io_add_watch");
		}*/
	}

	gtk_entry_set_text (entry, "");
}

static void
icon_press_cb (GtkEntry *entry, gint position, GdkEventButton *event,
		gpointer data)
{
	if (position == GTK_ENTRY_ICON_SECONDARY) {
		activate_cb (entry, data);
	}
}

static void
text_changed_cb (GtkEntry *entry, GParamSpec *pspec, gpointer data)
{
	gboolean has_text;

	has_text = gtk_entry_get_text_length (entry) > 0;
	gtk_entry_set_icon_sensitive (entry, GTK_ENTRY_ICON_SECONDARY,
					has_text);
}

GtkWidget*
launcher_init ()
{
	GtkListStore *store = gtk_list_store_new (1, G_TYPE_STRING);
	GtkEntryCompletion *completion;
	GtkWidget *entry = gtk_entry_new ();

	gtk_entry_set_icon_from_stock (GTK_ENTRY (entry),
					GTK_ENTRY_ICON_SECONDARY,
					GTK_STOCK_OK);
	g_signal_connect (entry, "icon-press", G_CALLBACK (icon_press_cb),
			store);
	g_signal_connect (entry, "notify::text", G_CALLBACK (text_changed_cb), 
			NULL);
	g_signal_connect (entry, "activate", G_CALLBACK (activate_cb), store);
	text_changed_cb (GTK_ENTRY (entry), NULL, NULL);

	completion = gtk_entry_completion_new ();
	gtk_entry_set_completion (GTK_ENTRY (entry), completion);
	g_object_unref (completion);

	list_load (store);
	gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (store));
	g_object_unref (store);

	gtk_entry_completion_set_text_column (completion, 0);

	return entry;
}
