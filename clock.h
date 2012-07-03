#ifndef __CLOCK_H
#define __CLOCK_H

#include <gtk/gtk.h>
#include <math.h>

typedef struct _GtkClock		GtkClock;
typedef struct _GtkClockClass		GtkClockClass;

struct _GtkClock
{
	GtkDrawingArea parent;

	/* private */
};

struct _GtkClockClass
{
	GtkDrawingAreaClass parent_class;
};

#define GTK_TYPE_CLOCK		(gtk_clock_get_type ())
#define GTK_CLOCK(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CLOCK, GtkClock))
#define GTK_CLOCK_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_CLOCK,  GtkClockClass))
#define GTK_IS_CLOCK(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CLOCK))
#define GTK_IS_CLOCK_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_CLOCK))
#define GTK_CLOCK_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CLOCK, GtkClockClass))

GtkWidget* gtk_clock_new (void);

#endif // __CLOCK_H
