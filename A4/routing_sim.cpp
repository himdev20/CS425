
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const int INF = 9999;

// This function prints the routing table for a given node
// It shows the destination, cost to reach it, and the next hop node
void printDVRTable(int node, const vector<vector<int>>& table, const vector<vector<int>>& nextHop) {
    cout << "Node " << node << " Routing Table:\n";
    cout << "Dest\tCost\tNext Hop\n";
    for (int i = 0; i < table.size(); ++i) {
        cout << i << "\t" << table[node][i] << "\t";
        if (nextHop[node][i] == -1) cout << "-";
        else cout << nextHop[node][i];
        cout << endl;
    }
    cout << endl;
}

// This function simulates the Distance Vector Routing (DVR) algorithm
// It initializes the distance table and next hop table, then applies the Bellman-Ford algorithm
void simulateDVR(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<vector<int>> dist = graph;
    vector<vector<int>> nextHop(n, vector<int>(n, -1));

    // This initializes the next hop matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                nextHop[i][j] = -1;  // Self
            } else if (graph[i][j] != INF) {
                nextHop[i][j] = j;  // Direct link
            }
        }
    }

    // This applies the Bellman-Ford algorithm to find the shortest paths
    // and update the next hop table
    bool updated;
    do {
        updated = false;
        // This loop checks all nodes
        for (int i = 0; i < n; ++i) {
            // This loop checks all destinations
            for (int j = 0; j < n; ++j) {
                // This skips if the same node
                if (i == j) continue;
                
                // This loop checks through all neighbors of node i
                for (int k = 0; k < n; ++k) {
                    // This skips if no direct link to neighbor k or if k is the same as i or j
                    // or if the distance to k is infinite
                    if (i == k || j == k || graph[i][k] == INF) continue;
                    
                    // This checks if the path through k is better than the current path
                    if (dist[i][j] > graph[i][k] + dist[k][j]) {
                        dist[i][j] = graph[i][k] + dist[k][j];
                        nextHop[i][j] = k;  // This updates the next hop to the neighbor
                        // This marks that an update has occurred
                        updated = true;
                    }
                }
            }
        }
    } while (updated);

    cout << "--- DVR Final Tables ---\n";
    for (int i = 0; i < n; ++i) printDVRTable(i, dist, nextHop);
}

// This function prints the routing table for a given node in the Link State Routing (LSR) algorithm
// It shows the destination, cost to reach it, and the next hop node
void printLSRTable(int src, const vector<int>& dist, const vector<int>& prev) {
    cout << "Node " << src << " Routing Table:\n";
    cout << "Dest\tCost\tNext Hop\n";
    for (int i = 0; i < dist.size(); ++i) {
        if (i == src) continue;
        cout << i << "\t" << dist[i] << "\t";
        int hop = i;
        while (prev[hop] != src && prev[hop] != -1)
            hop = prev[hop];
        cout << (prev[hop] == -1 ? -1 : hop) << endl;
    }
    cout << endl;
}

// This function simulates the Link State Routing (LSR) algorithm
// It initializes the distance and previous node tables, then applies Dijkstra's algorithm
void simulateLSR(const vector<vector<int>>& graph) {
    int n = graph.size();
    for (int src = 0; src < n; ++src) {
        vector<int> dist(n, INF);
        vector<int> prev(n, -1);
        vector<bool> visited(n, false);
        dist[src] = 0;
        
        // This applies Dijkstra's algorithm to find the shortest paths
        for (int count = 0; count < n; ++count) {
            // This finds the minimum distance vertex from the set of vertices not yet visited
            int u = -1;
            int minDistance = INF;
            for (int v = 0; v < n; ++v) {
                if (!visited[v] && dist[v] < minDistance) {
                    minDistance = dist[v];
                    u = v;
                }
            }
            
            // This breaks if all vertices are visited
            // or if the minimum distance is infinite (no path)
            if (u == -1) break;
            
            // This marks the vertex as visited
            // and updates the distance to its neighbors
            visited[u] = true;
            
            // This loop checks all neighbors of u
            for (int v = 0; v < n; ++v) {
                if (!visited[v] && graph[u][v] != INF && 
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    prev[v] = u;
                }
            }
        }
        
        printLSRTable(src, dist, prev);
    }
}

// This function reads the graph from a file
// The first line contains the number of nodes, followed by the adjacency matrix
vector<vector<int>> readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    int n;
    file >> n;
    vector<vector<int>> graph(n, vector<int>(n));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            file >> graph[i][j];

    file.close();
    return graph;
}


// This is the main function that reads the graph from a file and simulates both routing algorithms
// It takes the filename as a command line argument
int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string filename = argv[1];
    vector<vector<int>> graph = readGraphFromFile(filename);

    cout << "\n--- Distance Vector Routing Simulation ---\n";
    simulateDVR(graph);

    cout << "\n--- Link State Routing Simulation ---\n";
    simulateLSR(graph);

    return 0;
}