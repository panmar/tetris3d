CC	:= g++
CCFLAGS	:= -std=c++23 \
	-Wall -Wno-deprecated-declarations \
	-g \
	-Isrc -Isrc/third_party \
	-MMD -MP

LDFLAGS	:= -lglfw \
	-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \

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

