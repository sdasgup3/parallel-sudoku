OPTS= -O3 -std=c++11
CHARMC=/home/sdasgup3/CS598LVK/charm/net-linux-x86_64/bin/charmc $(OPTS)
TEST=graphColor

OBJS = Main.o  Node.o Utils.o
PROJ= #-tracemode projections 

TARGET = gc
TEST=	graphColor
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CHARMC) -language charm++ -o $(TARGET) $(OBJS) $(PROJ) 


Main.o: Main.cpp Module.decl.h graphColor.h State.h 
	$(CHARMC) -c Main.cpp

Node.o: Node.cpp Module.decl.h graphColor.h
	$(CHARMC) -c Node.cpp

Utils.o: Utils.cpp Utils.h State.h
	$(CHARMC)  -c  Utils.cpp

Module.decl.h: graphColor.ci
	$(CHARMC) -c graphColor.ci

run: $(TARGET)
	./charmrun +p4 $(TARGET) ../Tests/inputG_1.txt  ++local
clean:
	rm -f conv-host *.o charmrun $(TARGET)
	rm -f *.def.h *.decl.h
	rm -f *.log.gz *.projrc *.topo *.sts log

