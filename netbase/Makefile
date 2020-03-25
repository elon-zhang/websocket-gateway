EXES=netbase
B_HOME=.
INCDIR=-I$(B_HOME) ../

OUTDIR=$(B_HOME)/lib

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
OBJS=SYSnotify.$(TO)\
	 SYSthread.$(TO)\
	 SYStimerecord.$(TO)\
	 tcpbaseconnect.$(TO)\
	 tcpbasethread.$(TO)\
	 tcpactiveconnectlower.$(TO)\
	 tcpclientcreatorlower.$(TO)\
	 tcpclientlower.$(TO)\
	 tcpreconnectthreadlower.$(TO)\
	 tcpacceptconnectlower.$(TO)\
	 tcpacceptthreadlower.$(TO)\
	 tcpconnectpoollower.$(TO)\
	 tcpserverlower.$(TO)
	 
all:$(EXES) clean

SYSnotify.$(TO):./sys/SYSnotify.cpp
	$(CC) $(CFLAGS) ./sys/SYSnotify.cpp $(INCDIR)
SYSthread.$(TO):./sys/SYSthread.cpp
	$(CC) $(CFLAGS) ./sys/SYSthread.cpp $(INCDIR)
SYStimerecord.$(TO):./sys/SYStimerecord.cpp
	$(CC) $(CFLAGS) ./sys/SYStimerecord.cpp $(INCDIR)
tcpbaseconnect.$(TO):./tcpbase/tcpbaseconnect.cpp
	$(CC) $(CFLAGS) ./tcpbase/tcpbaseconnect.cpp $(INCDIR)
tcpbasethread.$(TO):./tcpbase/tcpbasethread.cpp
	$(CC) $(CFLAGS) ./tcpbase/tcpbasethread.cpp $(INCDIR)
tcpactiveconnectlower.$(TO):./tcplower/clientlower/tcpactiveconnectlower.cpp
	$(CC) $(CFLAGS) ./tcplower/clientlower/tcpactiveconnectlower.cpp $(INCDIR)
tcpclientcreatorlower.$(TO):./tcplower/clientlower/tcpclientcreatorlower.cpp
	$(CC) $(CFLAGS) ./tcplower/clientlower/tcpclientcreatorlower.cpp $(INCDIR)
tcpclientlower.$(TO):./tcplower/clientlower/tcpclientlower.cpp
	$(CC) $(CFLAGS) ./tcplower/clientlower/tcpclientlower.cpp $(INCDIR)
tcpreconnectthreadlower.$(TO):./tcplower/clientlower/tcpreconnectthreadlower.cpp
	$(CC) $(CFLAGS) ./tcplower/clientlower/tcpreconnectthreadlower.cpp $(INCDIR)
tcpacceptconnectlower.$(TO):./tcplower/serverlower/tcpacceptconnectlower.cpp
	$(CC) $(CFLAGS) ./tcplower/serverlower/tcpacceptconnectlower.cpp $(INCDIR)
tcpacceptthreadlower.$(TO):./tcplower/serverlower/tcpacceptthreadlower.cpp
	$(CC) $(CFLAGS) ./tcplower/serverlower/tcpacceptthreadlower.cpp $(INCDIR)
tcpconnectpoollower.$(TO):./tcplower/serverlower/tcpconnectpoollower.cpp
	$(CC) $(CFLAGS) ./tcplower/serverlower/tcpconnectpoollower.cpp $(INCDIR)
tcpserverlower.$(TO):./tcplower/serverlower/tcpserverlower.cpp
	$(CC) $(CFLAGS) ./tcplower/serverlower/tcpserverlower.cpp $(INCDIR)



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


