#ifndef __METER_H
#define __METER_H

#include <gtk/gtk.h>
#include <math.h>

typedef struct _GtkMeter		GtkMeter;
typedef struct _GtkMeterClass		GtkMeterClass;

struct _GtkMeter
{
	GtkDrawingArea parent;

	/* private */
};

struct _GtkMeterClass
{
	GtkDrawingAreaClass parent_class;
};

#define GTK_TYPE_METER		(gtk_meter_get_type ())
#define GTK_METER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_METER, GtkMeter))
#define GTK_METER_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_METER,  GtkMeterClass))
#define GTK_IS_METER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_METER))
#define GTK_IS_METER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_METER))
#define GTK_METER_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_METER, GtkMeterClass))

GtkWidget* gtk_meter_new (gchar*);
void gtk_meter_set_range (GtkMeter*, gdouble, gdouble);
void gtk_meter_set_low (GtkMeter*, gdouble);
void gtk_meter_set_high (GtkMeter*, gdouble);
void gtk_meter_set_value (GtkMeter*, gdouble);
gdouble gtk_meter_get_low (GtkMeter*);
gdouble gtk_meter_get_high (GtkMeter*);
gdouble gtk_meter_get_value (GtkMeter*);

#endif // __METER_H
