#include "tray.h"
#include "bubble.h"

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

typedef struct _GtkTrayPrivate GtkTrayPrivate;

struct _GtkTrayPrivate
{
	Atom manager_atom;
	Atom selection_atom;
	Atom opcode_atom;
	Atom orientation_atom;
};

#define GTK_TRAY_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_TRAY, GtkTrayPrivate))

G_DEFINE_TYPE (GtkTray, gtk_tray, GTK_TYPE_BOX);

static GdkFilterReturn
gtk_tray_handle_xevent (GdkXEvent *xevent, GdkEvent *event, gpointer user_data)
{
	GtkTray *tray = GTK_TRAY (user_data);
	GtkTrayPrivate *priv = GTK_TRAY_GET_PRIVATE (tray);
	XClientMessageEvent *xev;
	GtkWidget *socket;
	GtkWidget *bubble;
	Window window;

	switch (((XEvent*)xevent)->type) {
	case ClientMessage:
		xev = (XClientMessageEvent*)xevent;

		printf ("%d:%s %d:%s\n", xev->message_type,
			gdk_x11_get_xatom_name(xev->message_type),
			xev->data.l[1],
			gdk_x11_get_xatom_name(xev->data.l[1]));

		if (xev->message_type == priv->manager_atom &&
		    xev->data.l[1] == priv->selection_atom)
		{
			return GDK_FILTER_REMOVE;
		}
		else if (xev->message_type == priv->opcode_atom &&
		    xev->data.l[1] == SYSTEM_TRAY_REQUEST_DOCK)
		{
			window = xev->data.l[2];

			bubble = gtk_bubble_new ();
			socket = gtk_socket_new ();
			gtk_widget_set_size_request (bubble, 150, 150);
			gtk_widget_set_size_request (socket, 64, 64);
			gtk_container_add (GTK_CONTAINER (tray), bubble);
			gtk_container_add (GTK_CONTAINER (bubble), socket);
			gtk_widget_show (socket);
			gtk_widget_show (bubble);

			gtk_socket_add_id (GTK_SOCKET (socket), window);

			return GDK_FILTER_REMOVE;
		}
		break;
	default:
		break;
	}

	return GDK_FILTER_CONTINUE;
}

static void
gtk_tray_realize (GtkWidget *widget)
{
	GtkTray *tray = GTK_TRAY (widget);
	GtkTrayPrivate *priv = GTK_TRAY_GET_PRIVATE (tray);
	GdkWindow *window;

	XClientMessageEvent xevent;

	Display *display = XOpenDisplay (NULL);
	int screen_number = DefaultScreen (display);
	Window root_window = RootWindow (display, screen_number);

	gchar *selection_name = g_strdup_printf ("_NET_SYSTEM_TRAY_S%d",
						screen_number);
	priv->selection_atom = XInternAtom (display, selection_name, False);
	g_free (selection_name);
	priv->manager_atom = XInternAtom (display, "MANAGER", False);
	priv->opcode_atom = XInternAtom (display, "_NET_SYSTEM_TRAY_OPCODE",
					False);
	priv->orientation_atom = XInternAtom (display,
					"_NET_SYSTEM_TRAY_ORIENTATION", False);

	if (GTK_WIDGET_CLASS (gtk_tray_parent_class)->realize)
		GTK_WIDGET_CLASS (gtk_tray_parent_class)->realize (widget);

	gtk_widget_add_events (widget, GDK_PROPERTY_CHANGE_MASK |
					GDK_STRUCTURE_MASK);
	window = gtk_widget_get_window (widget);

	XSetSelectionOwner (display, priv->selection_atom,
			GDK_WINDOW_XID (window), CurrentTime);

	if (XGetSelectionOwner (display, priv->selection_atom) != None)
	{
		xevent.type = ClientMessage;
		xevent.window = root_window;
		xevent.message_type = priv->manager_atom;
		xevent.format = 32;
		xevent.data.l[0] = CurrentTime;
		xevent.data.l[1] = priv->selection_atom;
		xevent.data.l[2] = GDK_WINDOW_XID (window);
		xevent.data.l[3] = 0;
		xevent.data.l[4] = 0;

		XSendEvent (display, root_window, False,
			StructureNotifyMask, (XEvent*)&xevent);

		gdk_window_add_filter (window, gtk_tray_handle_xevent, tray);
	}
}

static void
gtk_tray_class_init (GtkTrayClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->realize = gtk_tray_realize;

	g_type_class_add_private (class, sizeof (GtkTrayPrivate));
}

static void
gtk_tray_init (GtkTray *tray)
{
}

GtkWidget*
gtk_tray_new (void)
{
	GtkWidget *ret = g_object_new (GTK_TYPE_TRAY, NULL);

	gtk_orientable_set_orientation (GTK_ORIENTABLE (ret),
					GTK_ORIENTATION_HORIZONTAL);

	return ret;
}
