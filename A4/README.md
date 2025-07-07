
# Distance Vector and Link State Routing Simulation
This project simulates two fundamental network routing algorithms: Distance Vector Routing (DVR) and Link State Routing (LSR) using C++. It reads a graph (network topology) from an input file and outputs the routing tables for each node.

---

## Core Concepts Implemented

### Distance Vector Routing (DVR)
 This approach is inspired by the Bellman-Ford algorithm.
- Each node regularly exchanges its routing information with its immediate neighbors.

- Over time, as nodes share and update their data, they gradually learn the shortest paths to all other nodes in the network.

- Every node maintains a routing table that keeps track of the cost to reach each destination, along with the next neighbor to forward the data to.



### Link State Routing (LSR)
This method is based on Dijkstra’s algorithm.

- Each node has a complete view of the entire network and uses this information to calculate the shortest path to every other node.

- As a result, each node creates a routing table that lists the most efficient path to each destination, along with the next node to send the data to.

### Graph Input Handling
- Reads adjacency matrix from file.
- Handles disconnected nodes using INF = 9999.

---

## Key Functions

1. **`printDVRTable()`**:  Prints a node’s DVR routing table in a readable format with destination, cost, and next hop.
2. **`simulateDVR()`**:  Simulates the Distance Vector Routing algorithm using the Bellman-Ford algorithm, updating the routing table iteratively until convergence.

3. **`printLSRTable()`** :  Prints a node’s LSR routing table showing destination, cost, and next hop.

4. **`simulateLSR()`**:  Simulates the Link State Routing algorithm using Dijkstra’s algorithm, calculating the shortest paths from each node.

5. **`readGraphFromFile()`**: Reads an adjacency matrix from a file to construct the network graph.


---

## Input Format
Provide a file with the following structure:

```
<N>  // Number of nodes
a11 a12 a13 ... a1N
a21 a22 a23 ... a2N
...
aN1 aN2 aN3 ... aNN
```
Use 9999 for no direct link between nodes.

### Example:
```
4
0 3 9999 7
3 0 1 9999
9999 1 0 2
7 9999 2 0
```

---

## How to Compile and Run

1. Compile the code:
```bash
g++ routing_simulation.cpp -o routing
```

2. Run the executable with input file:
```bash
./routing input.txt
```
---

## Expected Output

### DVR Output:
```
--- DVR Initial Tables (Iteration 0) ---
Node 0 Routing Table:
Dest   Cost   Next Hop
0      0      -
1      3      1
2      9999   -
3      7      3
...
```

### LSR Output:
```
Node 0 Routing Table:
Dest   Cost   Next Hop
1      3      1
2      4      1
3      6      1
...
```

---

## Individual Contributions

- Himanshu Mahale (230476): DVR logic, file parsing, testing.
- Vineet Nagrale (231158): LSR logic, debugging, README.

---

## References

 - Sources for Distance Vector Routing (Bellman-Ford Algorithm) : 
#https://www.geeksforgeeks.org/distance-vector-routing-dvr-protocol/
 https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm


-  Sources for Link State Routing (Dijkstra's Algorithm) : 
 https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-graph-data-structure/
 https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm


 - Sources for C++ Implementation and File I/O
 https://cplusplus.com/doc/tutorial/files/
 https://cplusplus.com/reference/vector/vector/

 - General Networking Concepts and RFCs
 https://datatracker.ietf.org/doc/html/rfc1058   # RIP (Distance Vector)
 https://datatracker.ietf.org/doc/html/rfc2328   # OSPF (Link State)


---

## Declaration

We declare that this work is original and free from plagiarism.

---
