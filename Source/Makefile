OPTS= -O3 -std=c++11
#CHARMC=/home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
CHARMC=/home/gangwan2/cs598svn/gangwan2/testbed/local_build/net-linux-x86_64-smp/bin/charmc $(OPTS) 
TEST=graphColor

OBJS = Main.o  Node.o Utils.o
PROJ= #-tracemode projections 

TARGET = gc
TEST=	graphColor
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CHARMC) -language charm++ -o $(TARGET) $(OBJS) $(PROJ) 


Main.o: Main.cpp Module.decl.h graphColor.h vertex.h 
	$(CHARMC) -c Main.cpp

Node.o: Node.cpp Module.decl.h graphColor.h
	$(CHARMC) -c Node.cpp

Utils.o: Utils.cpp Utils.h vertex.h
	$(CHARMC)  -c  Utils.cpp

Module.decl.h: graphColor.ci
	$(CHARMC) -c graphColor.ci

run: $(TARGET)
	./charmrun +p4 $(TARGET) ../Tests/inputG_1.txt  ++local
clean:
	rm -f conv-host *.o charmrun $(TARGET)
	rm -f *.def.h *.decl.h
	rm -f *.log.gz *.projrc *.topo *.sts log

