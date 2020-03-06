EXES=public
B_HOME=.
INCDIR=-I$(B_HOME) ./json ./share ./

OUTDIR=./lib

TO=o
#LIB_PRE=lib
#LIB_EXT=.so
LIB_PRE=lib
LIB_EXT=.a

RM=rm -f
AR=ar
CC=g++
LD=$(CC)

CFLAGS = -c -g -Wall -fPIC $(DEFS)
#LFLAGS = -shared
LFLAGS = crv
LIBPATH=-L$(B_HOME)/lib

STDLIBS=-lpthread -lc -lnsl -ldl -lm -lstdc++ -lrt

RMOBJ = -$(RM) *.$(TO)
OBJS=base64.$(TO)\
	 config.$(TO)\
	 configpars.$(TO)\
	 jsoncpp.$(TO)\
	 log.$(TO)\
	 mongoose.$(TO)\
	 publicthread.$(TO)\
	 sha1.$(TO)\
	 SpecilString.$(TO)
	 
all:$(EXES) clean

base64.$(TO):./base64.cpp
	$(CC) $(CFLAGS) ./base64.cpp $(INCDIR)
config.$(TO):./config.cpp
	$(CC) $(CFLAGS) ./config.cpp $(INCDIR)
configpars.$(TO):./configpars.cpp
	$(CC) $(CFLAGS) ./configpars.cpp $(INCDIR)
jsoncpp.$(TO):./jsoncpp.cpp
	$(CC) $(CFLAGS) ./jsoncpp.cpp $(INCDIR)
log.$(TO):./log.cpp
	$(CC) $(CFLAGS) ./log.cpp $(INCDIR)
mongoose.$(TO):./mongoose.cpp
	$(CC) $(CFLAGS) ./mongoose.cpp $(INCDIR)
publicthread.$(TO):./publicthread.cpp
	$(CC) $(CFLAGS) ./publicthread.cpp $(INCDIR)
sha1.$(TO):./sha1.cpp
	$(CC) $(CFLAGS) ./sha1.cpp $(INCDIR)
SpecilString.$(TO):./SpecilString.cpp
	$(CC) $(CFLAGS) ./SpecilString.cpp $(INCDIR)




$(EXES):$(OBJS)
	$(AR) $(LFLAGS) $(OUTDIR)/$(LIB_PRE)$(EXES)$(LIB_EXT) $(OBJS)
cleanobj:
	$(RMOBJ)
clean:cleanobj
	@echo clean all
	$(RMALL)

#install:
	#cp -rf $(B_HOME)/include/*  /usr/local/include/
	#cp -rf $(B_HOME)/lib/*      /usr/local/lib/


