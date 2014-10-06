OPTS= -O3 
CHARMC=/home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
TEST=graphColor

OBJS = Main.o graph.o Node.o
PROJ= #-tracemode projections 

TARGET = gc
TEST=	graphColor
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CHARMC) -language charm++ -o $(TARGET) $(OBJS) $(PROJ) 


graph.o: graph.cpp 
	g++ -std=c++11 -c graph.cpp

Main.o: Main.cpp Module.decl.h graphColor.h
	$(CHARMC) -c Main.cpp

Node.o: Node.cpp Module.decl.h graphColor.h
	$(CHARMC) -c Node.cpp

Module.decl.h: graphColor.ci
	$(CHARMC) -c graphColor.ci

run: $(TARGET)
	./charmrun +p4 $(TARGET) 30000  ++local
clean:
	rm -f conv-host *.o charmrun $(TARGET)
	rm -f *.def.h *.decl.h
	rm -f *.log.gz *.projrc *.topo *.sts log

