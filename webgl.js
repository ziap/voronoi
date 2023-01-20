const canvas = document.querySelector('canvas')
const gl = canvas.getContext('webgl2')

let memory_buffer
const decoder = new TextDecoder()

function cstr(ptr) {
  const mem_arr = new Uint8Array(memory_buffer)
  let len = 0;
  while (mem_arr[ptr + len] != 0) {
    len++
  }
  const bytes = mem_arr.slice(ptr, ptr + len)
  const str = decoder.decode(bytes);
  return str
}

let shaders = []
let programs = []
let buffers = []
let uniforms = []

WebAssembly.instantiateStreaming(fetch('./voronoi.wasm'), {
  env: {
    frand() {
      return Math.random()
    },
    fsin(x) {
      return Math.sin(x)
    },
    fcos(x) {
      return Math.cos(x)
    },
    puts(str) {
      console.log(cstr(str))
    },
    glCreateBuffer() {
      const buffer = gl.createBuffer()
      buffers.push(buffer)
      return buffers.length - 1
    },
    glCreateShader(type) {
      const shader = gl.createShader(type)
      shaders.push(shader)
      return shaders.length - 1
    },
    glCreateProgram() {
      const program = gl.createProgram()
      programs.push(program)
      return programs.length - 1
    },
    glGetUniformLocation(program, name) {
      const location = gl.getUniformLocation(programs[program], cstr(name))
      uniforms.push(location)
      return uniforms.length - 1
    },
    glClearColor(r, g, b, a) {
      gl.clearColor(r, g, b, a)
    },
    glViewport(x, y, w, h) {
      gl.viewport(x, y, w, h)
    },
    glSetShaderSource(shader, src) {
      gl.shaderSource(shaders[shader], cstr(src))
    },
    glCompileShader(shader) {
      gl.compileShader(shaders[shader])
    },
    glGetShaderParameter(shader, pname) {
      return gl.getShaderParameter(shaders[shader], pname)
    },
    glDeleteShader(shader) {
      gl.deleteShader(shaders[shader])
    },
    glAttachShader(program, shader) {
      gl.attachShader(programs[program], shaders[shader])
    },
    glLinkProgram(program) {
      gl.linkProgram(programs[program])
    },
    glValidateProgram(program) {
      gl.validateProgram(programs[program])
    },
    glEnable(cap) {
      gl.enable(cap)
    },
    glUseProgram(program) {
      gl.useProgram(programs[program])
    },
    glBindBuffer(target, buffer) {
      gl.bindBuffer(target, buffers[buffer])
    },
    glEnableVertexAttribArray(index) {
      gl.enableVertexAttribArray(index)
    },
    glBufferData(target, size, data, usage) {
      if (data != 0) {
        const slice = new Uint8Array(memory_buffer, data, size)
        gl.bufferData(target, slice, usage)
      } else {
        gl.bufferData(target, size, usage)
      }
    },
    glVertexAttribPointer(index, size, type, normalized, stride, offset) {
      gl.vertexAttribPointer(index, size, type, normalized, stride, offset)
    },
    glClear(mask) {
      gl.clear(mask)
    },
    glBufferSubData(target, offset, size, data) {
      const slice = new Uint8Array(memory_buffer, data, size)
      gl.bufferSubData(target, offset, slice)
    },
    glUniform2f(location, x, y) {
      gl.uniform2f(uniforms[location], x, y)
    },
    glUniform1f(location, x) {
      gl.uniform1f(uniforms[location], x)
    },
    glUniform1i(location, x) {
      gl.uniform1i(uniforms[location], x)
    },
    glDrawArrays(mode, first, count) {
      gl.drawArrays(mode, first, count)
    }
  }
}).then(wasm => {
  const { exports } = wasm.instance
  memory_buffer = exports.memory.buffer


  function resize() {
    canvas.width = innerWidth
    canvas.height = innerHeight
    exports.resize(innerWidth, innerHeight)
  }

  addEventListener('resize', resize)
  resize()

  exports.render_init()
  addEventListener('mousemove', e => {
    exports.mouse_move(e.x, e.y)
  })


  let last = performance.now()
  let dt = 0

  setInterval(() => {
    exports.render_update(dt)
    const now = performance.now()
    dt = (now - last) / 1000
    last = now
  }, 0)
})