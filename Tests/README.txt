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
