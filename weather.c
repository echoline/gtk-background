#define GWEATHER_I_KNOW_THIS_IS_UNSTABLE

#include "weather.h"
#include <math.h>
#include <libgweather/weather.h>
#include <libgweather/gweather-gconf.h>
#include <libgweather/gweather-location.h>
#include <libgweather/location-entry.h>

typedef struct _GtkWeatherPrivate GtkWeatherPrivate;

struct _GtkWeatherPrivate
{
	GWeatherLocation *location;
	GdkPixbufAnimation *radar;
};

#define GTK_WEATHER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_WEATHER, GtkWeatherPrivate))

G_DEFINE_TYPE (GtkWeather, gtk_weather, GTK_TYPE_DRAWING_AREA);

static void
gtk_weather_redraw_canvas (GtkWeather *weather)
{
	GtkWidget *widget = GTK_WIDGET (weather);

	gtk_widget_queue_draw_area (widget, 0, 0,
				gtk_widget_get_allocated_width (widget),
				gtk_widget_get_allocated_height (widget));
}

static gboolean
gtk_weather_update (gpointer data)
{
	GtkWeather *weather = GTK_WEATHER (data);
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);
	WeatherInfo *info;
	info = gweather_location_get_weather (priv->location);
	priv->radar = weather_info_get_radar (info);

	gtk_weather_redraw_canvas (weather);

	return TRUE;
}

static gboolean
gtk_weather_draw (GtkWidget *weather, cairo_t *cr)
{
	gint width = gtk_widget_get_allocated_width (weather);
	gint height = gtk_widget_get_allocated_height (weather);
	gdouble cx = width / 2.0;
	gdouble cy = height / 2.0;
	gdouble radius = MIN (width/2, height/2) - 5;
	cairo_pattern_t *pat;
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);
	GdkPixbuf *pixbuf = gdk_pixbuf_animation_get_static_image (priv->radar);

	cairo_set_line_width (cr, 2.0 * cairo_get_line_width (cr));
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);

	gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
	cairo_paint (cr);

	pat = cairo_pattern_create_radial (cx, cy, radius,
                                   0,  0, radius);
	cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 1, 0.2);
	cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0.3);
	cairo_set_source (cr, pat);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);

	return TRUE;
}

static void
gtk_weather_class_init (GtkWeatherClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->draw = gtk_weather_draw;

	g_type_class_add_private (class, sizeof (GtkWeatherPrivate));
}

static void
gtk_weather_init (GtkWeather *weather)
{
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);
	GWeatherLocation *world = gweather_location_new_world (TRUE);
	GWeatherLocationEntry *entry = GWEATHER_LOCATION_ENTRY
				(gweather_location_entry_new (world));

	gweather_location_entry_set_city (entry, NULL, "KLGD");
	priv->location = gweather_location_entry_get_location (entry);

	gtk_weather_update (weather);

	g_timeout_add (15 * 60 * 1000, gtk_weather_update, weather);
}

GtkWidget*
gtk_weather_new (void)
{
	return g_object_new (GTK_TYPE_WEATHER, NULL);
}
