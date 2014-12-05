
COMPILER := $(shell ${CXX} --version | head -1 | awk '{split($$3, Ver, "."); printf("%s-%d.%d", $$1, Ver[1], Ver[2]);}')

CompileSpeedBoost : CompileSpeedBoost.cpp
	    ${CXX} -time -lboost_serialization -o $@ $^
