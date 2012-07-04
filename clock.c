#include "clock.h"

typedef struct _GtkClockPrivate GtkClockPrivate;

struct _GtkClockPrivate
{
	struct tm time;
};

#define GTK_CLOCK_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_CLOCK, GtkClockPrivate))

G_DEFINE_TYPE (GtkClock, gtk_clock, GTK_TYPE_DRAWING_AREA);

static void
gtk_clock_redraw_canvas (GtkClock *clock)
{
	GtkWidget *widget = GTK_WIDGET (clock);

	gtk_widget_queue_draw_area (widget, 0, 0,
				gtk_widget_get_allocated_width (widget),
				gtk_widget_get_allocated_height (widget));
}

static gboolean
gtk_clock_update (gpointer data)
{
	GtkClock *clock = GTK_CLOCK (data);
	GtkClockPrivate *priv = GTK_CLOCK_GET_PRIVATE (clock);
	time_t timet;

	time (&timet);
	localtime_r (&timet, &priv->time);

	gtk_clock_redraw_canvas (clock);

	return TRUE;
}

static gboolean
gtk_clock_draw (GtkWidget *clock, cairo_t *cr)
{
	gint width = gtk_widget_get_allocated_width (clock);
	gint height = gtk_widget_get_allocated_height (clock);
	gdouble cx = width / 2.0;
	gdouble cy = height / 2.0;
	gdouble radius = MIN (width/2, height/2) - 5;
	gint inset;
	gint i;
	GtkClockPrivate *priv = GTK_CLOCK_GET_PRIVATE (clock);
	struct tm *time = &priv->time;

	cairo_set_line_width (cr, 2.0 * cairo_get_line_width (cr));
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);

	for (i = 0; i < 12; i++)
	{
		cairo_save (cr);

		if (i % 3 == 0)
			inset = 0.2 * radius;
		else {
			inset = 0.1 * radius;
			cairo_set_line_width (cr,
				0.5 * cairo_get_line_width (cr));
		}

		cairo_move_to (cr,
				cx + (radius - inset) * cos (i * M_PI / 6),
				cy + (radius - inset) * sin (i * M_PI / 6));
		cairo_line_to (cr,
				cx + radius * cos (i * M_PI / 6),
				cy + radius * sin (i * M_PI / 6));
		cairo_stroke (cr);

		cairo_restore (cr);
	}

	cairo_save (cr);
	cairo_set_line_width (cr, 2.5 * cairo_get_line_width (cr));
	cairo_move_to (cr, cx, cy);
	cairo_line_to (cr, cx + radius / 2 * sin (M_PI / 6 * time->tm_hour +
						M_PI / 360 * time->tm_min),
			cy + radius / 2 * -cos (M_PI / 6 * time->tm_hour +
						M_PI / 360 * time->tm_min));
	cairo_stroke (cr);
	cairo_restore (cr);

	cairo_move_to (cr, cx, cy);
	cairo_line_to (cr, cx + radius * 0.75 * sin (M_PI / 30 * time->tm_min),
			cy + radius * 0.75 * -cos (M_PI / 30 * time->tm_min));
	cairo_stroke (cr);

	cairo_set_source_rgb (cr, 1, 0, 0);
	cairo_move_to (cr, cx, cy);
	cairo_line_to (cr, cx + radius * 0.75 * sin (M_PI / 30 * time->tm_sec),
			cy + radius * 0.75 * -cos (M_PI / 30 * time->tm_sec));
	cairo_stroke (cr);

	return TRUE;
}

static void
gtk_clock_class_init (GtkClockClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->draw = gtk_clock_draw;

	g_type_class_add_private (class, sizeof (GtkClockPrivate));
}

static void
gtk_clock_init (GtkClock *clock)
{
	gtk_clock_update (clock);

	g_timeout_add (1000, gtk_clock_update, clock);
}

GtkWidget*
gtk_clock_new (void)
{
	return g_object_new (GTK_TYPE_CLOCK, NULL);
}
