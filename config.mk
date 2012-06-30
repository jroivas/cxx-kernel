# General build configuration

#ifeq (,$(filter _%,$(notdir $(CURDIR))))
#include target.mk
ifneq (all,$(ARCH))
#CURDIR=$(dir $(lastword $(MAKEFILE_LIST)))/
#$(error joope $(CURDIR) $(dir $(lastword $(MAKEFILE_LIST))) )
#ifneq ($(findstring $(CURDIR)config_$(ARCH).mk, $(wildcard *.mk)),)
#$(error found $(CUDRDIR)config_$(ARCH).mk)
CFILE=$(TOPDIR)/config_$(ARCH).mk
#CFILE=$(CURDIR)/config_$(ARCH).mk
#include $(CUDRDIR)config_$(ARCH).mk
#ifneq ($(findstring $(CFILE), $(wildcard $(CURDIR)*.mk)),)
ifneq ($(findstring $(CFILE), $(wildcard $(TOPDIR)/*.mk)),)
include $(CFILE)
endif
endif

#else
#VPATH = $(SRCDIR)

#endif
