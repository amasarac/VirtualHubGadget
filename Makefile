CC = gcc
CFLAGS = -Wall -Wextra -pthread
TEST_CFLAGS = $(CFLAGS) -I$(SRC_DIR)
TEST_LDFLAGS = -lcmocka
LDFLAGS = -ludev

SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET = usb-gadget
.PHONY: all clean
TEST_BIN = tests/test_queues

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

$(TEST_BIN): $(BUILD_DIR)/bulk_transfer_queue.o $(BUILD_DIR)/interrupt_transfer_queue.o $(BUILD_DIR)/isochronous_queue.o tests/test_queues.c
	$(CC) $(TEST_CFLAGS) $^ -o $@ $(TEST_LDFLAGS)

.PHONY: test
test: $(TEST_BIN)
	./$(TEST_BIN)
