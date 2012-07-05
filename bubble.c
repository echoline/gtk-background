#include "bubble.h"

G_DEFINE_TYPE (GtkBubble, gtk_bubble, GTK_TYPE_BIN);

static gboolean
gtk_bubble_draw (GtkWidget *bubble, cairo_t *cr)
{
	gint width = gtk_widget_get_allocated_width (bubble);
	gint height = gtk_widget_get_allocated_height (bubble);
	gdouble cx = width / 2.0;
	gdouble cy = height / 2.0;
	gdouble radius = MIN (width/2, height/2) - 5;
	cairo_pattern_t *pat;
	GtkWidget *child = gtk_bin_get_child (GTK_BIN (bubble));

	cairo_set_line_width (cr, 2.0 * cairo_get_line_width (cr));
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_fill_preserve (cr);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);

	if (child != NULL) {
		cairo_save (cr);
		cairo_move_to (cr,
			cx - gtk_widget_get_allocated_width (child) / 2,
			cy - gtk_widget_get_allocated_height (child) / 2);
		gtk_widget_draw (child, cr);
		cairo_restore (cr);
	}

	pat = cairo_pattern_create_radial (cx, cy, radius,
                                   0,  0, radius);
	cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 0, 0.4);
	cairo_pattern_add_color_stop_rgba (pat, 1, 1, 1, 1, 0.2);
	cairo_set_source (cr, pat);
	cairo_arc (cr, cx, cy, radius, 0, 2 * M_PI);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);

	return TRUE;
}

static void
gtk_bubble_size_allocate (GtkWidget *bubble, GtkAllocation *allocation)
{
	GtkWidget *child = gtk_bin_get_child (GTK_BIN (bubble));

	gtk_widget_set_allocation (bubble, allocation);

	gtk_widget_set_allocation (child, allocation);
}

static void
gtk_bubble_class_init (GtkBubbleClass *klass)
{
	GtkWidgetClass *class = GTK_WIDGET_CLASS (klass);

	class->draw = gtk_bubble_draw;
	class->size_allocate = gtk_bubble_size_allocate;
}

static void
gtk_bubble_init (GtkBubble *bubble)
{
}

GtkWidget*
gtk_bubble_new (void)
{
	return g_object_new (GTK_TYPE_BUBBLE, NULL);
}
