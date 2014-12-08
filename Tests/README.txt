e1.json (PRIORITY)
300 vertices, edgeDensity 12
Run with (1 node, 12 PEs):
--grain-size=292 --do-priority=true --num-colors=9
--grain-size=292 --do-priority=false --num-colors=9

e2.json (TIMEOUT)
1000 vertices, edgeDensity 5
Run with (1 node, 12 PEs):
--timeout=10 --newGraph=no --grain-size=960 --do-priority=true --num-colors=5
--timeout=30 --newGraph=no --grain-size=960 --do-priority=true --num-colors=5

e3.json (STATIC GRAIN SIZE)
300 vertices, edgeDensity 8
Run with (1 node, 12 PEs):
--timeout=200 --newGraph=no --grain-size=300 --do-priority=true --num-colors=4
--timeout=200 --newGraph=no --grain-size=290 --do-priority=true --num-colors=4
--timeout=200 --newGraph=no --grain-size=280 --do-priority=true --num-colors=4
--timeout=200 --newGraph=no --grain-size=260 --do-priority=true --num-colors=4

e4.json (VALUE ORDERING)
500 vertices, edgeDensity 7
Run with (1 node, 12 PEs):
--timeout=10 --newGraph=no --grain-size=480 --do-priority=true --num-colors=6
To remove value ordering, comment out following part in getNextValueOrdering
in node.cpp and stackNode.cpp : 
rank = rank; // + count;

****************************************************************************
***  Effect of having AND chares or handling independent subgraphs
*****************************************************************************
example3_v_1000_e_6000_imbalance_subgraphs.json
1000 vertices, edgeDensity 5
chromaticNum=6
Run with (2 node, 16 PEs):
--timeout=5 --newGraph=no --grain-size=960 --do-priority=true
--do-subgraph=true --num-colors=6
result:
Program time (s) = 0.011998, 0.009998, 0.011999
--timeout=5 --newGraph=no --grain-size=960 --do-priority=true
--do-subgraph=false --num-colors=6
result:
Program time (s) = 24.531271, 42.253576, killed due to memory*2, 35.293634

  
e4.json (KILL chase for successful OR node)
500 vertices, edgeDensity 7
Run with (1 node, 12 PEs):
--timeout=10 --newGraph=no --grain-size=480 --do-priority=true --num-colors=6
To remove the feature , do the following modification in graphColor.h
//if(acceptRegistration && false == amIChildOfFalseAndNode)  
if(false == amIChildOfFalseAndNode)  

Results:
With:
Start: 14:55:05  End:15:01:09: Program time= 167.853483: Kill chase time: (End
           - start - Program Time) = 196.146517
Without:
Start: 15:02:59 End: 15:06:23: Program time (s) = 95.908420:
Kill Chase Time: 108.091580



