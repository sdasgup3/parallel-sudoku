OPTS= -O3 -std=c++11
CARMC=/home/acun2/charm/net-linux-x86_64/bin/charmc $(OPTS)
TEST=graphColor
INC=-I${ROOT}/python_kit/intel64/include/python2.7
LDFLAGS=-lboost_program_options -lpython2.7 -L${ROOT}/python_kit/intel64/lib

OBJS = Main.o  Node.o Utils.o
PROJ= #-tracemode projections 

TARGET = gc
TEST=	graphColor
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CHARMC) $(LDFLAGS) -language charm++ -o $(TARGET) $(OBJS) $(PROJ) 

Main.o: Main.cpp Module.decl.h graphColor.h vertex.h 
	$(CHARMC) $(INC) -c Main.cpp

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

