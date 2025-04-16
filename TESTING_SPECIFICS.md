# Tests

Run each test 10 times

To disable nodes add

```
--disabled=1
```

# 3_regular_10_nodes

**nodes 0 to 9 distance is 2**

default TTL = 1

TTL increases by 1

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes/3_regular_with_10_nodes.txt --srcIndex=0 --sinkIndex=9 --ttl=1 --ttl_increase=1 --outputFolder=3_regular_10_nodes/flood"
```

---

Random Walk:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes/3_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --ttl=1 --ttl_increase=1 --walkers=2 --outputFolder=3_regular_10_nodes/random_walk"
```

---

Normalized Flood:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_10_nodes/3_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --ttl=1 --ttl_increase=1 --walkers=2 --outputFolder=3_regular_10_nodes/normalized_flood"
```

---

# 3_regular_20_nodes

**nodes 0 to 4 distance is 5**

default TTL = 5

TTL increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_20_nodes/3_regular_with_20_nodes.txt  --srcIndex=0 --sinkIndex=4 --outputFolder=3_regular_20_nodes/flood"
```

---

Random Walk:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_20_nodes/3_regular_with_20_nodes.txt  --srcIndex=0 --sinkIndex=4 --outputFolder=3_regular_20_nodes/random_walk --walkers=2"
```

---

Normalized Flood:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_20_nodes/3_regular_with_20_nodes.txt  --srcIndex=0 --sinkIndex=4 --outputFolder=3_regular_20_nodes/normalized_flood --walkers=2"
```

---

# 3_regular_30_nodes

**nodes 0 to 18 distance is 5**

default TTL = 5

TTL increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_30_nodes/3_regular_with_30_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_30_nodes/flood"
```

---

Random Walk:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_30_nodes/3_regular_with_30_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_30_nodes/random_walk --walkers=2"
```

---

Normalized Flood:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_30_nodes/3_regular_with_30_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_30_nodes/normalized_flood --walkers=2"
```

---

# 3_regular_40_nodes

**nodes 0 to 18 distance is 5**

default TTL = 5

TTL increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_40_nodes/3_regular_with_40_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_40_nodes/flood"
```

---

Random Walk:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_40_nodes/3_regular_with_40_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_40_nodes/random_walk --walkers=2"
```

---

Normalized Flood:

walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_40_nodes/3_regular_with_40_nodes.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=3_regular_40_nodes/normalized_flood --walkers=2"
```

---

# 3_regular_50_nodes

**nodes 0 to 9 distance is 7**

ttl - 5

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/flood"
```

---

Random Walk:

walkers - 5

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/random_walk --walkers=5"
```

---

Normalized Flood:

max walkers - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/3_regular_with_50_nodes/3_regular_with_50_nodes.txt  --srcIndex=0 --sinkIndex=9 --outputFolder=3_regular_50_nodes/normalized_flood --walkers=2"
```

---

# 4_regular_200_nodes

**nodes 0 to 169 distance is 5**

ttl - 2

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/4_regular_with_200_nodes/4_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=169 --outputFolder=4_regular_200_nodes/flood --ttl=2"
```

---

Random Walk:

walkers - 5

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/4_regular_with_200_nodes/4_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=197 --outputFolder=4_regular_200_nodes/random_walk --ttl=2 --walkers=5"
```

---

Normalized Flood:

max nodes - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/4_regular_with_200_nodes/4_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=197 --outputFolder=4_regular_200_nodes/normalized_flood --ttl=2 --walkers=2"
```

---

---

# 5_regular_200_nodes

**nodes 0 to 197 distance is 5**

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/5_regular_with_200_nodes/5_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=197 --outputFolder=5_regular_200_nodes/flood --ttl=3"
```

---

Random Walk:

walkers - 5

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/5_regular_with_200_nodes/5_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=197 --outputFolder=5_regular_200_nodes/random_walk --ttl=3 --walkers=5"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/5_regular_with_200_nodes/5_regular_with_200_nodes.txt  --srcIndex=0 --sinkIndex=197 --outputFolder=5_regular_200_nodes/normalized_flood --ttl=3 --walkers=3"
```

---

# cluster_6_with_4_nodes_each

**nodes 0 to 2 distance is 1**

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_4_nodes_each/cluster_6_with_4_nodes_each.txt  --srcIndex=0 --sinkIndex=2 --outputFolder=cluster_6_with_4_nodes_each/flood --ttl=3"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_4_nodes_each/cluster_6_with_4_nodes_each.txt  --srcIndex=0 --sinkIndex=2 --outputFolder=cluster_6_with_4_nodes_each/random_walk --ttl=3 --walkers=3"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_4_nodes_each/cluster_6_with_4_nodes_each.txt  --srcIndex=0 --sinkIndex=2 --outputFolder=cluster_6_with_4_nodes_each/normalized_flood --ttl=3 --walkers=3"
```

---

# cluster_6_with_10_nodes_each

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_10_nodes_each/cluster_6_with_10_nodes_each.txt  --srcIndex=0 --sinkIndex=17 --outputFolder=cluster_6_with_10_nodes_each/flood --ttl=3"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_10_nodes_each/cluster_6_with_10_nodes_each.txt  --srcIndex=0 --sinkIndex=17 --outputFolder=cluster_6_with_10_nodes_each/random_walk --ttl=3 --walkers=3"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_10_nodes_each/cluster_6_with_10_nodes_each.txt  --srcIndex=0 --sinkIndex=17 --outputFolder=cluster_6_with_10_nodes_each/normalized_flood --ttl=3 --walkers=3"
```

---

# cluster_6_with_20_nodes_each

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_20_nodes_each/cluster_6_with_20_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_20_nodes_each/flood --ttl=3"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_20_nodes_each/cluster_6_with_20_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_20_nodes_each/random_walk --ttl=3 --walkers=3"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_20_nodes_each/cluster_6_with_20_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_20_nodes_each/normalized_flood --ttl=3 --walkers=3"
```

---

# cluster_6_with_30_nodes_each

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_30_nodes_each/cluster_6_with_30_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_30_nodes_each/flood --ttl=3"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_30_nodes_each/cluster_6_with_30_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_30_nodes_each/random_walk --ttl=3 --walkers=3"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/cluster_6_with_30_nodes_each/cluster_6_with_30_nodes_each.txt  --srcIndex=0 --sinkIndex=18 --outputFolder=cluster_6_with_30_nodes_each/normalized_flood --ttl=3 --walkers=3"
```

---

# megagraph_3_clusters

**nodes 0 to 47 distance is 2**

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/megagraph_3_clusters/megagraph_3_clusters.txt  --srcIndex=0 --sinkIndex=47 --outputFolder=megagraph_3_clusters/flood --ttl=3"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/megagraph_3_clusters/megagraph_3_clusters.txt  --srcIndex=0 --sinkIndex=47 --outputFolder=megagraph_3_clusters/random_walk --ttl=3 --walkers=3"
```

---

Normalized Flood:

max nodes - 2

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/megagraph_3_clusters/megagraph_3_clusters.txt  --srcIndex=0 --sinkIndex=47 --outputFolder=megagraph_3_clusters/normalized_flood --ttl=3 --walkers=2"
```

---

# megagraph_5_clusters

**nodes 0 to 52 distance is 3**

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=3 --fileName=scratch/code/graphs/megagraph_5_clusters/megagraph_5_clusters.txt  --srcIndex=0 --sinkIndex=52 --outputFolder=megagraph_5_clusters/flood --ttl=3"
```

---

Random Walk:

walkers - 5

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/megagraph_5_clusters/megagraph_5_clusters.txt  --srcIndex=0 --sinkIndex=52 --outputFolder=megagraph_5_clusters/random_walk --ttl=3 --walkers=5"
```

---

Normalized Flood:

max nodes - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=3 --fileName=scratch/code/graphs/megagraph_5_clusters/megagraph_5_clusters.txt  --srcIndex=0 --sinkIndex=52 --outputFolder=megagraph_5_clusters/normalized_flood --ttl=3 --walkers=3"
```

---

# tree with 10 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=10 --srcIndex=0 --sinkIndex=9  --ttl=3 --outputFolder=tree_with_10_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=10 --srcIndex=0 --sinkIndex=9  --ttl=3 --outputFolder=tree_with_10_nodes/random_walk --walkers=1"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=10 --srcIndex=0 --sinkIndex=9  --ttl=3 --outputFolder=tree_with_10_nodes/normalized_flood --walkers=1"
```

---

# tree with 20 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=20 --srcIndex=0 --sinkIndex=19  --ttl=3 --outputFolder=tree_with_20_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=20 --srcIndex=0 --sinkIndex=19  --ttl=3 --outputFolder=tree_with_20_nodes/random_walk --walkers=2"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=20 --srcIndex=0 --sinkIndex=19  --ttl=3 --outputFolder=tree_with_20_nodes/normalized_flood --walkers=1"
```

---

# tree with 30 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=30 --srcIndex=0 --sinkIndex=29  --ttl=3 --outputFolder=tree_with_30_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=30 --srcIndex=0 --sinkIndex=29  --ttl=3 --outputFolder=tree_with_30_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=30 --srcIndex=0 --sinkIndex=29  --ttl=3 --outputFolder=tree_with_30_nodes/normalized_flood --walkers=1"
```

---

# tree with 40 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=40 --srcIndex=0 --sinkIndex=39  --ttl=3 --outputFolder=tree_with_40_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=40 --srcIndex=0 --sinkIndex=39  --ttl=3 --outputFolder=tree_with_40_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=40 --srcIndex=0 --sinkIndex=39  --ttl=3 --outputFolder=tree_with_40_nodes/normalized_flood --walkers=1"
```

---

# tree with 50 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=50 --srcIndex=0 --sinkIndex=49  --ttl=3 --outputFolder=tree_with_50_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=50 --srcIndex=0 --sinkIndex=49  --ttl=3 --outputFolder=tree_with_50_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=50 --srcIndex=0 --sinkIndex=49  --ttl=3 --outputFolder=tree_with_50_nodes/normalized_flood --walkers=1"
```

---

# tree with 60 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=60 --srcIndex=0 --sinkIndex=59  --ttl=3 --outputFolder=tree_with_60_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=60 --srcIndex=0 --sinkIndex=59  --ttl=3 --outputFolder=tree_with_60_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=60 --srcIndex=0 --sinkIndex=59  --ttl=3 --outputFolder=tree_with_60_nodes/normalized_flood --walkers=1"
```

---

# tree with 70 nodes

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=70 --srcIndex=0 --sinkIndex=69  --ttl=3 --outputFolder=tree_with_70_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=70 --srcIndex=0 --sinkIndex=69  --ttl=3 --outputFolder=tree_with_70_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=70 --srcIndex=0 --sinkIndex=69  --ttl=3 --outputFolder=tree_with_70_nodes/normalized_flood --walkers=1"
```

---

# tree with 80 nodes

**nodes 0 to 79 distance is 5**

ttl - 3

ttl increases by 5

---

Flood:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=80 --srcIndex=0 --sinkIndex=79  --ttl=3 --outputFolder=tree_with_80_nodes/flood"
```

---

Random Walk:

walkers - 3

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=1 --nodeNum=80 --srcIndex=0 --sinkIndex=79  --ttl=3 --outputFolder=tree_with_80_nodes/random_walk --walkers=3"
```

---

Normalized Flood:

max nodes - 1

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=2 --networkType=1 --nodeNum=80 --srcIndex=0 --sinkIndex=79  --ttl=3 --outputFolder=tree_with_80_nodes/normalized_flood --walkers=1"
```

---
