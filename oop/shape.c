#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct Shape;

struct ShapeVTable {
  double (*area)(const struct Shape *self);
  void (*draw)(const struct Shape *self);
  void (*destroy)(const struct Shape *self);
};

struct Shape {
  const struct ShapeVTable *vptr;

  int x;
  int y;
};

/* Interface to hide the internal vtable */

static inline double shape_area(const struct Shape *self) {
  return self->vptr->area(self);
}

static inline void shape_draw(const struct Shape *self) {
  return self->vptr->draw(self);
}

static inline void shape_destroy(const struct Shape *self) {
  self->vptr->destroy(self);
}

/* Circle */

struct Circle {
  struct Shape base;

  double radius;
};

double circle_area(const struct Shape *self) {
  struct Circle *c = (struct Circle *)self;
  return M_PI * c->radius * c->radius;
}

void circle_draw(const struct Shape *self) {
  struct Circle *c = (struct Circle *)self;
  printf("Draw Circle{r=%.2f} at (%d, %d)\n", c->radius, self->x, self->y);
}

void circle_destroy(const struct Shape *self) {
  /* Free any internal dynamic memory if it existed, then free self */
  free((void *)self);
}

static const struct ShapeVTable circle_vtable = {
    .area = circle_area,
    .draw = circle_draw,
    .destroy = circle_destroy,
};

#define MAKE_CIRCLE(X, Y, RADIUS)                                              \
  {                                                                            \
      .base =                                                                  \
          {                                                                    \
              .vptr = &circle_vtable,                                          \
              .x = (X),                                                        \
              .y = (Y),                                                        \
          },                                                                   \
      .radius = (RADIUS),                                                      \
  }

struct Circle *new_circle(int x, int y, double radius) {
  struct Circle *c = malloc(sizeof(struct Circle));
  if (!c) {
    return NULL;
  }

  c->base.vptr = &circle_vtable;
  c->base.x = x;
  c->base.y = y;

  c->radius = radius;

  return c;
}

/* Rectangle */

struct Rectangle {
  struct Shape base;

  double width;
  double height;
};

double rect_area(const struct Shape *self) {
  struct Rectangle *r = (struct Rectangle *)self;
  return r->width * r->height;
}

void rect_draw(const struct Shape *self) {
  struct Rectangle *r = (struct Rectangle *)self;
  printf("Draw Rectangle{width=%.2f, height=%.2f} at (%d, %d)\n", r->width,
         r->height, self->x, self->y);
}

void rect_destroy(const struct Shape *self) {
  /* Free any internal dynamic memory if it existed, then free self */
  free((void *)self);
}

static const struct ShapeVTable rect_vtable = {
    .area = rect_area,
    .draw = rect_draw,
    .destroy = rect_destroy,
};

#define MAKE_RECT(X, Y, WIDTH, HEIGHT)                                         \
  {                                                                            \
      .base =                                                                  \
          {                                                                    \
              .vptr = &rect_vtable,                                            \
              .x = (X),                                                        \
              .y = (Y),                                                        \
          },                                                                   \
      .width = (WIDTH),                                                        \
      .height = (HEIGHT),                                                      \
  }

struct Rectangle *new_rect(int x, int y, double width, double height) {
  struct Rectangle *r = malloc(sizeof(struct Rectangle));
  if (!r) {
    return NULL;
  }

  r->base.vptr = &rect_vtable;
  r->base.x = x;
  r->base.y = y;

  r->width = width;
  r->height = height;

  return r;
}

/* App */

#define LEN(ARR) (sizeof(ARR) / sizeof(ARR[0]))

int main(void) {
  /* Stack */
  struct Shape *shapes[] = {
      (struct Shape *)&(struct Circle)MAKE_CIRCLE(1, 2, 3.0),
      (struct Shape *)&(struct Rectangle)MAKE_RECT(7, 8, 9.0, 10.0),
      (struct Shape *)&(struct Circle)MAKE_CIRCLE(4, 5, 6.0),
      (struct Shape *)&(struct Rectangle)MAKE_RECT(11, 12, 13.0, 14.0)};

  for (int i = 0; i < LEN(shapes); i++) {
    struct Shape *shape = shapes[i];
    shape_draw(shape);
    printf("The area is %.2f\n", shape_area(shape));
  }

  /* Dynamic allocation */
  struct Shape *shapes1[] = {(struct Shape *)new_circle(1, 2, 3.0),
                             (struct Shape *)new_rect(7, 8, 9.0, 10.0),
                             (struct Shape *)new_circle(4, 5, 6.0),
                             (struct Shape *)new_rect(11, 12, 13.0, 14.0)};

  for (int i = 0; i < LEN(shapes1); i++) {
    struct Shape *shape = shapes1[i];
    shape_draw(shape);
    printf("The area is %.2f\n", shape_area(shape));
  }

  for (int i = 0; i < LEN(shapes1); i++) {
    shape_destroy(shapes1[i]);
  }

  return EXIT_SUCCESS;
}
