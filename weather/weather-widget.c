#include "weather-widget.h"
#define GWEATHER_I_KNOW_THIS_IS_UNSTABLE
#include "weather-priv.h"

typedef struct _GtkWeatherPrivate GtkWeatherPrivate;

struct _GtkWeatherPrivate
{
	GWeatherInfo *info;
	GtkWidget *temp;
	GtkWidget *icon;
	GtkWidget *wind;
};

#define GTK_WEATHER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_WEATHER, GtkWeatherPrivate))

G_DEFINE_TYPE (GtkWeather, gtk_weather, GTK_TYPE_BOX);

static gboolean
gtk_weather_update (gpointer data)
{
	GtkWeather *weather = GTK_WEATHER (data);
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);

	gweather_info_update (priv->info);

	return TRUE;
}

static void
gtk_weather_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GTK_WIDGET_CLASS (gtk_weather_parent_class)->size_allocate (widget,
                                allocation);
}

static void
gtk_weather_class_init (GtkWeatherClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->size_allocate = gtk_weather_size_allocate;

	g_type_class_add_private (class, sizeof (GtkWeatherPrivate));
}

void
gtk_weather_updated (gpointer data, gpointer user_data)
{
	GWeatherInfo *info = GWEATHER_INFO (data);
	GtkWeather *weather = GTK_WEATHER (user_data);
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);
	GdkRGBA color;

	gdk_rgba_parse (&color, "black");

	const gchar *icon_name = gweather_info_get_icon_name (info);

	gtk_label_set_text (GTK_LABEL (priv->temp),
				gweather_info_get_temp (info));
	gtk_widget_override_color (priv->temp, GTK_STATE_FLAG_NORMAL,
				&color);
	gtk_image_set_from_icon_name (GTK_IMAGE (priv->icon), icon_name,
					 GTK_ICON_SIZE_DIALOG);
	gtk_label_set_text (GTK_LABEL (priv->wind),
				gweather_info_get_wind (info));
	gtk_widget_override_color (priv->wind, GTK_STATE_FLAG_NORMAL,
				&color);
}

static void
gtk_weather_init (GtkWeather *weather)
{
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (weather);
	priv->info = g_object_new (GWEATHER_TYPE_INFO, NULL);
	priv->info->priv->location = _weather_location_new (
				"La Grande, OR", "KLGD",
				NULL, NULL, FALSE, -1.0, -1.0, NULL, NULL);

	gtk_weather_update (weather);

	g_timeout_add (30 * 60 * 1000, gtk_weather_update, weather);
	g_signal_connect (priv->info, "updated",
			G_CALLBACK (gtk_weather_updated), weather);
}

GtkWidget*
gtk_weather_new (void)
{
	GtkWidget *ret = g_object_new (GTK_TYPE_WEATHER, NULL);
	GtkWeatherPrivate *priv = GTK_WEATHER_GET_PRIVATE (GTK_WEATHER (ret));

	priv->temp = gtk_label_new ("");
	priv->icon = gtk_image_new ();
	priv->wind = gtk_label_new ("");

	gtk_container_add (GTK_CONTAINER (ret), priv->temp);
	gtk_container_add (GTK_CONTAINER (ret), priv->icon);
	gtk_container_add (GTK_CONTAINER (ret), priv->wind);

	gtk_orientable_set_orientation (GTK_ORIENTABLE (ret),
					GTK_ORIENTATION_VERTICAL);

	return ret;
}
