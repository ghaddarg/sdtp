##################################################################################
TESTEXT	= cc
OBJEXT	= o
GAPP 	= mg_gtest
##################################################################################
#                                   GTEST
##################################################################################
GSRCS := \
	src/gtest-all.$(TESTEXT) \
	src/gtest.$(TESTEXT) \
	src/gtest-death-test.$(TESTEXT) \
	src/gtest-filepath.$(TESTEXT) \
	src/gtest_main.$(TESTEXT) \
	src/gtest-port.$(TESTEXT) \
	src/gtest-printers.$(TESTEXT) \
	src/gtest-test-part.$(TESTEXT) \
	src/gtest-typed-test.$(TESTEXT)

GOBJS := $(patsubst %.$(TESTEXT),%.$(OBJEXT),$(GSRCS))

lib_$(GAPP).a: $(GOBJS)
	$(AR) rcs lib_$(GAPP).a $(GOBJS)
	
src/%.o: src/%.$(TESTEXT)
	$(CXX) $(GXXFLAGS) $(CXXFLAGS) -c $^ -o $@
##################################################################################
#                                   GCC FLAGS
##################################################################################
CXXFLAGS := \
	-Wall -Wextra -O0 -g -std=c++14 \
	-Iinclude -I.

GXXFLAGS := \
	-Wno-missing-field-initializers -Wno-sign-compare -Wno-conversion-null
##################################################################################
#                              MAIN MAKEFILE
##################################################################################
.PHONY: all

all: lib_$(GAPP).a

clean:
	rm -rf *.$(OBJEXT) \
		lib_$(GAPP).a \
		src/*.$(OBJEXT)
