#ifndef VORONOI_H
#define VORONOI_H

#define export __attribute__((visibility("default")))

export void voronoi_init(void);
export void voronoi_update(float);

export void resize(int, int);

#endif
