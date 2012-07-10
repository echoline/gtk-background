typedef struct _Sensor Sensor;
typedef struct _SensorClass SensorClass;

struct _Sensor
{
	GObject parent;
};

struct _SensorClass
{
	GObjectClass parent_class;
};

#define SENSOR_TYPE		(sensor_get_type ())
#define SENSOR(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), SENSOR_TYPE, obj))

GObject* sensor_new (void);
gdouble sensor_get_heading (Sensor *sensor);
gdouble sensor_get_pitch (Sensor *sensor);
gdouble sensor_get_roll (Sensor *sensor);
