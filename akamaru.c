/*                                           -*- mode: c; c-basic-offset: 2 -*-
 * See:
 *
 *     http://en.wikipedia.org/wiki/Verlet_integration
 *     http://www.teknikus.dk/tj/gdc2001.htm
 *
 * TODO:
 *
 * - Add circle object
 * - Try out this idea: make constraint solver take mean of all
 *   corrections at the end instead of meaning as it goes.
 * - Make a gravitation object - initialized with a vector or just a point.
 */

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "akamaru.h"

static void
akamaru_link_init (AkamaruLink *link)
{
  link->prev = link;
  link->next = link;
}

static void
akamaru_link_insert_before (AkamaruLink *link, AkamaruLink *anchor)
{
  link->prev = anchor->prev;
  link->next = anchor;
  link->prev->next = link;
  link->next->prev = link;
}

static void
akamaru_link_remove (AkamaruLink *link)
{
  link->prev->next = link->next;
  link->next->prev = link->prev;
}

static void
akamaru_list_init (AkamaruList *list, size_t offset)
{
  akamaru_link_init (&list->head);
  list->offset = offset;
}

static void
akamaru_list_for_each (AkamaruList *list, AkamaruListFunc func, void *data)
{
  AkamaruLink *l, *next;

  for (l = list->head.next; l != &list->head; l = next)
    {
      next = l->next;
      func ((char *) l - list->offset, data);
    }
}

static void
akamaru_list_append (AkamaruList *list, void *element)
{
  AkamaruLink *link;

  link = (AkamaruLink *) ((char *) element + list->offset);
  akamaru_link_insert_before (link, &list->head);
}

void
akamaru_object_init (AkamaruObject *object, double x, double y, double mass)
{
  object->position.x = x;
  object->position.y = y;
  object->previous_position.x = x;
  object->previous_position.y = y;
  object->mass = mass;
}

void
akamaru_spring_init (AkamaruSpring *spring, AkamaruObject *a, AkamaruObject *b, double length)
{
  spring->a = a;
  spring->b = b;
  spring->length = length;
}

AkamaruSpring *
akamaru_model_add_spring (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length)
{
  AkamaruSpring *spring;

  spring = g_new (AkamaruSpring, 1);
  spring->a = a;
  spring->b = b;
  spring->length = length;
  akamaru_list_append (&model->spring_list, spring);

  return spring;
}

void
akamaru_model_delete_spring (AkamaruModel *model, AkamaruSpring *spring)
{
  akamaru_link_remove (&spring->link);
  g_free (spring);
}

void
akamaru_model_for_each_spring (AkamaruModel *model, AkamaruSpringFunc func, void *data)
{
  akamaru_list_for_each (&model->spring_list, (AkamaruListFunc) func, data);
}

void
akamaru_stick_init (AkamaruStick *stick, AkamaruObject *a, AkamaruObject *b, double length)
{
  stick->a = a;
  stick->b = b;
  stick->length = length;
}

AkamaruStick *
akamaru_model_add_stick (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length)
{
  AkamaruStick *stick;

  stick = g_new (AkamaruStick, 1);
  stick->a = a;
  stick->b = b;
  stick->length = length;
  akamaru_list_append (&model->stick_list, stick);

  return stick;
}

void
akamaru_model_for_each_stick (AkamaruModel *model, AkamaruStickFunc func, void *data)
{
  akamaru_list_for_each (&model->stick_list, (AkamaruListFunc) func, data);
}

void
akamaru_string_init (AkamaruString *string, AkamaruObject *a, AkamaruObject *b, double length)
{
  string->a = a;
  string->b = b;
  string->length = length;
}

AkamaruString *
akamaru_model_add_string (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length)
{
  AkamaruString *string;

  string = g_new (AkamaruString, 1);
  string->a = a;
  string->b = b;
  string->length = length;
  akamaru_list_append (&model->string_list, string);

  return string;
}

void
akamaru_model_delete_string (AkamaruModel *model, AkamaruString *string)
{
  akamaru_link_remove (&string->link);
  g_free (string);
}

void
akamaru_model_for_each_string (AkamaruModel *model, AkamaruStringFunc func, void *data)
{
  akamaru_list_for_each (&model->string_list, (AkamaruListFunc) func, data);
}

AkamaruOffsetSpring *
akamaru_model_add_offset_spring (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b,
			 double dx, double dy)
{
  AkamaruOffsetSpring *spring;

  spring = g_new (AkamaruOffsetSpring, 1);
  spring->a = a;
  spring->b = b;
  spring->dx = dx;
  spring->dy = dy;
  akamaru_list_append (&model->offset_spring_list, spring);

  return spring;
}

void
akamaru_model_for_each_offset_spring (AkamaruModel *model, AkamaruOffsetSpringFunc func, void *data)
{
  akamaru_list_for_each (&model->offset_spring_list, (AkamaruListFunc) func, data);
}

void
akamaru_spacer_init (AkamaruSpacer *spacer, AkamaruObject *a, AkamaruObject *b, double length)
{
  spacer->a = a;
  spacer->b = b;
  spacer->length = length;
}

AkamaruSpacer *
akamaru_model_add_spacer (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length)
{
  AkamaruSpacer *spacer;

  spacer = g_new (AkamaruSpacer, 1);
  spacer->a = a;
  spacer->b = b;
  spacer->length = length;
  akamaru_list_append (&model->spacer_list, spacer);

  return spacer;
}

void
akamaru_model_delete_spacer (AkamaruModel *model, AkamaruSpacer *spacer)
{
  akamaru_link_remove (&spacer->link);
  g_free (spacer);
}

void
akamaru_model_for_each_spacer (AkamaruModel *model, AkamaruSpacerFunc func, void *data)
{
  akamaru_list_for_each (&model->spacer_list, (AkamaruListFunc) func, data);
}

void
akamaru_anchor_init (AkamaruAnchor *anchor, AkamaruObject *object, double x, double y)
{
  anchor->object = object;
  anchor->x = x;
  anchor->y = y;
}

AkamaruAnchor *
akamaru_model_add_anchor (AkamaruModel *model, AkamaruObject *object, double x, double y)
{
  AkamaruAnchor *anchor;

  anchor = g_new (AkamaruAnchor, 1);
  anchor->object = object;
  anchor->x = x;
  anchor->y = y;
  akamaru_list_append (&model->anchor_list, anchor);

  return anchor;
}

void
akamaru_model_delete_anchor (AkamaruModel *model, AkamaruAnchor *anchor)
{
  akamaru_link_remove (&anchor->link);
  g_free (anchor);
}

void
akamaru_model_for_each_anchor (AkamaruModel *model, AkamaruAnchorFunc func, void *data)
{
  akamaru_list_for_each (&model->anchor_list, (AkamaruListFunc) func, data);
}

void
akamaru_polygon_init_from_va_list (AkamaruPolygon *p, int enclosing,
			   int num_points, va_list ap)
{
  double dx, dy, length;
  int i, j;

  /* Polygons are defined counter-clock-wise in a coordinate system
   * with the y-axis pointing down. */

  p->num_points = num_points;
  p->points = g_new (AkamaruPoint, num_points);
  p->enclosing = enclosing;

  for (i = 0; i < num_points; i++) {
    p->points[i].x = va_arg (ap, double);
    p->points[i].y = va_arg (ap, double);
  }

  p->normals = g_new (AkamaruVector, p->num_points);
  /* Compute outward pointing normals.  p->normals[i] is the normal
   * for the edged between p->points[i] and p->points[i + 1]. */
  for (i = 0; i < p->num_points; i++) {
    j = (i + 1) % p->num_points;
    dx = p->points[j].x - p->points[i].x;
    dy = p->points[j].y - p->points[i].y;
    length = sqrt (dx * dx + dy * dy);
    p->normals[i].x = -dy / length;
    p->normals[i].y = dx / length;
  }
}

void
akamaru_polygon_init (AkamaruPolygon *polygon, int enclosing, int num_points, ...)
{
  va_list ap;

  va_start (ap, num_points);
  akamaru_polygon_init_from_va_list (polygon, enclosing, num_points, ap);
  va_end (ap);
}

AkamaruPolygon *
akamaru_model_add_polygon (AkamaruModel *model, int enclosing, int num_points, ...)
{
  AkamaruPolygon *polygon;
  va_list ap;

  polygon = g_new0 (AkamaruPolygon, 1);
  va_start (ap, num_points);
  akamaru_polygon_init_from_va_list (polygon, enclosing, num_points, ap);
  va_end (ap);
  akamaru_list_append (&model->polygon_list, polygon);

  return polygon;
}

void
akamaru_model_delete_polygon (AkamaruModel *model, AkamaruPolygon *polygon)
{
  akamaru_link_remove (&polygon->link);
  g_free (polygon);
}

void
akamaru_model_for_each_polygon (AkamaruModel *model, AkamaruPolygonFunc func, void *data)
{
  akamaru_list_for_each (&model->polygon_list, (AkamaruListFunc) func, data);
}

AkamaruPolygon *
akamaru_model_add_diamond (AkamaruModel *model, double x, double y)
{
  return akamaru_model_add_polygon (model, FALSE, 5,
			    x, y,
			    x + 10, y + 40,
			    x + 90, y + 40,
			    x + 100, y,
			    x + 50, y - 20);
}

AkamaruPolygon *
akamaru_model_add_rectangle (AkamaruModel *model, double x0, double y0, double x1, double y1)
{
  return akamaru_model_add_polygon (model, FALSE, 4, x0, y0, x0, y1, x1, y1, x1, y0);
}

AkamaruPolygon *
akamaru_model_add_enclosing_rectangle (AkamaruModel *model, double x0, double y0,
			       double x1, double y1)
{
  return akamaru_model_add_polygon (model, TRUE, 4, x0, y0, x0, y1, x1, y1, x1, y0);
}

AkamaruOffset *
akamaru_model_add_offset (AkamaruModel *model, int num_objects, double dx, double dy)
{
  AkamaruOffset *offset;

  offset = g_new (AkamaruOffset, 1);
  offset->num_objects = num_objects;
  offset->objects = g_new (AkamaruObject *, num_objects);
  offset->dx = dx;
  offset->dy = dy;
  akamaru_list_append (&model->offset_list, offset);

  return offset;
}

void
akamaru_model_for_each_offset (AkamaruModel *model, AkamaruOffsetFunc func, void *data)
{
  akamaru_list_for_each (&model->offset_list, (AkamaruListFunc) func, data);
}

void
akamaru_model_init (AkamaruModel *model)
{
  memset (model, 0, sizeof *model);
  akamaru_list_init (&model->object_list, G_STRUCT_OFFSET (AkamaruObject, link));
  akamaru_list_init (&model->spacer_list, G_STRUCT_OFFSET (AkamaruSpacer, link));
  akamaru_list_init (&model->string_list, G_STRUCT_OFFSET (AkamaruString, link));
  akamaru_list_init (&model->stick_list, G_STRUCT_OFFSET (AkamaruStick, link));
  akamaru_list_init (&model->spring_list, G_STRUCT_OFFSET (AkamaruSpring, link));
  akamaru_list_init (&model->anchor_list, G_STRUCT_OFFSET (AkamaruAnchor, link));
  akamaru_list_init (&model->polygon_list, G_STRUCT_OFFSET (AkamaruPolygon, link));
  akamaru_list_init (&model->offset_list, G_STRUCT_OFFSET (AkamaruOffset, link));
  akamaru_list_init (&model->offset_spring_list,
	     G_STRUCT_OFFSET (AkamaruOffsetSpring, link));
}

AkamaruObject *
akamaru_model_add_object (AkamaruModel *model, double x, double y, double mass, void *data)
{
  AkamaruObject *object;

  object = g_new (AkamaruObject, 1);
  object->position.x = x;
  object->position.y = y;
  object->previous_position.x = x;
  object->previous_position.y = y;
  object->mass = mass;
  akamaru_list_append (&model->object_list, object);
  object->data = data;

  return object;
}

void
akamaru_model_delete_object (AkamaruModel *model, AkamaruObject *object)
{
  akamaru_link_remove (&object->link);
  g_free (object);
}

void
akamaru_model_for_each_object (AkamaruModel *model, AkamaruObjectFunc func, void *data)
{
  akamaru_list_for_each (&model->object_list, (AkamaruListFunc) func, data);
}

static void
free_element (void *element, void *data)
{
  g_free (element);
}

void
akamaru_model_delete_spacers (AkamaruModel *model)
{
  akamaru_list_for_each (&model->spacer_list, free_element, NULL);
  akamaru_list_init (&model->spacer_list, G_STRUCT_OFFSET (AkamaruSpacer, link));
}

static void
free_polygon (AkamaruPolygon *polygon, void *data)
{
  g_free (polygon->points);
  g_free (polygon->normals);
  g_free (polygon);
}

static void
free_offset (AkamaruOffset *offset, void *data)
{
  g_free (offset->objects);
  g_free (offset);
}

void
akamaru_model_fini (AkamaruModel *model)
{
  akamaru_list_for_each (&model->object_list, free_element, NULL);
  akamaru_list_for_each (&model->spacer_list, free_element, NULL);
  akamaru_list_for_each (&model->string_list, free_element, NULL);
  akamaru_list_for_each (&model->stick_list, free_element, NULL);
  akamaru_list_for_each (&model->spring_list, free_element, NULL);
  akamaru_list_for_each (&model->anchor_list, free_element, NULL);
  akamaru_model_for_each_polygon (model, free_polygon, NULL);
  akamaru_model_for_each_offset (model, free_offset, NULL);
  akamaru_list_for_each (&model->offset_spring_list, free_element, NULL);
}

static void
akamaru_object_accumulate_forces (AkamaruObject *object, void *data)
{
  AkamaruModel *model = data;
  AkamaruVector v;

  /* Gravity */
  object->force.x = object->gravity.x * object->mass;
  object->force.y = object->gravity.y * object->mass;

  /* Friction */
  v.x = object->position.x - object->previous_position.x;
  v.y = object->position.y - object->previous_position.y;
  object->force.x -= v.x * ((object->friction != 0) ?
                                   object->friction : model->friction);
  object->force.y -= v.y * ((object->friction != 0) ?
                                   object->friction : model->friction);
}

static void
akamaru_spring_accumulate_forces (AkamaruSpring *spring, void *data)
{
  AkamaruModel *model = data;
  double x, y, dx, dy, distance, displacement;
  AkamaruVector u;

  x = spring->a->position.x;
  y = spring->a->position.y;
  dx = spring->b->position.x - x;
  dy = spring->b->position.y - y;
  distance = sqrt (dx * dx + dy * dy);
  u.x = dx / distance;
  u.y = dy / distance;
  displacement = distance - spring->length;
  spring->a->force.x += u.x * model->k * displacement;
  spring->a->force.y += u.y * model->k * displacement;
  spring->b->force.x -= u.x * model->k * displacement;
  spring->b->force.y -= u.y * model->k * displacement;
}

void
akamaru_offset_spring_accumulate_forces (AkamaruOffsetSpring *spring, void *data)
{
  AkamaruModel *model = data;
  double dx, dy;
  AkamaruPoint middle;

  middle.x = (spring->a->position.x + spring->b->position.x) / 2;
  middle.y = (spring->a->position.y + spring->b->position.y) / 2;

  dx = (middle.x - spring->dx / 2) - spring->a->position.x;
  dy = (middle.y - spring->dy / 2) - spring->a->position.y;

  spring->a->force.x += dx * model->k;
  spring->a->force.y += dy * model->k;
  spring->b->force.x -= dx * model->k;
  spring->b->force.y -= dy * model->k;
}

static void
akamaru_model_accumulate_forces (AkamaruModel *model)
{
  akamaru_model_for_each_object (model, akamaru_object_accumulate_forces, model);
  akamaru_model_for_each_spring (model, akamaru_spring_accumulate_forces, model);
  akamaru_model_for_each_offset_spring (model,
				akamaru_offset_spring_accumulate_forces, model);
}

static void
akamaru_object_integrate (AkamaruObject *object, void *data)
{
  double x, y, step;

  step = * (double *) data;
  x = object->position.x;
  y = object->position.y;

  object->position.x =
    x + (x - object->previous_position.x) + object->force.x * step * step;
  object->position.y =
    y + (y - object->previous_position.y) + object->force.y * step * step;

  object->previous_position.x = x;
  object->previous_position.y = y;
}

/* The square root in the distance computation for the string and
 * stick constraints can be aproximated using Newton:
 *
 *    distance =
 *      (model->sticks[i].length +
 *       (dx * dx + dy * dy) / model->sticks[i].length) / 2;
 *
 * This works really well, since the constraints aren't typically
 * violated much.  Thus, the distance is really close to the stick
 * length, which then makes a good initial guess.  However, the
 * approximation seems to be slower that just calling sqrt()...
 */

static inline double
estimate_distance (double dx, double dy, double r)
{
#ifdef APPROXIMATE_SQUARE_ROOTS
  return (r + (dx * dx + dy * dy) / r) / 2;
#else
  return sqrt (dx * dx + dy * dy);
#endif
}

static int
akamaru_polygon_contains_point (AkamaruPolygon *polygon, AkamaruPoint *point)
{
  int i;
  double dx, dy;

  for (i = 0; i < polygon->num_points; i++) {
    dx = point->x - polygon->points[i].x;
    dy = point->y - polygon->points[i].y;

    if (polygon->normals[i].x * dx + polygon->normals[i].y * dy >= 0)
      return polygon->enclosing;
  }

  return !polygon->enclosing;
}

static int
akamaru_polygon_contains_rectangle (AkamaruPolygon *polygon, AkamaruObject *object)
{
  int i;
  double dx, dy;

  for (i = 0; i < polygon->num_points; i++) {
    dx = object->position.x + polygon->normals[i].x * object->radius - polygon->points[i].x;
    dy = object->position.y + polygon->normals[i].y * object->radius - polygon->points[i].y;

    if (polygon->normals[i].x * dx + polygon->normals[i].y * dy >= 0.0)
      return polygon->enclosing;
  }

  return !polygon->enclosing;
}

static void
akamaru_polygon_reflect_object (AkamaruPolygon *polygon, AkamaruObject *object, double elasticity)
{
  int i, edge = 0;
  double d, distance;
  AkamaruVector *n = NULL;

  distance  = -1000;

  for (i = 0; i < polygon->num_points; i++) {
    d = polygon->normals[i].x * (object->position.x - polygon->points[i].x + polygon->normals[i].x * object->radius) +
      polygon->normals[i].y * (object->position.y - polygon->points[i].y + polygon->normals[i].y * object->radius);

    if (d > distance) {
      distance = d;
      edge = i;
      n = &polygon->normals[i];
    }
  }

  object->position.x -= (1 + elasticity) * distance * n->x;
  object->position.y -= (1 + elasticity) * distance * n->y;

  distance =
    n->x * (object->previous_position.x - polygon->points[edge].x + polygon->normals[edge].x * object->radius) +
    n->y * (object->previous_position.y - polygon->points[edge].y + polygon->normals[edge].y * object->radius);

  object->previous_position.x -= (1 + elasticity) * distance * n->x;
  object->previous_position.y -= (1 + elasticity) * distance * n->y;
}

typedef struct _ObjectConstrainPolygonClosure {
  AkamaruModel *model;
  AkamaruPolygon *polygon;
} ObjectConstrainPolygonClosure;

static void
akamaru_object_constrain_polygon (AkamaruObject *object, void *data)
{
  ObjectConstrainPolygonClosure *closure = data;

//   if (akamaru_polygon_contains_point (closure->polygon, &object->position))
   if (akamaru_polygon_contains_rectangle (closure->polygon, object))
    akamaru_polygon_reflect_object (closure->polygon, object,
			    closure->model->elasticity);
}

static void
akamaru_model_constrain_polygon (AkamaruPolygon *polygon, void *data)
{
  ObjectConstrainPolygonClosure closure;
  closure.polygon = polygon;
  closure.model = data;

  akamaru_model_for_each_object (closure.model, akamaru_object_constrain_polygon, &closure);
}

static void
akamaru_model_constrain_anchor (AkamaruAnchor *anchor, void *data)
{
  if (anchor->object == NULL)
    return;

  anchor->object->position.x = anchor->x;
  anchor->object->position.y = anchor->y;
}

static void
akamaru_model_constrain_offset (AkamaruOffset *offset, void *data)
{
  double x, y;
  int i;

  x = 0;
  y = 0;
  for (i = 0; i < offset->num_objects; i++)
    {
      x += offset->objects[i]->position.x;
      y += offset->objects[i]->position.y;
    }

  x = x / offset->num_objects - offset->dx * (offset->num_objects - 1) / 2;
  y = y / offset->num_objects - offset->dy * (offset->num_objects - 1) / 2;

  for (i = 0; i < offset->num_objects; i++)
    {
      offset->objects[i]->position.x = x + offset->dx * i;
      offset->objects[i]->position.y = y + offset->dy * i;
    }
}

static void
akamaru_model_constrain_string (AkamaruString *string, void *data)
{
  double x, y, dx, dy, distance, fraction;

  x  = string->a->position.x;
  y  = string->a->position.y;
  dx = string->b->position.x - x;
  dy = string->b->position.y - y;
  distance = estimate_distance (dx, dy, string->length);

  if (distance < string->length)
    return;

  fraction = (distance - string->length) / distance / 2;
  string->a->position.x = x + dx * fraction;
  string->a->position.y = y + dy * fraction;
  string->b->position.x = x + dx * (1 - fraction);
  string->b->position.y = y + dy * (1 - fraction);
}


static void
akamaru_model_constrain_spacer (AkamaruSpacer *spacer, void *data)
{
  double x, y, dx, dy, distance, fraction;

  x = spacer->a->position.x;
  y = spacer->a->position.y;
  dx = spacer->b->position.x - x;
  dy = spacer->b->position.y - y;

  distance = estimate_distance (dx, dy, spacer->length);
  if (distance > spacer->length)
    return;

  fraction = (distance - spacer->length) / distance / 2;
  spacer->a->position.x = x + dx * fraction;
  spacer->a->position.y = y + dy * fraction;
  spacer->b->position.x = x + dx * (1 - fraction);
  spacer->b->position.y = y + dy * (1 - fraction);
}

static void
akamaru_model_constrain_stick (AkamaruStick *stick, void *data)
{
  double x, y, dx, dy, distance, fraction;

  x  = stick->a->position.x;
  y  = stick->a->position.y;
  dx = stick->b->position.x - x;
  dy = stick->b->position.y - y;

  distance = estimate_distance (dx, dy, stick->length);

  fraction = (distance - stick->length) / distance / 2;
  stick->a->position.x = x + dx * fraction;
  stick->a->position.y = y + dy * fraction;
  stick->b->position.x = x + dx * (1 - fraction);
  stick->b->position.y = y + dy * (1 - fraction);
}

static void
akamaru_model_constrain (AkamaruModel *model)
{
  akamaru_model_for_each_anchor (model, akamaru_model_constrain_anchor, model);
  akamaru_model_for_each_string (model, akamaru_model_constrain_string, model);
  akamaru_model_for_each_spacer (model, akamaru_model_constrain_spacer, model);
  akamaru_model_for_each_stick (model, akamaru_model_constrain_stick, model);
  akamaru_model_for_each_offset (model, akamaru_model_constrain_offset, model);
  akamaru_model_for_each_polygon (model, akamaru_model_constrain_polygon, model);
}

void
akamaru_model_step (AkamaruModel *model, double delta_t)
{
  int i;

  akamaru_model_accumulate_forces (model);
  akamaru_model_for_each_object (model, akamaru_object_integrate, &delta_t);
  for (i = 0; i < model->constrain_iterations; i++)
    akamaru_model_constrain (model);

  model->theta += delta_t;
}

static double
akamaru_object_distance (AkamaruObject *object, double x, double y)
{
  double dx, dy;

  dx = object->position.x - x;
  dy = object->position.y - y;

  return sqrt (dx*dx + dy*dy);
}

typedef struct {
  double x;
  double y;
  double distance;
  AkamaruObject *object;
} FindNearestClosure;

static void
akamaru_model_update_closest (AkamaruObject *object, void *data)
{
  FindNearestClosure *closure = data;
  double distance;

  distance = akamaru_object_distance (object, closure->x, closure->y);
  if (closure->object == NULL || distance < closure->distance) {
    closure->distance = distance;
    closure->object = object;
  }
}

AkamaruObject *
akamaru_model_find_nearest (AkamaruModel *model, double x, double y)
{
  FindNearestClosure closure;

  closure.x = x;
  closure.y = y;
  closure.object = NULL;
  akamaru_model_for_each_object (model, akamaru_model_update_closest, &closure);

  return closure.object;
}
