CC = g++

TARGET = libbrq.so.1.0
SONAME = libbrq.so.1
LINK = libbrq.so

SOURCE_DIR = ./source
HEADER_DIR = ./header
BIN_DIR = ./bin
OBJ_DIR = ./obj

CPPFLAGS += -W -Wall -Wundef -ggdb -fPIC -Wno-non-virtual-dtor

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.cpp)
HEADER_FILES := $(wildcard $(HEADER_DIR)/*.h)
OBJS := $(patsubst %.cpp,%.o,$(subst $(SOURCE_DIR),$(OBJ_DIR),$(SOURCE_FILES)))
SOS := $(patsubst %.cpp,%.so,$(subst $(SOURCE_DIR),$(SO_DIR),$(SOURCE_FILES)))
TARGET_FILES := $(BIN_DIR)/$(TARGET)

TEST_DIR = ./test
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

.PHONY: all clean a test

all: $(OBJS)
	$(CC) $(CPPFLAGS) -fPIC -shared -export-dynamic -o $(TARGET_FILES) $(OBJS)
	cd $(BIN_DIR);ln -sf $(TARGET) $(SONAME);ln -sf $(TARGET) $(LINK)

$TARGET: $(OBJS)
	$(CC) -o $(TARGET_FILES) $(OBJS) -I$(HEADER_DIR) $(CPPFLAGS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADER_DIR)/%.h
	$(CC) -c $(CPPFLAGS) -I$(HEADER_DIR) $(CFLAGS) -o $@ $<

clean:
	@rm $(OBJS) $(TARGET_FILES) -f

static: $(OBJS)
	ar r $(BIN_DIR)/libbrq.a $(OBJS)

test:
	$(CC) -o $(BIN_DIR)/test $(TEST_FILES) -I$(HEADER_DIR)/ -I../ $(TARGET_FILES) -ggdb
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/so/;./bin/test

gdb:
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/so/;gdb ./bin/test
