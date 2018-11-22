#SUBDIRS := $(shell ls | grep Lib)
#SUBDIRS += alarmLib
SUBDIRS += mydogsrc


all: inc $(SUBDIRS)
#	@make -C demo
#	$(STRIP) demo/demo
#	$(STRIP) lib/$(LibPth)/*.so
#	cp demo/demo /home/rong/lishuang/hi3536cpoe

inc:
#	@$(foreach SUBDIR, $(SUBDIRS), cp $(SUBDIR)/$(SUBDIR).h ./include/ ;)

$(SUBDIRS):
	@make -C $@ 
#@make -C $@ 1>/dev/NULL

clean:
	@$(foreach SUBDIR, $(SUBDIRS), make clean -C $(SUBDIR) clean > /dev/NULL ;)
	#@$(RM) lib/$(HIARCH)/*
	@make -C demo clean > /dev/NULL

.PHONY: all clean $(SUBDIRS)

