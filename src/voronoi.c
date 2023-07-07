#include "voronoi.h"

#include "../shaders/voronoi.h"
#include "imports.h"

static int u_resolution;

static int width, height;
void resize(int new_w, int new_h) {
  width = new_w;
  height = new_h;
  glViewport(0, 0, new_w, new_h);
}

#define PI 3.141592653589793f

#define SEED_COUNT 1024
#define TRIGS_PER_SEED 32

static int compile_shader(const char *src, int type) {
  int shader = glCreateShader(type);
  glSetShaderSource(shader, src);
  glCompileShader(shader);

  if (!glGetShaderParameter(shader, GL_COMPILE_STATUS)) {
    const char *msg = type == GL_VERTEX_SHADER
                        ? "ERROR: Failed to compile fragment shader"
                        : "ERROR: Failed to compile vertex shader";
    puts(msg);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static int create_program(const char *vertex_src, const char *fragment_src) {
  int program = glCreateProgram();
  int vertex_shader = compile_shader(vertex_src, GL_VERTEX_SHADER);
  int fragment_shader = compile_shader(fragment_src, GL_FRAGMENT_SHADER);

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}

typedef struct {
  float x;
  float y;

  float vx;
  float vy;

  float hue;
} Seed;

typedef struct {
  float pos[2];

  float hue;
} Vertex;

Seed seeds[SEED_COUNT];

static Seed create_seed(void) {
  Seed s;
  s.x = frand();
  s.y = frand();

  s.hue = frand();

  float angle = frand() * 2 * PI;
  float speed = (0.4f + sqrtf(frand()) * 0.8f) / sqrtf(SEED_COUNT);

  s.vx = fcos(angle) * speed;
  s.vy = fsin(angle) * speed;
  return s;
}

Vertex vertices[3 * TRIGS_PER_SEED * SEED_COUNT];

void voronoi_init(void) {
  glEnable(GL_DEPTH_TEST);

  for (int i = 0; i < SEED_COUNT; ++i) {
    seeds[i] = create_seed();

    for (int j = 0; j < 3 * TRIGS_PER_SEED; ++j) {
      vertices[3 * TRIGS_PER_SEED * i + j].hue = seeds[i].hue;
    }
  }

  int main_program = create_program(voronoi_vert, voronoi_frag);
  glUseProgram(main_program);

  int vao = glCreateVertexArray();
  glBindVertexArray(vao);

  int vbo = glCreateBuffer();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0, 2, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, pos)
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, hue)
  );

  u_resolution = glGetUniformLocation(main_program, "u_resolution");

  int u_seed_scale = glGetUniformLocation(main_program, "u_seed_scale");
  int u_trig_count = glGetUniformLocation(main_program, "u_trig_count");

  glUniform1f(u_seed_scale, 4.0 / sqrtf(SEED_COUNT));
  glUniform1i(u_trig_count, TRIGS_PER_SEED);
}

static void update_seed(Seed *seed, float dt) {
  float dx = seed->vx * dt;
  float dy = seed->vy * dt;

  seed->x += dx;
  seed->y += dy;

  if (seed->x < 0 || seed->x > 1) {
    seed->x -= dx;
    seed->vx = -seed->vx;
  }

  if (seed->y < 0 || seed->y > 1) {
    seed->y -= dy;
    seed->vy = -seed->vy;
  }
}

void voronoi_update(float dt) {
  for (int i = 0; i < SEED_COUNT; ++i) {
    update_seed(seeds + i, dt);

    for (int j = 0; j < 3 * TRIGS_PER_SEED; ++j) {
      Vertex *v = vertices + (3 * TRIGS_PER_SEED * i + j);

      v->pos[0] = seeds[i].x;
      v->pos[1] = seeds[i].y;
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  glUniform2f(u_resolution, width, height);
  glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex));
}
