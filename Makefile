CC = g++
COPT = `sdl2-config --libs --cflags` -ggdb3 -O3 -Wall -lSDL2_image -lm

BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/objects
HEAD_DIR = $(BUILD_DIR)/headers

OUTPUT_FILE = main

SRCS = $(wildcard *.cpp)
HEAD = $(wildcard *.h)

OBJS = $(addprefix $(OBJS_DIR)/,$(notdir $(SRCS:.cpp=.o)))

default: main

info: $(OBJS)
	$(info SRCS is $(SRCS))
	$(info OBJS is $(OBJS))

main: $(OBJS)
	$(CC) $(COPT) -o $(OUTPUT_FILE) $(OBJS)

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(HEAD_DIR)
	rm -f $(OUTPUT_FILE)
	rm -f *.temp

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	cc $(COPT) -c $< -o $@ 
