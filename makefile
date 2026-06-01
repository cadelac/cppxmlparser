TARGET = appl
SRCS = Document.C Lex.C Parser.C System.C Lockable.C Log.C Time.C Main.C  
INCLUDES =
INCLUDES += -I/usr/lib/gcc/i486-linux-gnu/4.1.2/include
INCLUDES += -I/usr/include/c++/4.1.2/i486-linux-gnu
INCLUDES += -I/usr/include/c++/4.1
#INCLUDES += -I/usr/include/linux
INCLUDES += -I.
LFLAGS =
LIBS = -lpthread

#
#

CC = g++
# __i386__ or __x86_64__
#-D_THREAD_SAFE -D__i386__ 
CFLAGS = -Wall -g -pthread -DMULTITHREADED -DCONSOLE_OFF 
BUILD_DIR = build
#OBJS = $(SRCS:.C=.o)
OBJS = $(patsubst %.C, $(BUILD_DIR)/%.o,  $(SRCS))

#
#

.PHONY: depend clean

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)


-include $(SRCS:.C=.d)
#-include $(patsubst %.C, %.d,  $(SRCS))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LFLAGS) $(LIBS)

%.d: %.C 
	@echo "building dependencies for [$<]"
	@set -e; $(CC) -MM $(CFLAGS) $<	| sed 's/\($*\)\.o[ :]*/\1.o $*.d : /g' > $*.d; \
		[ -s $*.d ] || rm -f $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(BUILD_DIR)/%.o : %.C %.d
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) *.d *~ 
