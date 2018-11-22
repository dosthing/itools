#CC = arm-hisiv510
#CC = arm-hisiv500
#CC = arm-hisiv400
#CC = arm-hisiv300
#CC = arm-hisiv200
CC = arm-hisiv100nptl

#INCLUDE = -I .
#CFLAGS = -Ddebugprintf
#LIBS:= -pthread
STRIP:= $(CC)-linux-strip

ifeq ($(CC),arm-hisiv100nptl)
dir=../v100/
else ifeq ($(CC),arm-hisiv200)
dir=../v200/
else ifeq ($(CC),arm-hisiv400)
dir=../v400/
else ifeq ($(CC),arm-hisiv500)
dir=../v500/
else ifeq ($(CC),arm-hisiv510)
dir=../v510/
else
dir=../v300/
endif

#arm-hisiv100nptl-linux-gcc daemon.c  -lpthread -D debugprintf -o mydogser
all:
	$(CC)-linux-gcc daemon.c -I . -D debugprintf -lpthread  -o mydogser
	$(STRIP) mydogser
	cp mydogser $(dir)
	
clean:
	rm -f  mydogser $(dir)/mydogser