#ifndef VORONOI_H
#define VORONOI_H

#define export extern __attribute__((visibility("default")))

export void render_init();
export void render_update(float);

export void resize(int, int);
export void mouse_move(float, float);

#endif
