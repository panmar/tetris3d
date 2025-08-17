CC	:= g++
CCFLAGS	:= -std=c++23 \
	-Wall -Wno-deprecated-declarations \
	-g \
	-Isrc -Isrc/third_party \
	-MMD -MP

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	LDFLAGS	:= -lglfw \
		-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else ifeq ($(UNAME_S), Linux)
	GLFW_LIB := src/third_party/libglfw3.a
	LDFLAGS := $(GLFW_LIB) \
		-lGL -lX11 -lpthread -ldl -lXrandr -lXi
endif

SRC_DIR := src
BIN_DIR := bin
SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%.cc=$(BIN_DIR)/%.o)

TARGET := $(BIN_DIR)/tetris3d

.PHONY: game clean

game: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

-include $(OBJS:.o=.d)

