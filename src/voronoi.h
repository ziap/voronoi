#ifndef VORONOI_H
#define VORONOI_H

#define export __attribute__((visibility("default")))

export void render_init();
export void render_update(float);

export void resize(int, int);

#endif
