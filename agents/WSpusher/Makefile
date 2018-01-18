UNAME := $(shell uname)

ifeq ($(ARCH), arm)
        include arm.mk
else
	ifeq ($(UNAME), Linux)
        	include debian.mk
	endif
	ifeq ($(UNAME), Darwin)
        	include macosx.mk
	endif
endif
