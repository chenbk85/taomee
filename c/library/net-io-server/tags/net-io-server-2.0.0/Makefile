CC = g++

CPPFLAGS += -I$(HEADER_DIR) -I$(INCLUDE_DIR) -W -Wall -Wundef -Wmissing-declarations -ggdb -fPIC -shared

TARGET = libnetioserver.so.2.0
SONAME = libnetioserver.so.2
LINK = libnetioserver.so

SOURCE_DIR = ./src/source
HEADER_DIR = ./src/header
INCLUDE_DIR = ./include
BIN_DIR = ./bin
OBJ_DIR = ./obj

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(HEADER_DIR)/*.h)
INCLUDE_FILES := $(wildcard $(INCLUDE_DIR)/*.h)
OBJS := $(patsubst %.cpp,%.o,$(subst $(SOURCE_DIR),$(OBJ_DIR),$(SOURCE_FILES)))
TARGET_FILES := $(BIN_DIR)/$(TARGET)

TEST_DIR = ./test
TEST_TARGET = $(BIN_DIR)/test
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

$(TARGET): $(OBJS)
	echo $(OBJS)
	$(CC) -o $(TARGET_FILES) -Wl,-soname,$(SONAME) $(OBJS) $(CPPFLAGS)
	@rm -f $(BIN_DIR)/$(LINK) $(BIN_DIR)/$(SONAME)
	@cd $(BIN_DIR);ln -s $(TARGET) $(LINK);ln -s $(TARGET) $(SONAME)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADER_DIR)/%.h $(INCLUDE_DIR)/*.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

.PHONY: clean o so test run static

o: $(OBJS)
	@echo "only generate obj files!"

clean:
	-rm $(OBJS) $(TARGET_FILES) $(TEST_TARGET) $(BIN_DIR)/libnetioserver.a -rf

test: $(TARGET) $(TEST_FILES)
	$(CC) -o $(TEST_TARGET) $(TEST_FILES) -L$(BIN_DIR) $(BIN_DIR)/libnetioserver.a -I$(INCLUDE_DIR) -I$(HEADER_DIR) -ggdb

run: test
	export LD_LIBRARY_PATH=$(BIN_DIR):$LD_LIBRARY_PATH;$(TEST_TARGET)

gdb: test
	gdb $(TARGET_FILES)

static: $(OBJS)
	ar -r $(BIN_DIR)/libnetioserver.a $(OBJS)
