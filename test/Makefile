##################################################################################
SHAREDDIR	= shared

GSRCDIR = ../$(SHAREDDIR)/gtest
APP		= mg_test
GTESTLIB = mg_gtest

SRCEXT	= cpp
OBJEXT  = o
##################################################################################
#                                   SQLITE
##################################################################################
CPPSRCS := \
	../$(SHAREDDIR)/cmd_parser.$(SRCEXT)

OBJS := $(CPPSRCS:.$(SRCEXT)=.$(OBJEXT))

%.o: %.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
##################################################################################
#                                   GCC FLAGS
##################################################################################
CXXFLAGS := \
	-fexceptions -Wall -Wextra -Werror -O0 -g -std=c++14 \
	-I../$(SHAREDDIR) \
	-I$(GSRCDIR) \
	-I$(GSRCDIR)/include

LDFLAGS := -lpthread -ldl
##################################################################################
#                              MAIN MAKEFILE
##################################################################################
.PHONY: all
all: $(APP)

lib$(APP).a: $(OBJS)
	$(AR) rcs lib$(APP).a $(OBJS)

$(APP): build_gtest lib$(APP).a
	$(CXX) $(CXXFLAGS) $(APP).cc -o $(APP) lib$(APP).a $(GSRCDIR)/lib_$(GTESTLIB).a $(LDFLAGS)

check:
	./$(APP)

clean: clean_gtest
	rm  -f $(APP) lib$(APP).a \
		../$(SHAREDDIR)/*.o 
##################################################################################
#                              GTEST
##################################################################################
build_gtest:
	$(call make-gtest)
clean_gtest:
	$(call clean-gtest)
##################################################################################
#                              DEFINES
##################################################################################
define make-gtest
	cd $(GSRCDIR); \
	make;
endef
define clean-gtest
	cd $(GSRCDIR); \
	make clean;
endef