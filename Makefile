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
DEPS := $(OBJS:.o=.d)

#INC_DIRS := $(shell find $(SRC_DIRS) -type d)
#INC_FLAGS := $(addprefix -I,$(INC_DIRS))

#CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CPPFLAGS ?= $(INC_FLAGS)

test:
	@echo SERVER_SRCS: $(SERVER_SRCS)
	@echo SERVER_OBJS: $(SERVER_OBJS)
	@echo TARGET_EXEC: $(TARGET_EXEC)
	@echo SERVER_TARGET_EXEC: $(SERVER_TARGET_EXEC)

all: $(TARGET_EXEC)

$(SERVER_TARGET_EXEC) server: $(BUILD_DIR)/$(SERVER_TARGET_EXEC)

$(BUILD_DIR)/$(SERVER_TARGET_EXEC): $(SERVER_OBJS)
	@echo link SERVER_OBJS: $<
	$(CC) $(SERVER_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(CLIENT_TARGET_EXEC): $(CLIENT_OBJS)
	$(CC) $< -o $@ $(LDFLAGS)

$(SERVER_OBJS): $(SERVER_SRCS)
	@echo SERVER_OBJ_BUILD: $< $@
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJS): $(CLIENT_SRCS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
