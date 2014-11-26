
FEBIRD_HOME = ../../..

ifeq "$(origin CXX)" "default"
  ifeq "$(shell test -e /opt/bin/g++ && echo 1)" "1"
    CXX := /opt/bin/g++
  else
    ifeq "$(shell test -e ${HOME}/opt/bin/g++ && echo 1)" "1"
      CXX := ${HOME}/opt/bin/g++
    endif
  endif
endif

ifeq "$(origin LD)" "default"
  LD := ${CXX}
endif

override INCS := -I${FEBIRD_HOME}/src ${INCS}
#override CXXFLAGS += -pipe
override CXXFLAGS += -std=gnu++11
override CXXFLAGS += -Wall -Wextra
override CXXFLAGS += -Wno-unused-parameter
#override CXXFLAGS += -Wno-unused-variable
#CXXFLAGS += -Wconversion -Wno-sign-conversion

override CXXFLAGS += -Wfatal-errors

ifneq ($(shell uname), Darwin)
  override LIBS += -lrt
endif
ifeq ($(shell expr substr `uname` 1 6), CYGWIN)
	DLL_EXT = .dll.a
else
	DLL_EXT = .so
endif

COMPILER := $(shell ${CXX} --version | head -1 | awk '{split($$3, Ver, "."); printf("%s-%d.%d", $$1, Ver[1], Ver[2]);}')
UNAME_MachineSystem := $(shell uname -m -s | sed 's:[ /]:-:g')

ifeq "$(shell expr substr ${COMPILER} 1 5)" "clang"
  override CXXFLAGS += -fcolor-diagnostics
endif

ifeq "$(shell expr substr ${COMPILER} 1 3)" "g++"
  override CXXFLAGS += -time
  override CXXFLAGS += -fmax-errors=5
  #override CXXFLAGS += -fmax-errors=2
endif

# icc or icpc
ifeq "$(shell expr substr ${COMPILER} 1 2)" "ic"
  override CXXFLAGS += -xHost -fasm-blocks
else
  override CXXFLAGS += -march=native
endif

BUILD_DIR := build/${COMPILER}-${UNAME_MachineSystem}

DBG_DIR := ${BUILD_DIR}/dbg
RLS_DIR := ${BUILD_DIR}/rls

SRCS += $(wildcard *.cpp)
OBJS_R := $(addprefix ${RLS_DIR}/, $(addsuffix .o, $(basename ${SRCS})))
OBJS_D := $(addprefix ${DBG_DIR}/, $(addsuffix .o ,$(basename ${SRCS})))
BINS_D := $(addsuffix .exe ,$(basename ${OBJS_D}))
BINS_R := $(addsuffix .exe ,$(basename ${OBJS_R}))

ext_ldflags = $(strip $(shell sed -n 's,//Makefile\s*:\s*LDFLAGS\s*:\s*\(.*\),\1,p' $(subst .exe,.cpp,$(subst ${RLS_DIR}/,,$(subst ${DBG_DIR}/,,$@)))))
ext_cxxflags = $(strip $(shell sed -n 's,//Makefile\s*:\s*CXXFLAGS\s*:\s*\(.*\),\1,p' $<))

.PHONY : all clean link

all : ${BINS_D} ${BINS_R} ${OBJS_D} ${OBJS_R} link

link : ${BINS_D} ${BINS_R}
	mkdir -p dbg; cd dbg; \
	for f in `find ../${DBG_DIR} -name '*.exe'`; do \
		ln -sf $$f .; \
	done; cd ..
	mkdir -p rls; cd rls; \
	for f in `find ../${RLS_DIR} -name '*.exe'`; do \
		ln -sf $$f .; \
	done; cd ..

ifeq (${STATIC},1)
${BINS_D} : ${FEBIRD_HOME}/lib/libnark-d.a
${BINS_R} : ${FEBIRD_HOME}/lib/libnark-r.a
${BINS_D} : LIBS := -Wl,--whole-archive ${FEBIRD_HOME}/lib/libnark-d.a -Wl,--no-whole-archive ${LIBS}
${BINS_R} : LIBS := -Wl,--whole-archive ${FEBIRD_HOME}/lib/libnark-r.a -Wl,--no-whole-archive ${LIBS}
else
${BINS_D} : ${FEBIRD_HOME}/lib/libnark-d${DLL_EXT}
${BINS_R} : ${FEBIRD_HOME}/lib/libnark-r${DLL_EXT}
${BINS_D} : LIBS := -L${FEBIRD_HOME}/lib -lnark-d ${LIBS}
${BINS_R} : LIBS := -L${FEBIRD_HOME}/lib -lnark-r ${LIBS}
endif

clean :
	rm -rf ${BUILD_DIR} dbg rls

${DBG_DIR}/%.o : %.cpp
	@mkdir -p $(dir $@)
	${CXX} -O0 -g3 -c ${INCS} ${CXXFLAGS} -o $@ $< $(ext_cxxflags)

#${RLS_DIR}/%.o : CXXFLAGS += -funsafe-loop-optimizations -fgcse-sm -fgcse-las -fgcse-after-reload
${RLS_DIR}/%.o : %.cpp
	@mkdir -p $(dir $@)
	${CXX} -Ofast -c ${INCS} ${CXXFLAGS} -o $@ $< $(ext_cxxflags) -DNDEBUG

%.exe : %.o
	@echo Linking ... $@
	${LD} ${LDFLAGS} -o $@ $< ${LIBS} $(ext_ldflags)
