include ../../../Make.config.mk

SRCS    := $(wildcard *.cpp)
FEBIRD_INC = ../../../src

ifeq (YES, ${DEBUG})
   LIBS += -lnark-${COMPILER}-d
   OUTPUT = d
else
   LIBS += -lnark-${COMPILER}
   OUTPUT = a
endif

LIBS += -lz -lbz2
#LIBS += -boost_serialization
CXXFLAGS += -Wno-invalid-offsetof

include ../../../Make.rules.mk

