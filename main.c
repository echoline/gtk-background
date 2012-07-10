#define WEATHER 0

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include "sensor.h"
#include "tray.h"
#include "launcher.h"
#include "clock.h"
#include "meter.h"
#include "info.h"
#if WEATHER
	#include "bubble.h"
	#include "weather/weather-widget.h"
#endif
#include "akamaru.h"
#include "drag.h"

// to keep track of whether the sensor has sent at least one update
gboolean sensor_on = FALSE;

static void
gtk_drag_update (AkamaruObject *object, void *data)
{
	GtkDrag *drag = (GtkDrag*)object->data;

	gtk_drag_animate (drag);
}

static void
walls (AkamaruObject *object, void *data)
{
	AkamaruModel *model = (AkamaruModel*)data;
	double x = object->position.x, y = object->position.y;
	double dx = 0, dy = 0;
	gint maxx, maxy;
	gboolean bump = FALSE;

	// walls
	maxy = gdk_screen_height () - gtk_widget_get_allocated_height (
					GTK_WIDGET (object->data)) - 25;
	maxx = gdk_screen_width () - gtk_widget_get_allocated_width (
					GTK_WIDGET (object->data)) - 25;

	if (y < 50)
	{
		dy = 50 - y;
		bump = TRUE;
	}
	else if (y > maxy)
	{
		dy = maxy - y;
		bump = TRUE;
	}

	if (x < 25)
	{
		dx = 25 - x;
		bump = TRUE;
	}
	else if (x > maxx)
	{
		dx = maxx - x;
		bump = TRUE;
	}

	if (bump)
	{
		y += dy / 2;
		x += dx / 2;
	}

	object->position.x = x;
	object->position.y = y;
}

static gboolean
animate (gpointer data)
{
	AkamaruModel *model = (AkamaruModel*)data;

	akamaru_model_step (model, 1);

	akamaru_model_for_each_object (model, walls, model);
	akamaru_model_for_each_object (model, gtk_drag_update, NULL);

	return TRUE;
}

static void
animate_begin (Sensor *sensor, gpointer model)
{

	g_signal_handler_disconnect (sensor,
				g_signal_handler_find (sensor,
					G_SIGNAL_MATCH_FUNC,
					0, 0, NULL,
					G_CALLBACK (animate_begin),
					0));
}

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
	AkamaruModel *model;

	gtk_init (&argc, &argv);

	sensor = (Sensor*)sensor_new ();

	model = g_new0 (AkamaruModel, 1);
	akamaru_model_init (model);
	model->constrain_iterations = 1;

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

	drag = gtk_drag_new (sensor, model);
	widget = cpu_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 180, 60, TRUE);

	drag = gtk_drag_new (sensor, model);
	widget = mem_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 180, 210, TRUE);

	drag = gtk_drag_new (sensor, model);
	widget = gtk_clock_new ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 180, height - 180, TRUE);

#if WEATHER
	drag = gtk_drag_new (sensor, model);
	widget = gtk_weather_new ();
	bubble = gtk_bubble_new ();
	gtk_container_add (GTK_CONTAINER (bubble), widget);
	gtk_widget_set_size_request (bubble, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), bubble);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 380, height - 180, TRUE);
#endif

	gtk_widget_show_all (root);

//	g_signal_connect (sensor, "updated", G_CALLBACK (animate_begin), model);
	g_timeout_add (15, animate, model);

	gtk_main ();
}
