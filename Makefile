CC=clang
CFLAGS=-Wall -Wextra -march=native -mtune=native
LDLIBS=-lm -lglfw -lGL -lGLEW -lX11 -lGLU
DEBUG_FLAGS=-Og -g
BUILD_FLAGS=-O3 -s
WASM_FLAGS=-O3 --target=wasm32 -flto -fno-builtin -nostdlib -fvisibility=hidden
WASM_LD_FLAGS=--no-entry --strip-all --lto-O3 --allow-undefined --export-dynamic
WASM_OPT_FLAGS=-O3

INPUT_DIR=src
INPUTS=$(wildcard $(INPUT_DIR)/*.c)
OUTPUT=voronoi

SHADER_DIR=shaders
FRAGMENT_SHADERS=$(wildcard $(SHADER_DIR)/*.frag)
SHADER_OUTPUTS=$(patsubst %.frag, $(SHADER_DIR)/%.h, $(notdir $(FRAGMENT_SHADERS)))

.PHONY: build debug web $(OUTPUT).wasm

build: build/$(OUTPUT)

debug: debug/$(OUTPUT)

web: $(OUTPUT).wasm

build/$(OUTPUT): $(INPUTS) $(SHADER_OUTPUTS)
	@mkdir -p build
	$(CC) -o $@ $(INPUTS) $(CFLAGS) $(BUILD_FLAGS) $(LDLIBS)

debug/$(OUTPUT): $(INPUTS) $(SHADER_OUTPUTS)
	@mkdir -p debug
	$(CC) -o $@ $(INPUTS) $(CFLAGS) $(DEBUG_FLAGS) $(LDLIBS)

$(OUTPUT).wasm: $(SHADER_OUTPUTS)
	$(CC) $(INPUT_DIR)/voronoi.c -o voronoi.o $(WASM_FLAGS) $(CFLAGS) -c
	wasm-ld -o $@ $(WASM_LD_FLAGS) voronoi.o
	wasm-opt $@ -o $@ $(WASM_OPT_FLAGS)
	rm voronoi.o

$(SHADER_OUTPUTS): $(SHADER_DIR)/%.h: $(SHADER_DIR)/%.frag $(SHADER_DIR)/%.vert
	@echo Generating $@
	@echo "static const char* $(basename $(notdir $@))_frag = " > $@
	@sed 's/^/\"/g' $(word 1,$^) | sed 's/$$/\\n\"/g' >> $@
	@echo ";" >> $@
	@echo "static const char* $(basename $(notdir $@))_vert = " >> $@
	@sed 's/^/\"/g' $(word 2,$^) | sed 's/$$/\\n\"/g' >> $@
	@echo ";" >> $@
