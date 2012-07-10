#include <gtk/gtk.h>
#include <math.h>
#include "sensor.h"
#include "drag.h"

typedef struct _GtkDragPrivate GtkDragPrivate;

struct _GtkDragPrivate
{
	gboolean clicked;
	gdouble x;
	gdouble y;
	gdouble vx;
	gdouble vy;
	gdouble ox;
	gdouble oy;
	GIOChannel *chan;
	Sensor *sensor;
};

#define GTK_DRAG_GET_PRIVATE(obj)      (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_DRAG, GtkDragPrivate))

G_DEFINE_TYPE (GtkDrag, gtk_drag, GTK_TYPE_EVENT_BOX);

static void
gtk_drag_move (GtkDrag *drag, GdkEventMotion *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent
				(GTK_WIDGET (drag)));

	if (priv->clicked)
	{
		priv->x = event->x_root - priv->ox;
		priv->y = event->y_root - priv->oy;

		gtk_fixed_move (parent, GTK_WIDGET (drag),
				(int)(priv->x), (int)(priv->y));
	}
}

static void
gtk_drag_clicked (GtkDrag *drag, GdkEventButton *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent (
					GTK_WIDGET (drag)));
	GList *widgets;

	priv->clicked = TRUE;
	priv->ox = event->x;
	priv->oy = event->y;
	priv->x = event->x_root - event->x;
	priv->y = event->y_root - event->y;
	priv->vx = priv->vy = 0;

	// move and focus
	gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (drag));
	gtk_fixed_put (parent, GTK_WIDGET (drag), priv->x, priv->y);
}

static void
gtk_drag_sensor_updated (Sensor *sensor, GtkDrag *drag)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (!priv->clicked) {
		GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent (
						GTK_WIDGET (drag)));
		gdouble pitch = sensor_get_pitch (sensor);
		gdouble roll = sensor_get_roll (sensor);

		if (pitch < 0)
			pitch += 360;
		if (roll < 0)
			roll += 360;

		priv->vy = -sin(pitch * (G_PI / 180.0));
		priv->vx = sin(roll * (G_PI / 180.0));
	}
}

static void
gtk_drag_unclicked (GtkDrag *drag, GdkEvent *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	priv->clicked = FALSE;
}

static gboolean
gtk_drag_animate_timeout (gpointer data)
{
	GtkDrag *drag = GTK_DRAG (data);
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (priv->clicked)
		return TRUE;

	gtk_drag_set_coords (drag, priv->x + priv->vx,
				priv->y + priv->vy);

	return TRUE;
}

static void
gtk_drag_realize (GtkWidget *widget)
{
	GtkDrag *drag = GTK_DRAG (widget);
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent
				(GTK_WIDGET (drag)));

	if (GTK_WIDGET_CLASS (gtk_drag_parent_class)->realize)
		GTK_WIDGET_CLASS (gtk_drag_parent_class)->realize (widget);

	if (priv->sensor) {
		g_signal_connect (priv->sensor, "updated",
			G_CALLBACK (gtk_drag_sensor_updated), drag);
		g_timeout_add (1, gtk_drag_animate_timeout, drag);
	} 
}

static void
gtk_drag_class_init (GtkDragClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->realize = gtk_drag_realize;

	g_type_class_add_private (class, sizeof (GtkDragPrivate));
}

static void
gtk_drag_init (GtkDrag *drag)
{
	gtk_widget_set_has_window (GTK_WIDGET (drag), FALSE);

	g_signal_connect (drag, "button_press_event",
			G_CALLBACK (gtk_drag_clicked), NULL);
	g_signal_connect (drag, "button_release_event",
			G_CALLBACK (gtk_drag_unclicked), NULL);
	g_signal_connect (drag, "motion_notify_event",
			G_CALLBACK (gtk_drag_move), NULL);
}

GtkWidget*
gtk_drag_new (Sensor *sensor)
{
	GtkDrag *ret = g_object_new (GTK_TYPE_DRAG, NULL);
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (ret);

	priv->sensor = sensor;

	return GTK_WIDGET (ret);
}

void
gtk_drag_set_coords (GtkDrag *drag, gdouble x, gdouble y)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent (
					GTK_WIDGET (drag)));

	gint maxy = gdk_screen_height () - gtk_widget_get_allocated_height (
					GTK_WIDGET (drag));
	gint maxx = gdk_screen_width () - gtk_widget_get_allocated_width (
					GTK_WIDGET (drag));

	priv->y = y;
	priv->x = x;

	if (priv->y < 0)
		priv->y = 0;
	else if (priv->y > maxy)
		priv->y = maxy;

	if (priv->x < 0)
		priv->x = 0;
	else if (priv->x > maxx)
		priv->x = maxx;

	gtk_fixed_move (parent, GTK_WIDGET (drag), priv->x, priv->y);

}

void
gtk_drag_get_coords (GtkDrag *drag, gdouble *x, gdouble *y)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (x)
		*x = priv->x;
	if (y)
		*y = priv->y;
}
