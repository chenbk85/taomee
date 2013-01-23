CC = g++

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = libstubserver.so
CPPFLAGS += -I./include -fPIC -g -Wall -DDEBUG
TARGET_FLAGS = -shared -L./lib -lmysqlclient -lssl -lrt

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET_FILE = $(BIN_DIR)/$(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET_FILE) $(OBJS) $(TARGET_FLAGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

.PHONY : all clean gdb

all: $(TARGET)
	ctags -R ./

clean:
	rm -f $(TARGET_FILE) $(OBJS) $(OBJ_DIR)/*.d
