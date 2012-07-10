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
#include "akamaru.h"
#include "drag.h"

static void
gtk_drag_update (AkamaruObject *object, void *data)
{
	GtkDrag *drag = (GtkDrag*)object->data;

	gtk_drag_animate (drag);
}

static void
spacing2 (AkamaruObject *b, void *data)
{
	AkamaruObject *a = (AkamaruObject*)data;
	double x, y, dx, dy, distance, fraction;

	if (a == b)
		return;

	x = a->position.x;
	y = a->position.y;
	dx = b->position.x - x;
	dy = b->position.y - y;

	distance = sqrt (dx*dx + dy*dy);
	if (distance > 150)
		return;

	fraction = (distance - 150) / distance / 2;
	a->position.x = x + dx * fraction;
	a->position.y = y + dy * fraction;
	b->position.x = x + dx * (1 - fraction);
	b->position.y = y + dy * (1 - fraction);
}

static void
spacing (AkamaruObject *object, void *data)
{
	AkamaruModel *model = (AkamaruModel*)data;
	double x = object->position.x, y = object->position.y;
	double dx = 0, dy = 0;
	gint maxx, maxy;
	gboolean bump = FALSE;

	// other objects
	akamaru_model_for_each_object (model, spacing2, object);

	// walls
	maxy = gdk_screen_height () - gtk_widget_get_allocated_height (
					GTK_WIDGET (object->data));
	maxx = gdk_screen_width () - gtk_widget_get_allocated_width (
					GTK_WIDGET (object->data));

	if (y < 0)
	{
		dy = 0 - y;
		bump = TRUE;
	}
	else if (y > maxy)
	{
		dy = maxy - y;
		bump = TRUE;
	}

	if (x < 0)
	{
		dx = 0 - x;
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
	akamaru_model_for_each_object (model, spacing, model);
	akamaru_model_for_each_object (model, gtk_drag_update, NULL);

	return TRUE;
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
	AkamaruModel model;

	gtk_init (&argc, &argv);

	sensor = (Sensor*)sensor_new ();
	akamaru_model_init (&model);
	g_timeout_add (15, animate, &model);

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

	drag = gtk_drag_new (sensor, &model);
	widget = cpu_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, 25);

	drag = gtk_drag_new (sensor, &model);
	widget = mem_init ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, 175);

	drag = gtk_drag_new (sensor, &model);
	widget = gtk_clock_new ();
	gtk_widget_set_size_request (widget, 150, 150);
	gtk_container_add (GTK_CONTAINER (drag), widget);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 175, height - 175);

/*
	drag = gtk_drag_new (sensor, &model);
	widget = gtk_weather_new ();
	bubble = gtk_bubble_new ();
	gtk_container_add (GTK_CONTAINER (bubble), widget);
	gtk_widget_set_size_request (bubble, 200, 150);
	gtk_container_add (GTK_CONTAINER (drag), bubble);
	gtk_container_add (GTK_CONTAINER (fixed), drag);
	gtk_drag_set_coords (GTK_DRAG (drag), width - 375, height - 175);
*/

	gtk_widget_show_all (root);

	gtk_main ();
}
