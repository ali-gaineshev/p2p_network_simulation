# Tests
Run each test 5 times 


## 3_regular_10_nodes

**nodes 0 to 9 distance is 2**

default TTL = 1

TTL increases by 1

***
Flood: 

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_10_nodes --ttl=1"
```

***

Random Walk:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_10_nodes --ttl=1 --walkers=2"
```


***
Normalized Flood: 

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_10_nodes --ttl=1 --walkers=2"
```


----------------------------------------------------
## 3_regular_50_nodes

**nodes 0 to 9 distance is 7**

ttl - 5

ttl increases by 5


***
Flood: 

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/flood"
```


***
Random Walk:

walkers - 5

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/random_walk --walkers=5"
```

***
Normalized Flood: 

max walkers - 2 

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/normalized_flood --walkers=2"
```

---------------------------------
5_regular_200_nodes

**nodes 0 to 197 distance is 5**

ttl - 3

***
Flood: 

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/5_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=5_regular_with_200_nodes/flood"
```

***
Random Walk:

walkers - 5

***
Normalized Flood: 

max nodes - 3

---------------------------------
4_regular_200_nodes

**nodes 0 to 169 distance is 5**

ttl - 2

***
Flood: 

***
Random Walk:

walkers - 5

***
Normalized Flood: 

max nodes - 2

---------------------------------
tree with 80 nodes

nodes 0 to 79 distance is 5

Flood: 
ttl - 3

Random Walk:
ttl - 3
walkers - 3

Normalized Flood: 
ttl - 3
max nodes - 1

---------------------------------
cluster_6_wit_4_nodes_each

nodes 0 to 2 distance is 1

Flood: 
ttl - 3

Random Walk:
ttl - 3
walkers - 3

Normalized Flood: 
ttl - 3
max nodes - 3

---------------------------------
megagraph_3_clusters



Flood: 
ttl - 3

Random Walk:
ttl - 3
walkers - 3

Normalized Flood: 
ttl - 3
max nodes - 2


---------------------------------
megagraph_5_clusters


Flood: 
ttl - 3

Random Walk:
ttl - 3
walkers - 5

Normalized Flood: 
ttl - 3
max nodes - 3
