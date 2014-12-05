
COMPILER := $(shell ${CXX} --version | head -1 | awk '{split($$3, Ver, "."); printf("%s-%d.%d", $$1, Ver[1], Ver[2]);}')

#override CXXFLAGS += -I ../../../src

CompileSpeedFebird : CompileSpeedFebird.cpp
	    ${CXX} ${CXXFLAGS} -time -I../../../src -L../../../lib -lnark-${COMPILER}-d -lboost_system${BOOST_SUFFIX} -o $@ $^

