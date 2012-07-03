#ifndef __TRAY_H
#define __TRAY_H

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <X11/Xatom.h>

typedef struct _GtkTray		GtkTray;
typedef struct _GtkTrayClass	GtkTrayClass;

struct _GtkTray
{
	GtkBox parent;

	/* private */
};

struct _GtkTrayClass
{
	GtkBoxClass parent_class;
};

#define GTK_TYPE_TRAY		(gtk_tray_get_type ())
#define GTK_TRAY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_TRAY, GtkTray))
#define GTK_TRAY_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_TRAY,  GtkTrayClass))
#define GTK_IS_TRAY(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_TRAY))
#define GTK_IS_TRAY_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_TRAY))
#define GTK_TRAY_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_TRAY, GtkTrayClass))

GtkWidget* gtk_tray_new (void);

#endif // __TRAY_H
