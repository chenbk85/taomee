CC = g++

TARGET = libdemo.so
SRC_DIR = .
OBJ_DIR = ./objs
BIN_DIR = .
CPPFLAGS += -I./ -I../ -Wall -W -fPIC -shared -ggdb -DDEBUG

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET_FILE = $(BIN_DIR)/$(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET_FILE) $(OBJS) $(CPPFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) -o $@ $<

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

.PHONY: clean

clean:
	rm -f $(TARGET_FILE) $(OBJ_DIR)/*
