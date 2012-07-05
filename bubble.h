#ifndef __BUBBLE_H
#define __BUBBLE_H

#include <gtk/gtk.h>
#include <math.h>

typedef struct _GtkBubble		GtkBubble;
typedef struct _GtkBubbleClass		GtkBubbleClass;

struct _GtkBubble
{
	GtkBin parent;

	/* private */
};

struct _GtkBubbleClass
{
	GtkBinClass parent_class;
};

#define GTK_TYPE_BUBBLE		(gtk_bubble_get_type ())
#define GTK_BUBBLE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_BUBBLE, GtkBubble))
#define GTK_BUBBLE_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_BUBBLE,  GtkBubbleClass))
#define GTK_IS_BUBBLE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_BUBBLE))
#define GTK_IS_BUBBLE_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_BUBBLE))
#define GTK_BUBBLE_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_BUBBLE, GtkBubbleClass))

GtkWidget* gtk_bubble_new (void);

#endif // __BUBBLE_H
