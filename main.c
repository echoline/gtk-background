#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include "sensor.h"
#include "tray.h"
#include "launcher.h"
#include "clock.h"
#include "meter.h"
#include "info.h"
#include "bubble.h"
#include "weather/weather-widget.h"
#include "drag.h"

int
main (int argc, char **argv)
{
	GtkWidget *root;
	GtkWidget *fixed;
	GtkWidget *box;
	GtkWidget *widget;
	GtkWidget *bubble;
	GtkWidget *drag;
	Sensor *sensor;
	gint width;
	gint height;

	gtk_init (&argc, &argv);

	sensor = (Sensor*)sensor_new ();

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

//	bubble = gtk_bubble_new ();
//	gtk_widget_set_size_request (bubble, width - 50, 150);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, TRUE);
//	gtk_container_add (GTK_CONTAINER (bubble), box);
//	gtk_fixed_put (GTK_FIXED (fixed), bubble, 25, 25);
	gtk_fixed_put (GTK_FIXED (fixed), box, 25, 25);

	widget = launcher_init ();
	gtk_widget_set_size_request (widget, width / 3, -1);
	gtk_container_add (GTK_CONTAINER (box), widget);

	widget = gtk_tray_new ();
	gtk_container_add (GTK_CONTAINER (box), widget);

	drag = gtk_drag_new (sensor);
	widget = cpu_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, 25);

	drag = gtk_drag_new (sensor);
	widget = mem_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, 175);

	drag = gtk_drag_new (sensor);
	widget = gtk_clock_new ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, height - 175);

	drag = gtk_drag_new (sensor);
	widget = gtk_weather_new ();
	bubble = gtk_bubble_new ();
	gtk_container_add (GTK_CONTAINER (bubble), widget);
	gtk_widget_set_size_request (bubble, 200, 150);
	gtk_container_add (GTK_CONTAINER (drag), bubble);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 375, height - 175);

	gtk_widget_show_all (root);

	gtk_main ();
}
