e1.json
300 vertices, edgeDensity 12
Run with (1 node, 12 PEs):
--grain-size=292 --do-priority=true --num-colors=9
--grain-size=292 --do-priority=false --num-colors=9

e2.json
1000 vertices, edgeDensity 5
Run with (1 node, 12 PEs):
--timeout=10 --newGraph=no --grain-size=960 --do-priority=true --num-colors=5
--timeout=30 --newGraph=no --grain-size=960 --do-priority=true --num-colors=5


example3_v_1000_e_6000_imbalance_subgraphs.json
1000 vertices, edgeDensity 5
chromaticNum=6
Run with (2 node, 16 PEs):
--timeout=5 --newGraph=no --grain-size=960 --do-priority=true
--do-subgraph=true --num-colors=6
result:
Program time (s) = 0.011998
--timeout=5 --newGraph=no --grain-size=960 --do-priority=true
--do-subgraph=false --num-colors=6
result:
Program time (s) = 24.531271

