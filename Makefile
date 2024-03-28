####################################################
# Note : this makefile has been made for macos
####################################################
# Compilers may work differently depending on the OS
####################################################
# The make old is an other version of the make which
# doesnt use precompiled headers files in case they 
# dont work well on certain system 
####################################################

# Compilation Parameters
COPT = -O3 -Wall 

# Compiled files directory
BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/objects
HEAD_DIR = $(BUILD_DIR)/headers

# Output file
OUTPUT_FILE = a
SUFFIX = out

# Getting the code in the same folders
SRCS = $(wildcard *.cpp)
HEAD = $(wildcard *.h)
OBJS = $(addprefix $(OBJS_DIR)/,$(notdir $(SRCS:.cpp=.o)))
PCHS = $(addprefix $(HEAD_DIR)/,$(notdir $(HEAD:.h=.h.gch)))

default: main

# Cleanses the compiled files
clean: 
	rm -rf $(OBJS_DIR)
	rm -rf $(HEAD_DIR)
	rm -f *.$(SUFFIX)
	rm -f *.temp
	rm -f *.h.gch

# Builds the program
main: $(PCHS) $(OBJS) 
	gcc $(COPT) $(OBJS) $(LIB) -o $(OUTPUT_FILE).$(SUFFIX)

# Old version of the make
old:  $(HEAD) $(OBJS)
	gcc $(COPT) $(OBJS) $(HEAD) $(LIB)

# Builds and run the program
run: main
	./$(OUTPUT_FILE).$(SUFFIX)

# Rule to make the .o files
$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(OBJS_DIR)
	cc $(COPT) -c $< -o $@ $(INC) -I./$(HEAD_DIR)

# Rule to make the .h.gch files
$(HEAD_DIR)/%.h.gch: %.h
	@mkdir -p $(HEAD_DIR)
	cc $(COPT) -c $< -o $@