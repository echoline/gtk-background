#include "launcher.h"

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
	}
	g_free(conf);
}

static gboolean
list_check (GtkTreeModel *store, const gchar *text)
{
	gchar *tmp;
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_first(store, &iter)) do {
		gtk_tree_model_get (store, &iter, 0, &tmp, -1);
		if (!g_ascii_strcasecmp(tmp, text)) {
			g_free (tmp);
			return TRUE;
		}
		g_free (tmp);
	} while (gtk_tree_model_iter_next (store, &iter));

	return FALSE;
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

	if (!list_check (GTK_TREE_MODEL (store), text))
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, text, -1);
		list_save (GTK_TREE_MODEL (store));
	}

	if (!g_shell_parse_argv(text, &argc, &argv, NULL) ||
		!g_spawn_async_with_pipes(NULL, argv, NULL,
					G_SPAWN_SEARCH_PATH |
					G_SPAWN_STDERR_TO_DEV_NULL,
					NULL, NULL, NULL, NULL,
					NULL, NULL, NULL))
	{
		command = g_strconcat ("xdg-open ", text, NULL);

		if (g_shell_parse_argv(command, &argc, &argv, NULL))
			g_spawn_async_with_pipes(NULL, argv, NULL,
					G_SPAWN_SEARCH_PATH |
					G_SPAWN_STDERR_TO_DEV_NULL,
					NULL, NULL, NULL, NULL,
					NULL, NULL, NULL);

		g_free(command);
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
launcher_new ()
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
