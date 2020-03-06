EXECUTABLE	:= h5gateserver

SRCEXTS		:=.c .cpp
LIBDIR		:=/usr/lib64 /usr/lib64/mysql ../gamelib 
LIBS		:=pthread dl netbase public
INCLUDES	:=./ ../include  /usr/include/ ../include/public 
SRCDIR		:=./  ../public ../../public
#../publicsdk/transfer 
CC			:=g++
CFLAGS		:= -g   #-Wall -O3  -DDEBUG_BUF_ERR -DDEBUG_SYS_ERR  -DGD_PAI  -DTEST_PEIPAI
CPPFLAGS	:= $(CFLAGS)
CPPFLAGS	+= $(addprefix -I,$(INCLUDES))
CPPFLAGS	+= #-MMD
RM-F		:= rm -f



###########################################################
# # You shouldn't need to change anything below this point.
###########################################################
SRCS	= $(foreach d,$(SRCDIR),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

OBJS	= $(foreach x,$(SRCEXTS), \
			$(patsubst %$(x),%.o,$(filter %$(x),$(SRCS))))


DEPS	= $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))


.PHONY : all deps objs clean veryclean rebuild info

all: $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) $(OBJS)
	@$(RM-F) $(DEPS)

veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean all
ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)
$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))

info:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(MISSING_DEPS)
	@echo $(MISSING_DEPS_SOURCES)

