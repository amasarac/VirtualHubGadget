CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build

SRCS = \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/isochronous_queue.c \
    $(SRC_DIR)/interrupt_transfer_queue.c \
    $(SRC_DIR)/bulk_transfer_queue.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET = usb-gadget

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
