BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/objects
HEAD_DIR = $(BUILD_DIR)/headers
IMGUI_BUILD = $(BUILD_DIR)/imgui
IMGUI_DIR = imgui

OUTPUT_FILE = main

HEAD = $(wildcard *.h)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJS_DIR)/,$(notdir $(SRCS:.cpp=.o)))

IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_SRC += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
IMGUI_OBJ = $(addprefix $(IMGUI_BUILD)/,$(notdir $(IMGUI_SRC:.cpp=.o)))

CC = g++
COPT = `sdl2-config --libs --cflags` -ggdb3 -O3 -lSDL2_image -lm -w 
COPT += -I /usr/local/opt/glfws
COPT += -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
COPT += -I/usr/local/include -I/opt/local/include

LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lglfw3

default: main

info: $(OBJS)
	$(info SRCS is $(SRCS))
	$(info OBJS is $(OBJS))

main: $(OBJS) $(IMGUI_OBJ)
	$(CC) $(COPT) -o $(OUTPUT_FILE) $(OBJS) $(IMGUI_OBJ) $(LIBS)
	./main

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(HEAD_DIR)
	rm -f $(OUTPUT_FILE)
	rm -f *.temp

$(IMGUI_BUILD)/%.o:$(IMGUI_DIR)/%.cpp
	@mkdir -p $(IMGUI_BUILD)
	cc $(COPT) -c $< -o $@

$(IMGUI_BUILD)/%.o:$(IMGUI_DIR)/backends/%.cpp
	@mkdir -p $(IMGUI_BUILD)
	cc $(COPT) -c $< -o $@ 

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	cc $(COPT) -c $< -o $@ 
