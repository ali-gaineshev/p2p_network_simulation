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
| Holly Gummerson | 12345678   | hgummerson@uvic.ca   |
| Ali Gaineshev   | 87654321   | ggaineshev@gmail.com |
| Mathew Terhune  | 56781234   | mterhune@uvic.ca     |

---

## Objective

Create and simluate

---

## Overview

---

## Technologies Used

---

# Features

✅ Feature: Different topologies
✅ Feature
✅ Feature
✅ Feature

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
./ns3 build && ./ns3 run "scratch/network-sim.cc --nodeNum=10 --srcIndex=0 --sinkIndex=9 --networkType=1"
```

To run file:

```
 ./ns3 build && ./ns3 run "scratch/code/network-sim.cc --nodeNum=10 --srcIndex=0 --sinkIndex=9 --networkType=3 --fileName=scratch/code/graphs/2_regular_with_10_nodes.txt"
```

#### Running Network Anim

---

## Usage

---
