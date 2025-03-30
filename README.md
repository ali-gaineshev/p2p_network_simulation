# Comparison of Different Search Algorithms in Peer-to-Peer Network

CSC 466 - Overlay and peer to peer networks
Semester Project
Spring 2025

---

## Table of Contents

- [Project-Overview](#project-overview)
- [Course Information](#course-information)
- [Team Members](#team-members)
- [Objective](#objective)
- [Overview](#overview)
- [Technologies Used](#technologies-used)
- [Features](#features)
- [Installation & Setup](#installation--setup)
- [Usage](#usage)
- [Limitations & Future Work](#limitations--future-work)
- [References](#references)
- [License](#license)

---

## Project Overview

---

## Course Information

---

## Team Members

| Name            | Student ID | Email                |
| --------------- | ---------- | -------------------- |
| Holly Gummerson | V00986098   | hgummerson@uvic.ca   |
| Ali Gaineshev   | V00097349   | ggaineshev@gmail.com |
| Mathew Terhune  | V00943466   | mterhune@uvic.ca     |

---

## Objective

Create and simluate

---

## Overview

---

## Technologies Used

* Nice visualiztion website - https://graphonline.top/en/
* Import graphml file into it
---

# Features

- Different topologies


---

## Installation & Setup

### Prerequisites

- g++ / gcc:C++ Compiler (C++17 support needed)
- Python3 : Python Bindings / waf system
- cmake
- git

#### Network simulator 3 setup

#### MacOS

https://www.nsnam.org/docs/installation/html/macos.html

Follow the steps below to isntall and setup NS-3 on macOS

STEP ONE

Install [Homebrew](https://brew.sh/) `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

using brew install the following
`brew install gcc` GNU Compiler Collection
`brew install python@3.11` (most recent versions will work as well)
Install the required libraries for Network Simulator

- sqlite:`brew install sqlite`
- libzml2:`brew install libxml2`
- NetAnim: `brew install qt@5`
- GNUplot: `brew install gnuplot`

While sqlite and libxml12 are not being used in out implementation. However, we are unsure of how not having them install may leave to inconsistencies.

STEP 2

At this point you can choose what route you would like to take when installing NS3 you can choose to do git or \_\_ version

GITHUB VERSION

Navigate to where you would like to install NS-3 to and then run.

`git clone https://gitlab.com/nsnam/ns-3-dev.git`

#### Makefile setup

#### Project Structure

### Clone the repository

# Running the code TEMP PlEASE LOOK HERE
replace the whole scratch folder content with this. Then

To start:

```
./ns3 clean
./ns3 configure --disable-werror --disable-warning
```

To run tree topology:

```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --nodeNum=10 --srcIndex=0 --sinkIndex=9"
```

To run file:

```
 ./ns3 build && ./ns3 run "scratch/code/network-sim.cc --fileName=filepath  --srcIndex=0 --sinkIndex=9"
```
Then add network type:

* 1 - TREE
* 2 - LINEAR
* 3 - READ_FILE


```
--networkType=X
```

After that add *Search Algorithm*:

For Flood:
```
--searchAlg=0
```

For Random Walk:
```
--searchAlg=1 --walkers=k
```

For Normalized Flood:
```
--searchAlg=2 --walkers=k
```

To set up initial ttl:
```
--ttl=X
```

#### Example calls:

* Flood with initial ttl 5 and tree topology
```
./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=0 --networkType=1 --nodeNum=10 --srcIndex=0 --sinkIndex=9  --ttl=5"
```

* running a file with random walk and 6 walkers
```
 ./ns3 build && ./ns3 run "scratch/code/network-sim.cc --searchAlg=1 --networkType=3 --fileName=scratch/code/graphs/2_regular_with_10_nodes.txt  --srcIndex=0 --sinkIndex=9 --walkers=6"
```

#### Running Network Anim

```
./netanim-3.109/NetAnim
```
After NetAnim opens:

1. Select 'Open XML trace file'
2. Open "p2p-network-routing.xml" from the ns3 folder
3. Press 'Play animation'

#### Running Graph Generator

* Creating a regular graph connected with x edges and y nodes 
```
% python ./graph_generator.py regular x y
```

* Creating a cluster graph with x groups of y nodes
```
% python ./graph_generator.py cluster x y
```

* Creating a mega-graph connected with subcluster one containing x groups of y nodes so on ...
```
% python ./graph_generator.py megagraph x y a b ... d e
```

## Usage

---
