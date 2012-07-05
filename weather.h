#ifndef __WEATHER_H
#define __WEATHER_H

#include <gtk/gtk.h>

typedef struct _GtkWeather		GtkWeather;
typedef struct _GtkWeatherClass		GtkWeatherClass;

struct _GtkWeather
{
	GtkDrawingArea parent;

	/* private */
};

struct _GtkWeatherClass
{
	GtkDrawingAreaClass parent_class;
};

#define GTK_TYPE_WEATHER		(gtk_weather_get_type ())
#define GTK_WEATHER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_WEATHER, GtkWeather))
#define GTK_WEATHER_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GTK_WEATHER,  GtkWeatherClass))
#define GTK_IS_WEATHER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_WEATHER))
#define GTK_IS_WEATHER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GTK_TYPE_WEATHER))
#define GTK_WEATHER_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WEATHER, GtkWeatherClass))

GtkWidget* gtk_weather_new (void);

#endif // __WEATHER_H
