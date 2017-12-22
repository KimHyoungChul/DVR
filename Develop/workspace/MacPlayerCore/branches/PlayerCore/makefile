include platforminc.h

#make GDB=1 for debug
ifeq ($(GDB),1)
	CFLAGS	+= -g
	CFLAGS	+= -DUBLOG_DEBUG
endif

OUTPUT = .

BIN_PATH 	+= $(OUTPUT)/bin/
LIB_PATH 	+= $(OUTPUT)/lib/
LOCAL_PATH	= 
MY_COREPATH	= ./core

INCLUDES 	+=					\
		-I$(MY_COREPATH)/services/control	\
		-I$(MY_COREPATH)/services/common	\
		-I$(MY_COREPATH)/services/videostream	\
		-I$(MY_COREPATH)/UI


MY_CORE_SERVICE_SRC_FILES = 						\
		$(MY_COREPATH)/services/control/srv_rc_pool.c		\
		$(MY_COREPATH)/services/videostream/srv_vs_file.c	\
		$(MY_COREPATH)/services/videostream/srv_vs_vms.c	\
		$(MY_COREPATH)/services/common/srv_com_handler.c	\
		$(MY_COREPATH)/services/common/up_sys_log.c		\
		$(MY_COREPATH)/services/common/up_sys_sock.c		\
		$(MY_COREPATH)/services/common/up_sys_util.c		\
		$(MY_COREPATH)/services/common/util_circle_list.c

MY_CORE_UI_SRC_FILES = 					\
		$(MY_COREPATH)/UI/up_ui_context.c	\
		$(MY_COREPATH)/UI/up_ui_core.c
 
LOCAL_SRC_FILES =	$(MY_CORE_SERVICE_SRC_FILES)
LOCAL_SRC_FILES +=	$(MY_CORE_UI_SRC_FILES)

LIBS		+=
#CFLAGS 		+=-Wall -g -Wshadow -lpthread  -O3 $(INCLUDES) $(LIBS) -D__LINUX_32__
#CFLAGS 		+=-Wall -g -Wshadow -lpthread  $(INCLUDES) $(LIBS) -D__LINUX_32__
CFLAGS 		+=-Wall -Wshadow -lpthread  $(INCLUDES) $(LIBS) -D__LINUX_32__

#MAIN_SOURCES = $(wildcard src/*.cpp) $(wildcard src/*.c)
#MAIN_OBJS = $(addsuffix .o, $(basename $(MAIN_SOURCES)))
#MAIN_DEPS = $(addsuffix .d, $(basename $(MAIN_SOURCES)))

UBUTILDLL 		= $(LIB_PATH)libPlayerCore.a
UBUTIL_SOURCES = $(LOCAL_SRC_FILES)
UBUTIL_OBJS = $(addsuffix .o, $(basename $(UBUTIL_SOURCES)))
UBUTIL_DEPS = $(addsuffix .d, $(basename $(UBUTIL_SOURCES)))

all: domkdir $(PROGRAM) $(UBUTILDLL) 

domkdir:
		@mkdir -p $(BIN_PATH)
		@mkdir -p $(LIB_PATH)

$(UBUTILDLL): $(UBUTIL_OBJS)
		$(AR) rcu $@ $(UBUTIL_OBJS)

#$(PROGRAM): $(MAIN_OBJS)   $(UBUTILDLL)
#		$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) $(UBUTILDLL) 


include $(MAIN_DEPS)
include $(UBUTIL_DEPS)

%.d: %.cpp
		$(CPP) -MM $(CFLAGS) $< > $@.$$$$; \
		sed 's,^$(*F)\.o[ :]*,$*.o $@ : ,g' <$@.$$$$ > $@; \
		rm -rf $@.$$$$ 

%.o: %.cpp
		$(CPP) -c $(CFLAGS) $< -o $@

%.d: %.c
		$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
		sed 's,^$(*F)\.o[ :]*,$*.o $@ : ,g' <$@.$$$$ > $@; \
		rm -rf $@.$$$$
 
%.o: %.c
		$(CC) -c $(CFLAGS) $< -o $@

clean:
		-rm $(MAIN_OBJS)
		-rm $(UBUTIL_OBJS)
		-rm $(MAIN_DEPS)
		-rm $(UBUTIL_DEPS)

distclean: clean
		-rm  $(PROGRAM)
		-rm -fr $(LIB_PATH)
