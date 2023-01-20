#include "voronoi.h"

#include "../shaders/voronoi.h"
#include "imports.h"

static unsigned int main_program;
static int u_resolution;
static int u_trig_count;

static int width, height;
static float mouse_x, mouse_y;

void resize(int new_w, int new_h) {
  width = new_w;
  height = new_h;
  glViewport(0, 0, new_w, new_h);
}

void mouse_move(float xpos, float ypos) {
  mouse_x = xpos;
  mouse_y = height - ypos;
}

#define PI 3.141592653589793

#define SEED_COUNT 256
#define TRIGS_PER_SEED 32

const char *fragment_shader_compile_error = "Failed to compile fragment shader";
const char *vertex_shader_compile_error = "Failed to compile vertex shader";

static int compile_shader(const char *src, unsigned int type) {
  unsigned int shader = glCreateShader(type);
  glSetShaderSource(shader, src);
  glCompileShader(shader);

  if (!glGetShaderParameter(shader, GL_COMPILE_STATUS)) {
    if (type == GL_FRAGMENT_SHADER) puts(fragment_shader_compile_error);
    else
      puts(vertex_shader_compile_error);
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

  float dx;
  float dy;

  float color;
} Seed;

typedef struct {
  float position[2];

  float color;
} Vertex;

Seed seeds[SEED_COUNT];

Seed create_seed() {
  Seed s;

  s.x = frand();
  s.y = frand();

  s.color = frand();

  float angle = frand() * 2 * PI;
  float speed = 0.04 + frand() * 0.08;
  s.dx = fcos(angle) * speed;
  s.dy = fsin(angle) * speed;
  return s;
}

int vbo;
Vertex vertices[3 * TRIGS_PER_SEED * SEED_COUNT];

void render_init() {
  glEnable(GL_DEPTH_TEST);

  for (int i = 0; i < SEED_COUNT; ++i) seeds[i] = create_seed();

  main_program = create_program(voronoi_vert, voronoi_frag);
  glUseProgram(main_program);

  vbo = glCreateBuffer();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0, 2, GL_FLOAT, false, sizeof(Vertex),
    (const void *)offsetof(Vertex, position)
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1, 1, GL_FLOAT, false, sizeof(Vertex), (const void *)offsetof(Vertex, color)
  );

  u_resolution = glGetUniformLocation(main_program, "u_resolution");
  u_trig_count = glGetUniformLocation(main_program, "u_trig_count");
  glUniform1i(u_trig_count, TRIGS_PER_SEED);
};

void update_seed(Seed *seed, float dt) {
  seed->x += seed->dx * dt;
  seed->y += seed->dy * dt;

  if (seed->x < 0 || seed->x > 1) {
    seed->x -= seed->dx * dt * 2;
    seed->dx = -seed->dx;
  }

  if (seed->y < 0 || seed->y > 1) {
    seed->y -= seed->dy * dt * 2;
    seed->dy = -seed->dy;
  }
}

float current_time = 0;
void render_update(float dt) {
  current_time += dt;

  for (int i = 0; i < SEED_COUNT - 1; ++i) {
    update_seed(seeds + i, dt);

    Vertex v = {.position = {seeds[i].x, seeds[i].y}, .color = seeds[i].color};
    for (int j = 0; j < 3 * TRIGS_PER_SEED; ++j) {
      vertices[3 * TRIGS_PER_SEED * i + j] = v;
    }
  }

  Vertex v = {
    .position = {mouse_x / width, mouse_y / height},
    .color = 0.5 + 0.5 * fsin(current_time)};
  for (int i = 0; i < 3 * TRIGS_PER_SEED; ++i) {
    vertices[3 * TRIGS_PER_SEED * (SEED_COUNT - 1) + i] = v;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glUniform2f(u_resolution, width, height);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex));
}
