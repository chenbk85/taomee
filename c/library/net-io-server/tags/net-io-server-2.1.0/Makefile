CC = g++

CPPFLAGS += -I$(HEADER_DIR) -I$(INCLUDE_DIR) -W -Wall -Wno-non-virtual-dtor -Wundef -ggdb -shared -fPIC

TARGET = libnetioserver.so.2.0
SONAME = libnetioserver.so.2
LINK = libnetioserver.so

STATIC_TARGET = libnetioserver.a
STATIC_TARGET_FILES = $(BIN_DIR)/$(STATIC_TARGET)

SOURCE_DIR = ./src/source
HEADER_DIR = ./src/header
INCLUDE_DIR = ./include
BIN_DIR = ./bin
OBJ_DIR = ./obj

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(HEADER_DIR)/*.h)
INCLUDE_FILES := $(wildcard $(INCLUDE_DIR)/*.h)
OBJS := $(patsubst %.cpp,%.o,$(subst $(SOURCE_DIR),$(OBJ_DIR),$(SOURCE_FILES)))
DS := $(OBJS:.o=.d)
TARGET_FILES := $(BIN_DIR)/$(TARGET)

TEST_DIR = ./test
TEST_TARGET = $(BIN_DIR)/test
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

DEMO_DIR = ./demo
DEMO_TARGET = $(DEMO_DIR)/bin/simple_file_server

$(TARGET): $(DS) $(OBJS)
	$(CC) -o $(TARGET_FILES) -Wl,-soname,$(SONAME) $(OBJS) $(CPPFLAGS)
	@rm -f $(BIN_DIR)/$(LINK) $(BIN_DIR)/$(SONAME)
	@cd $(BIN_DIR);ln -s $(TARGET) $(LINK);ln -s $(TARGET) $(SONAME)

-include $(DS)

$(OBJ_DIR)/%.d: $(SOURCE_DIR)/%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

.PHONY: clean o so test run static demo demo_run

clean:
	-rm $(OBJ_DIR)/* $(TARGET_FILES) $(TEST_TARGET) $(DEMO_TARGET) -rf

static: $(OBJS)
	ar -r $(BIN_DIR)/libnetioserver.a $(OBJS)

o: $(OBJS)
	@echo "only generate obj files!"

test: $(TARGET) $(TEST_FILES)
	$(CC) -o $(TEST_TARGET) $(TEST_FILES) ./bin/libnetioserver.so -I$(INCLUDE_DIR) -I$(HEADER_DIR) -I$(TEST_DIR) -ggdb

demo: $(DEMO_DIR)/*.cpp $(DEMO_DIR)/*.h
	$(CC) -o $(DEMO_TARGET)  $(DEMO_DIR)/*.cpp -I$(INCLUDE_DIR) -I$(HEADER_DIR) -I$(DEMO_DIR) $(BIN_DIR)/libnetioserver.a -ggdb

demo_run:
	export LD_LIBRARY_PATH=$(DEMO_DIR)/bin:$LD_LIBRARY_PATH;$(DEMO_TARGET) 127.0.0.1 3333

run: test
	export LD_LIBRARY_PATH=$(BIN_DIR):$LD_LIBRARY_PATH;$(TEST_TARGET)

gdb: test
	export LD_LIBRARY_PATH=$(BIN_DIR):$LD_LIBRARY_PATH;gdb $(TEST_TARGET)
