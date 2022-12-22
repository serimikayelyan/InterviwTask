SERVER_TARGET_EXEC ?= server_app
CLIENT_TARGET_EXEC ?= client_app

TARGET_EXEC ?= $(SERVER_TARGET_EXEC) $(CLIENT_TARGET_EXEC)

BUILD_DIR ?= ./build
SERVER_SRC_DIRS ?= ./server
CLIENT_SRC_DIRS ?= ./client

SERVER_SRCS := $(shell find $(SERVER_SRC_DIRS) -name '*.cpp' -or -name '*.c')
CLIENT_SRCS := $(shell find $(CLIENT_SRC_DIRS) -name '*.cpp' -or -name '*.c')
SERVER_OBJS := $(SERVER_SRCS:%=$(BUILD_DIR)/%.o)
CLIENT_OBJS := $(CLIENT_SRCS:%=$(BUILD_DIR)/%.o)

all: $(TARGET_EXEC)

$(SERVER_TARGET_EXEC) server: $(BUILD_DIR)/$(SERVER_TARGET_EXEC)

$(CLIENT_TARGET_EXEC) client: $(BUILD_DIR)/$(CLIENT_TARGET_EXEC)

$(BUILD_DIR)/$(SERVER_TARGET_EXEC): $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o $@ $(LDFLAGS) -lpthread

$(BUILD_DIR)/$(CLIENT_TARGET_EXEC): $(CLIENT_OBJS)
	$(CC) $< -o $@ $(LDFLAGS)

$(SERVER_OBJS): $(SERVER_SRCS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJS): $(CLIENT_SRCS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

MKDIR_P ?= mkdir -p
