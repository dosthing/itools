CC = gcc
INC_FLAGS := -I../include/
CFLAGS := -Ddebugprintf
CFLAGS += $(INC_FLAGS)
CFLAGS += -lpthread 
CFLAGS += -L../cjsonsrclibcJSON.a 
CFLAGS += -Wall -g -O2
#CFLAGS := -Wall -g -O2 -fPIC 
#-static 
#CFLAGS := -Wall -g  -fPIC #-static 
#LIBS:= -pthread
STRIP:= $(CC)-linux-strip


#找出当前目录下所有的.c文件
CSOURCE:=$(shell find . -type f -name "*.c")
#生成.c文件对应的.o文件
OBJECTS:=$(subst .c,.o,$(CSOURCE))

all:
	$(CC)  $(CSOURCE) $(CFLAGS) -o mydogser
#	$(STRIP) mydogser

	
clean:
	#rm -f  mydogser $(dir)/mydogser