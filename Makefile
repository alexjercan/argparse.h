CC = clang
CFLAGS = -Wall -Wextra -g
BUILD_DIR = build
SRC_DIR = how_to
LIB_HEADER = argparse.h

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
EXECS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SRCS))

all: $(EXECS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(LIB_HEADER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

