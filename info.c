#include "info.h"
#include "meter.h"

static gboolean
cpu_meter_update (gpointer data)
{
	GtkMeter *meter = GTK_METER (data);

	gchar *contents;

	if (g_file_get_contents("/proc/loadavg", &contents, NULL, NULL)) {
		gtk_meter_set_value (meter, g_ascii_strtod (contents, NULL));
		g_free(contents);
	}

	return TRUE;
}

GtkWidget*
cpu_init ()
{
	GtkWidget *meter = gtk_meter_new ("cpu");

	gdouble processors = 0.0;
	gchar *contents, *tmp;

	if (g_file_get_contents("/proc/cpuinfo", &contents, NULL, NULL)) {
		tmp = contents;
		while ((tmp = g_strstr_len (tmp, -1, "processor"))) {
			tmp++;
			processors++;
		}
		g_free(contents);
	}

	gtk_meter_set_high (GTK_METER (meter), processors);

	cpu_meter_update (meter);
	g_timeout_add (1000, cpu_meter_update, meter);

	return meter;
}

static gboolean
mem_meter_update (gpointer data)
{
	GtkMeter *meter = GTK_METER (data);

	gchar *contents, **lines, *tmp;
	gint i = 0;

	if (g_file_get_contents("/proc/meminfo", &contents, NULL, NULL)) {
		lines = g_strsplit (contents, "\n", 0);
		while (lines[i] != NULL) {
			if (!g_ascii_strncasecmp (lines[i], "MemFree:", 8)) {
				tmp = g_strstr_len (lines[i], -1, " ");
				if (tmp != NULL) {
					while (g_ascii_isspace (*tmp))
						tmp++;
					gtk_meter_set_value (GTK_METER (meter), 
						g_ascii_strtod (tmp, NULL));
				}
				break;
			}
			i++;
		}
		g_strfreev (lines);
		g_free (contents);
	}

	return TRUE;
}

GtkWidget*
mem_init ()
{
	GtkWidget *meter = gtk_meter_new ("mem");

	gchar *contents, **lines, *tmp;
	gint i = 0;

	if (g_file_get_contents("/proc/meminfo", &contents, NULL, NULL)) {
		lines = g_strsplit (contents, "\n", 0);
		while (lines[i] != NULL) {
			if (!g_ascii_strncasecmp (lines[i], "MemTotal:", 9)) {
				tmp = g_strstr_len (lines[i], -1, " ");
				if (tmp != NULL) {
					while (g_ascii_isspace (*tmp))
						tmp++;
					gtk_meter_set_high (GTK_METER (meter), 
						g_ascii_strtod (tmp, NULL));
				}
				break;
			}
			i++;
		}
		g_strfreev (lines);
		g_free (contents);
	}

	mem_meter_update (meter);
	g_timeout_add (1000, mem_meter_update, meter);

	return meter;
}
