CXX	= g++
AR	= ar
##################################################################################
SRCDIR	= src
INCDIR	= inc
LIBDIR	= lib
TESTDIR	= test
SHARED	= shared
APP		= mg_server

GSRCDIR	= ../googletest/googletest

MKDIR	= mkdir -p
OUTDIR	= exe

SRCEXT	= cpp
OBJEXT	= o
##################################################################################
#                                   SQLITE
##################################################################################
CPPSRCS := \
	$(SRCDIR)/ftp_server.$(SRCEXT) \
	../$(SHARED)/cmd_parser.$(SRCEXT) \

OBJS := $(CPPSRCS:.$(SRCEXT)=.$(OBJEXT))

%.o: %.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
##################################################################################
#                                   GXX FLAGS
##################################################################################
CXXFLAGS := \
	-fexceptions -Wall -Wextra -Werror -O0 -g -std=c++11 \
	-I$(INCDIR) -I../$(SHARED)

LDFLAGS := 
##################################################################################
#                              MAIN MAKEFILE
##################################################################################
.PHONY: all
all: dir $(APP)

lib$(APP).a: $(OBJS)
	$(AR) rcs $(LIBDIR)/lib$(APP).a $(OBJS)

$(APP): lib$(APP).a
	$(CXX) $(CXXFLAGS) $@.cpp -o $(OUTDIR)/$@ $(LIBDIR)/lib$(APP).a $(LDFLAGS)

dir: dir_lib
	${MKDIR} ${OUTDIR}

dir_lib:
	${MKDIR} ${LIBDIR}

check:
	./$(OUTDIR)/$(APP)

clean: 
	rm -rf $(OUTDIR) \
		-rf ${LIBDIR} \
		*.$(OBJEXT) \
		lib$(APP).a \
		$(SRCDIR)/*.$(OBJEXT) \
		../$(SHARED)/*.$(OBJEXT)
