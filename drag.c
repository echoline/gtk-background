#include "drag.h"

typedef struct _GtkDragPrivate GtkDragPrivate;

struct _GtkDragPrivate
{
	gboolean clicked;
	gint x;
	gint y;
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
		gtk_fixed_move (parent, GTK_WIDGET (drag),
			(int)(event->x_root - priv->x),
			(int)(event->y_root - priv->y));
	}
}

static void
gtk_drag_clicked (GtkDrag *drag, GdkEventButton *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	GtkFixed *parent = GTK_FIXED (gtk_widget_get_parent (
					GTK_WIDGET (drag)));

	priv->clicked = TRUE;
	priv->x = event->x;
	priv->y = event->y;

	gtk_container_remove (GTK_CONTAINER (parent), GTK_WIDGET (drag));
	gtk_fixed_put (parent, GTK_WIDGET (drag), event->x_root - event->x,
						event->y_root - event->y);
}

static void
gtk_drag_unclicked (GtkDrag *drag, GdkEvent *event, gpointer user_data)
{
	GtkDragPrivate *priv = GTK_DRAG_GET_PRIVATE (drag);
	priv->clicked = FALSE;
}

static void
gtk_drag_class_init (GtkDragClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

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
gtk_drag_new (void)
{
	return g_object_new (GTK_TYPE_DRAG, NULL);
}
