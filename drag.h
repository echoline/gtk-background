#ifndef __DRAG_H
#define __DRAG_H

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
#define GTK_DRAG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_DRAG, GtkDrag))
#define GTK_DRAG_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_DRAG,  GtkDragClass))
#define GTK_IS_DRAG(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_DRAG))
#define GTK_IS_DRAG_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_DRAG))
#define GTK_DRAG_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_DRAG, GtkDragClass))

GtkWidget* gtk_drag_new (Sensor *sensor, AkamaruModel *model);
void gtk_drag_set_coords (GtkDrag *drag, gdouble x, gdouble y, gboolean);
void gtk_drag_get_coords (GtkDrag *drag, gdouble *x, gdouble *y);
void gtk_drag_animate (GtkDrag *drag);

#endif // __DRAG_H
