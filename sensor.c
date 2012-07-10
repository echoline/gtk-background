#include <gtk/gtk.h>
#include <fcntl.h>
#include <poll.h>
#include "sensor.h"

typedef struct _SensorPrivate SensorPrivate;

struct _SensorPrivate
{
	GIOChannel *channel;
	gdouble heading;
	gdouble pitch;
	gdouble roll;
};

#define SENSOR_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), SENSOR_TYPE, SensorPrivate))

enum {
    SIGNAL_UPDATED,
    SIGNAL_LAST
};

static guint sensor_signals[SIGNAL_LAST];

G_DEFINE_TYPE (Sensor, sensor, G_TYPE_OBJECT);

static gboolean
sensor_timeout_cb (gpointer data)
{
	Sensor *sensor = data;
	SensorPrivate *priv = SENSOR_GET_PRIVATE (sensor);
	struct pollfd fds[1];
	gchar *str;
	fds[0].fd = g_io_channel_unix_get_fd (priv->channel);
	fds[0].events = POLLIN;

	if (poll(fds, 1, 0) > 0)
	{
		if (fds[0].revents & POLLIN)
			if (g_io_channel_read_line (priv->channel, &str,
						NULL, NULL, NULL) 
					== G_IO_STATUS_NORMAL)
			{
				if (sscanf (str, "heading %lf pitch %lf "
						"roll %lf",
						&priv->heading, &priv->pitch,
						&priv->roll) == 3)
				{
					printf ("%lf %lf %lf\n", priv->heading,
						priv->pitch, priv->roll);
					g_signal_emit (sensor,
						sensor_signals[SIGNAL_UPDATED],
						0);
				}
				g_free (str);
			}
			else
				return FALSE;

		if (fds[0].revents & POLLHUP)
			return FALSE;
	}

	return TRUE;
}

static void
sensor_init (Sensor *sensor)
{
	SensorPrivate *priv = SENSOR_GET_PRIVATE (sensor);

	int fd = open ("/tmp/arduino", O_RDONLY);
	priv->channel = g_io_channel_unix_new (fd);

	if (priv->channel)
	{
		g_timeout_add (1, sensor_timeout_cb, sensor);
	}
}

void
sensor_class_init (SensorClass *klass)
{
	GObjectClass *class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (class, sizeof (SensorPrivate));

	sensor_signals[SIGNAL_UPDATED] = g_signal_new ("updated",
				SENSOR_TYPE, G_SIGNAL_RUN_FIRST,
				0, /* class_offset */
				NULL, /* accumulator */
				NULL, /* accu_data */
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);
}

GObject*
sensor_new (void)
{
        GObject *ret = g_object_new (SENSOR_TYPE, NULL);

        return ret;
}

gdouble
sensor_get_heading (Sensor *sensor)
{
	SensorPrivate *priv = SENSOR_GET_PRIVATE (sensor);
	return priv->heading;
}

gdouble
sensor_get_pitch (Sensor *sensor)
{
	SensorPrivate *priv = SENSOR_GET_PRIVATE (sensor);
	return priv->pitch;
}

gdouble
sensor_get_roll (Sensor *sensor)
{
	SensorPrivate *priv = SENSOR_GET_PRIVATE (sensor);
	return priv->roll;
}
