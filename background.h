#ifndef __BACKGROUND_H
#define __BACKGROUND_H

typedef struct _GtkBackground		GtkBackground;
typedef struct _GtkBackgroundClass	GtkBackgroundClass;

struct _GtkBackground
{
	GtkFixed parent;

	/* private */
};

struct _GtkBackgroundClass
{
	GtkFixedClass parent_class;
};

#define GTK_TYPE_BACKGROUND		(gtk_background_get_type ())
#define GTK_BACKGROUND(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_BACKGROUND, GtkBackground))
#define GTK_BACKGROUND_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_BACKGROUND,  GtkBackgroundClass))
#define GTK_IS_BACKGROUND(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_BACKGROUND))
#define GTK_IS_BACKGROUND_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_BACKGROUND))
#define GTK_BACKGROUND_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_BACKGROUND, GtkBackgroundClass))

GtkWidget* gtk_background_new (void);

#endif // __BACKGROUND_H
