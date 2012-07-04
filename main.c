#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include "clock.h"
#include "launcher.h"
#include "tray.h"
#include "meter.h"
#include "info.h"

int
main (int argc, char **argv)
{
	GtkWidget *root;
	GtkWidget *fixed;
	GtkWidget *clock;
	GtkWidget *tray;
	GtkWidget *cpumeter;
	GtkWidget *memmeter;
	GtkWidget *launcher;
	gint width;
	gint height;

	gtk_init (&argc, &argv);

	width = gdk_screen_width ();
	height = gdk_screen_height ();

	root = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	g_signal_connect (root, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);

	gtk_window_set_type_hint (GTK_WINDOW (root),
				GDK_WINDOW_TYPE_HINT_DESKTOP);

	gtk_widget_set_size_request (root, width, height);

	fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (root), fixed);

	launcher = launcher_new ();
	gtk_widget_set_size_request (launcher, 480, -1);
	gtk_fixed_put (GTK_FIXED (fixed), launcher, 25, 25);

	clock = gtk_clock_new ();
	gtk_widget_set_size_request (clock, 150, 150);
	gtk_fixed_put (GTK_FIXED (fixed), clock, width - 175, height - 175);

	cpumeter = cpu_init ();
	gtk_widget_set_size_request (cpumeter, 150, 150);
	gtk_fixed_put (GTK_FIXED (fixed), cpumeter, width - 175, 25);

	memmeter = mem_init ();
	gtk_widget_set_size_request (memmeter, 150, 150);
	gtk_fixed_put (GTK_FIXED (fixed), memmeter, width - 175, 200);

	tray = gtk_tray_new ();
	gtk_fixed_put (GTK_FIXED (fixed), tray, 25, height - 100);

	gtk_widget_show_all (root);

	gtk_main ();
}
