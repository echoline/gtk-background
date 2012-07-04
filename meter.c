#include "meter.h"

typedef struct _GtkMeterPrivate GtkMeterPrivate;

struct _GtkMeterPrivate
{
	gdouble low;
	gdouble high;
	gdouble value;
	gchar *name;
};

#define GTK_METER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_METER, GtkMeterPrivate))

G_DEFINE_TYPE (GtkMeter, gtk_meter, GTK_TYPE_DRAWING_AREA);

static void
gtk_meter_redraw_canvas (GtkMeter *meter)
{
	GtkWidget *widget = GTK_WIDGET (meter);

	gtk_widget_queue_draw_area (widget, 0, 0,
				gtk_widget_get_allocated_width (widget),
				gtk_widget_get_allocated_height (widget));
}

static gboolean
gtk_meter_draw (GtkWidget *meter, cairo_t *cr)
{
	gint width = gtk_widget_get_allocated_width (meter);
	gint height = gtk_widget_get_allocated_height (meter);
	gdouble cx = width / 2.0;
	gdouble cy = height / 2.0;
	gdouble radius = MIN (width/2, height/2) - 5;
	gdouble ny = cy + radius * 0.9;
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);
	gdouble low = priv->low;
	gdouble high = priv->high;
	gdouble value = priv->value;
	gdouble theta = (((value - low) / (high - low)) * 0.5 - 0.25) * M_PI;
	cairo_text_extents_t extents;
	gchar *name = priv->name;
	gdouble inc;
	cairo_pattern_t *pat;

	cairo_set_line_width (cr, 2.0 * cairo_get_line_width (cr));
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);

	for (inc = -0.25; inc <= 0.25; inc += 0.05)
	{
		cairo_move_to (cr, cx + radius * 1.2 * sin (inc * M_PI),
				ny + radius * 1.2 * -cos (inc * M_PI));
		cairo_line_to (cr, cx + radius * 1.25 * sin (inc * M_PI),
				ny + radius * 1.25 * -cos (inc * M_PI));
		cairo_stroke (cr);
	}

	cairo_set_source_rgb (cr, 1, 0, 0);
	cairo_move_to (cr, cx, ny);
	cairo_line_to (cr, cx + radius * sin (theta),
			ny + radius * -cos (theta));
	cairo_stroke (cr);

	pat = cairo_pattern_create_radial (cx, cy, radius,
                                   0,  0, radius);
	cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 1, 0.2);
	cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0.3);
	cairo_set_source (cr, pat);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);

	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_arc (cr, cx, cy, radius, 0.1 * M_PI, 0.9 * M_PI);
	cairo_fill (cr);

	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
					CAIRO_FONT_WEIGHT_NORMAL);
	cairo_text_extents (cr, name, &extents);
	cairo_move_to (cr, cx - (extents.width/2 + extents.x_bearing),
			cy + radius / 2.0);
	cairo_show_text (cr, name);

	return TRUE;
}

static void
gtk_meter_class_init (GtkMeterClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->draw = gtk_meter_draw;

	g_type_class_add_private (class, sizeof (GtkMeterPrivate));
}

static void
gtk_meter_init (GtkMeter *meter)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);
	priv->low = 0.0;
	priv->high = 1.0;
	priv->value = 0.5;
}

GtkWidget*
gtk_meter_new (gchar *name)
{
	GtkWidget *meter = g_object_new (GTK_TYPE_METER, NULL);
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	priv->name = g_strdup(name);

	return meter;
}

void
gtk_meter_set_range (GtkMeter *meter, gdouble low, gdouble high)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	priv->low = low;
	priv->high = high;

	gtk_meter_redraw_canvas (meter);
}

void
gtk_meter_set_value (GtkMeter *meter, gdouble value)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	if (priv->low >= value)
		priv->value = priv->low;
	else if (priv->high <= value)
		priv->value = priv->high;
	else
		priv->value = value;

	gtk_meter_redraw_canvas (meter);
}

void
gtk_meter_set_high (GtkMeter *meter, gdouble high)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	priv->high = high;

	gtk_meter_redraw_canvas (meter);
}

void
gtk_meter_set_low (GtkMeter *meter, gdouble low)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	priv->low = low;

	gtk_meter_redraw_canvas (meter);
}

gdouble
gtk_meter_get_value (GtkMeter *meter)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	return priv->value;
}

gdouble
gtk_meter_get_low (GtkMeter *meter)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	return priv->low;
}

gdouble
gtk_meter_get_high (GtkMeter *meter)
{
	GtkMeterPrivate *priv = GTK_METER_GET_PRIVATE (meter);

	return priv->high;
}
