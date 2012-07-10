#include <gtk/gtk.h>
#include <math.h>
#include "sensor.h"
#include "akamaru.h"
#include "drag.h"

typedef struct _GtkDragPrivate GtkDragPrivate;

struct _GtkDragPrivate
{
	gboolean clicked;
	gdouble ox;
	gdouble oy;
	Sensor *sensor;
	AkamaruModel *model;
	AkamaruObject *object;
};

#define GTK_DRAG_GET_PRIVATE(obj)      (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_DRAG, GtkDragPrivate))

G_DEFINE_TYPE (GtkDrag, gtk_drag, GTK_TYPE_EVENT_BOX);

static void
gtk_drag_move (GtkDrag *drag, GdkEventMotion *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (priv->clicked)
	{
		gtk_drag_set_coords (drag, event->x_root - priv->ox,
					event->y_root - priv->oy, TRUE);
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
	priv->object->position.x = event->x_root - event->x;
	priv->object->position.y = event->y_root - event->y;
	priv->object->gravity.x = priv->object->gravity.y = 0;

	// move and focus
	gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (drag));
	gtk_fixed_put (parent, GTK_WIDGET (drag), priv->object->position.x,
			priv->object->position.y);
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

		priv->object->gravity.x = sin(roll * (G_PI / 180.0));
		priv->object->gravity.y = -sin(pitch * (G_PI / 180.0));
	}
}

static void
gtk_drag_unclicked (GtkDrag *drag, GdkEvent *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	priv->clicked = FALSE;
}

void
gtk_drag_animate (GtkDrag *drag)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (!priv->clicked)
	{
		gtk_drag_set_coords (drag, priv->object->position.x,
					priv->object->position.y, FALSE);
	}
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

static void
gtk_drag_add_spacer (AkamaruObject *a, void *data)
{
	AkamaruObject *b = (AkamaruObject*)data;

	if (a == b)
		return;

	AkamaruModel *model = GTK_DRAG_GET_PRIVATE (GTK_DRAG (a->data))->model;

	akamaru_model_add_spacer (model, a, b, 150);
}

static void
gtk_drag_add_spacers (AkamaruObject *object, void *data)
{
	AkamaruModel *model = (AkamaruModel*)data;

	akamaru_model_for_each_object (model, gtk_drag_add_spacer, object);
}

GtkWidget*
gtk_drag_new (Sensor *sensor, AkamaruModel *model)
{
	GtkDrag *ret = g_object_new (GTK_TYPE_DRAG, NULL);
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (ret);

	priv->sensor = sensor;
	priv->model = model;
	priv->object = akamaru_model_add_object (priv->model, 0, 0, 1, ret); 
	priv->object->gravity.x = priv->object->gravity.y = 0;

	akamaru_model_for_each_object (priv->model, gtk_drag_add_spacers,
					priv->model);

	return GTK_WIDGET (ret);
}

void
gtk_drag_set_coords (GtkDrag *drag, gdouble x, gdouble y, gboolean resetvel)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent (
					GTK_WIDGET (drag)));

	priv->object->position.x = x;
	priv->object->position.y = y;

	if (resetvel)
	{
		priv->object->previous_position.x = x;
		priv->object->previous_position.y = y;
	}

	gtk_fixed_move (parent, GTK_WIDGET (drag), x, y);
}

void
gtk_drag_get_coords (GtkDrag *drag, gdouble *x, gdouble *y)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);

	if (x)
		*x = priv->object->position.x;
	if (y)
		*y = priv->object->position.y;
}
