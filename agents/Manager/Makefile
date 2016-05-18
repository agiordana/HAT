UNAME := $(shell uname)

ifeq ($(ARCH), arm)
        include arm.mk
endif

ifeq ($(ARCH), rpi)
	include rpi.mk
else
	ifeq ($(UNAME), Linux)
        	include debian.mk
	endif
	ifeq ($(UNAME), Darwin)
        	include macosx.mk
	endif
endif
