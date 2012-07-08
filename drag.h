#ifndef __DRAG_H
#define __DRAG_H

#include <gtk/gtk.h>

typedef struct _GtkDrag		GtkDrag;
typedef struct _GtkDragClass		GtkDragClass;

struct _GtkDrag
{
	GtkEventBox parent;

	/* private */
};

struct _GtkDragClass
{
	GtkEventBoxClass parent_class;
};

#define GTK_TYPE_DRAG		(gtk_drag_get_type ())
#define GTK_DRAG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CLOCK, GtkDrag))
#define GTK_DRAG_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_CLOCK,  GtkDragClass))
#define GTK_IS_DRAG(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CLOCK))
#define GTK_IS_DRAG_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_CLOCK))
#define GTK_DRAG_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CLOCK, GtkDragClass))

GtkWidget* gtk_drag_new (void);

#endif // __DRAG_H
