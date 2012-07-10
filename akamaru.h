/*                                           -*- mode: c; c-basic-offset: 2 -*-
 */

#ifndef __AKAMARU_H__
#define __AKAMARU_H__

typedef struct _AkamaruLink AkamaruLink;
typedef struct _AkamaruList AkamaruList;
struct _AkamaruLink {
  AkamaruLink *prev;
  AkamaruLink *next;
};

struct _AkamaruList {
  AkamaruLink head;
  size_t offset;
};

typedef void (*AkamaruListFunc) (void *element, void *data);

typedef struct _AkamaruPoint AkamaruPoint;
typedef struct _AkamaruPoint AkamaruVector;
struct _AkamaruPoint {
  double x, y;
};

typedef struct _AkamaruObject AkamaruObject;
typedef struct _AkamaruStick AkamaruStick;
typedef struct _AkamaruString AkamaruString;
typedef struct _AkamaruSpring AkamaruSpring;
typedef struct _AkamaruOffsetSpring AkamaruOffsetSpring;
typedef struct _AkamaruSpacer AkamaruSpacer;
typedef struct _AkamaruAnchor AkamaruAnchor;
typedef struct _AkamaruPolygon AkamaruPolygon;
typedef struct _AkamaruOffset AkamaruOffset;
typedef struct _AkamaruModel AkamaruModel;

struct _AkamaruObject {
  AkamaruVector force;

  AkamaruPoint position;
  AkamaruPoint previous_position;
  AkamaruVector gravity;

  double radius;
  double mass;
  double theta;
  double friction;

  AkamaruLink link;

  void *data;
};

struct _AkamaruStick {
  AkamaruObject *a, *b;
  int length;
  AkamaruLink link;
};

struct _AkamaruString {
  AkamaruObject *a, *b;
  int length;
  AkamaruLink link;
};

struct _AkamaruOffset {
  int num_objects;
  AkamaruObject **objects;
  int dx, dy;
  AkamaruLink link;
};

struct _AkamaruSpring {
  AkamaruObject *a, *b;
  int length;
  AkamaruLink link;
};

struct _AkamaruOffsetSpring {
  AkamaruObject *a, *b;
  int dx, dy;
  AkamaruLink link;
};

struct _AkamaruSpacer {
  AkamaruObject *a, *b;
  int length;
  AkamaruLink link;
};

struct _AkamaruAnchor {
  AkamaruObject *object;
  double x, y;
  AkamaruLink link;
};

struct _AkamaruPolygon {
  int num_points;
  AkamaruPoint *points;
  AkamaruVector *normals;
  int enclosing;
  AkamaruLink link;
};

struct _AkamaruModel {
  AkamaruList object_list;
  AkamaruList spacer_list;
  AkamaruList string_list;
  AkamaruList stick_list;
  AkamaruList spring_list;
  AkamaruList anchor_list;
  AkamaruList polygon_list;
  AkamaruList offset_list;
  AkamaruList offset_spring_list;

  int num_objects;
  AkamaruObject *objects;
  int num_sticks;
  AkamaruStick *sticks;
  int num_strings;
  AkamaruString *strings;
  int num_offsets;
  AkamaruOffset *offsets;
  int num_springs;
  AkamaruSpring *springs;
  int num_offset_springs;
  AkamaruOffsetSpring *offset_springs;
  int num_spacers;
  AkamaruSpacer *spacers;
  int num_anchors;
  AkamaruAnchor *anchors;
  int num_polygons;
  AkamaruPolygon *polygons;

  double k;
  double elasticity;
  double friction;

  int constrain_iterations;
  double theta;
};

typedef void (*AkamaruObjectFunc) (AkamaruObject *object, void *data);

void akamaru_object_init (AkamaruObject *object, double x, double y, double mass);
void akamaru_offset_spring_init (AkamaruOffsetSpring *spring,
			 AkamaruObject *a, AkamaruObject *b, double dx, double dy);
void akamaru_spring_init (AkamaruSpring *spring, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_stick_init (AkamaruStick *stick, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_string_init (AkamaruString *string, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_spacer_init (AkamaruSpacer *spacer, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_anchor_init (AkamaruAnchor *anchor, AkamaruObject *object, double x, double y);

void akamaru_polygon_init (AkamaruPolygon *p, int enclosing, int num_points, ...);
void akamaru_polygon_init_diamond (AkamaruPolygon *polygon, double x, double y);
void akamaru_polygon_init_rectangle (AkamaruPolygon *polygon, double x0, double y0,
			     double x1, double y1);
void akamaru_polygon_init_enclosing_rectangle (AkamaruPolygon *polygon, double x0, double y0,
				       double x1, double y1);

void akamaru_model_init (AkamaruModel *model);
void akamaru_model_fini (AkamaruModel *model);

AkamaruObject *akamaru_model_add_object (AkamaruModel *model, double x, double y, double mass, void *data);
void akamaru_model_delete_object (AkamaruModel *model, AkamaruObject *object);
void akamaru_model_for_each_object (AkamaruModel *model, AkamaruObjectFunc func, void *data);

typedef void (*AkamaruSpacerFunc) (AkamaruSpacer *spacer, void *data);
AkamaruSpacer *akamaru_model_add_spacer (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_model_for_each_spacer (AkamaruModel *model, AkamaruSpacerFunc func, void *data);
void akamaru_model_delete_spacers (AkamaruModel *model);
void akamaru_model_delete_spacer (AkamaruModel *model, AkamaruSpacer *spacer);

typedef void (*AkamaruSpringFunc) (AkamaruSpring *spring, void *data);
AkamaruSpring *akamaru_model_add_spring (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_model_delete_spring (AkamaruModel *model, AkamaruSpring *spring);
void akamaru_model_for_each_spring (AkamaruModel *model, AkamaruSpringFunc func, void *data);

typedef void (*AkamaruStickFunc) (AkamaruStick *stick, void *data);
AkamaruStick *akamaru_model_add_stick (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_model_for_each_stick (AkamaruModel *model, AkamaruStickFunc func, void *data);

typedef void (*AkamaruStringFunc) (AkamaruString *string, void *data);
AkamaruString *akamaru_model_add_string (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b, double length);
void akamaru_model_delete_string (AkamaruModel *model, AkamaruString *string);
void akamaru_model_for_each_string (AkamaruModel *model, AkamaruStringFunc func, void *data);

typedef void (*AkamaruAnchorFunc) (AkamaruAnchor *anchor, void *data);
AkamaruAnchor *akamaru_model_add_anchor (AkamaruModel *model, AkamaruObject *object, double x, double y);
void akamaru_model_delete_anchor (AkamaruModel *model, AkamaruAnchor *anchor);
void akamaru_model_for_each_anchor (AkamaruModel *model, AkamaruAnchorFunc func, void *data);

typedef void (*AkamaruPolygonFunc) (AkamaruPolygon *polygon, void *data);
AkamaruPolygon *akamaru_model_add_polygon (AkamaruModel *model, int enclosing, int num_points, ...);
void            akamaru_model_delete_polygon (AkamaruModel *model, AkamaruPolygon *polygon);
AkamaruPolygon *akamaru_model_add_diamond (AkamaruModel *model, double x, double y);
AkamaruPolygon *akamaru_model_add_rectangle (AkamaruModel *model, double x0, double y0,
			      double x1, double y1);
AkamaruPolygon *akamaru_model_add_enclosing_rectangle (AkamaruModel *model, double x0, double y0,
					double x1, double y1);
void akamaru_model_for_each_polygon (AkamaruModel *model, AkamaruPolygonFunc func, void *data);

typedef void (*AkamaruOffsetSpringFunc) (AkamaruOffsetSpring *spring, void *data);
AkamaruOffsetSpring *akamaru_model_add_offset_spring (AkamaruModel *model, AkamaruObject *a, AkamaruObject *b,
				       double dx, double dy);
void akamaru_model_for_each_offset_spring (AkamaruModel *model,
				   AkamaruOffsetSpringFunc func, void *data);

typedef void (*AkamaruOffsetFunc) (AkamaruOffset *offset, void *data);
AkamaruOffset *akamaru_model_add_offset (AkamaruModel *model,
			  int num_objects, double dx, double dy);
void akamaru_model_for_each_offset (AkamaruModel *model, AkamaruOffsetFunc func, void *data);

void akamaru_model_step (AkamaruModel *model, double delta_t);

AkamaruObject *akamaru_model_find_nearest (AkamaruModel *model, double x, double y);

#endif
