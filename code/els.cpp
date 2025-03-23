#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <limits>
#include <iomanip>
#include <fstream>

using namespace std;

typedef vector<vector<int>> Graph;

// Compute degeneracy ordering using a bucket-based approach.
vector<int> degeneracyOrderingBucket(const Graph& graph) {
    int n = graph.size();
    vector<int> order;
    vector<int> degree(n, 0);
    vector<bool> removed(n, false);
    
    int maxDegree = 0;
    for (int i = 0; i < n; i++) {
        degree[i] = graph[i].size();
        maxDegree = max(maxDegree, degree[i]);
    }
    
    vector<vector<int>> buckets(maxDegree + 1);
    for (int i = 0; i < n; i++) {
        buckets[degree[i]].push_back(i);
    }
    
    for (int i = 0; i < n; i++) {
        int d = 0;
        while (d < buckets.size() && buckets[d].empty()) {
            d++;
        }
        if (d == buckets.size()) break;
        
        int v = buckets[d].back();
        buckets[d].pop_back();
        if (removed[v])
            continue;
        order.push_back(v);
        removed[v] = true;
        
        for (int u : graph[v]) {
            if (!removed[u]) {
                int currDeg = degree[u];
                auto& bucket = buckets[currDeg];
                bucket.erase(remove(bucket.begin(), bucket.end(), u), bucket.end());
                degree[u]--;
                buckets[degree[u]].push_back(u);
            }
        }
    }
    
    return order;
}

// Bron-Kerbosch algorithm with pivoting (with a recursion depth limit).
void BronKerboschPivot(const Graph& graph, set<int>& P, set<int>& R, set<int>& X, 
                         int& cliqueCount, int& maxCliqueSize, set<int>& largestClique,
                         int depth, const int maxDepth, map<int, int>& cliqueSizes,
                         ostream& /*dummyOutput*/) {
    if (depth > maxDepth) {
        return;
    }
    
    if (P.empty() && X.empty()) {
        cliqueCount++;
        cliqueSizes[R.size()]++;
        if (R.size() > maxCliqueSize) {
            maxCliqueSize = R.size();
            largestClique = R;
        }
        return;
    }
    
    // Choose pivot from P ∪ X.
    set<int> unionPX;
    unionPX.insert(P.begin(), P.end());
    unionPX.insert(X.begin(), X.end());
    int pivot = -1, maxConnections = -1;
    for (int u : unionPX) {
        int connections = 0;
        for (int v : P) {
            if (find(graph[u].begin(), graph[u].end(), v) != graph[u].end()) {
                connections++;
            }
        }
        if (connections > maxConnections) {
            maxConnections = connections;
            pivot = u;
        }
    }
    
    // P without neighbors of pivot.
    set<int> P_without_neighbors = P;
    if (pivot != -1) {
        for (int neighbor : graph[pivot]) {
            P_without_neighbors.erase(neighbor);
        }
    }
    
    for (int v : P_without_neighbors) {
        set<int> R_new = R;
        R_new.insert(v);
        
        set<int> P_new;
        for (int u : P) {
            if (find(graph[v].begin(), graph[v].end(), u) != graph[v].end()) {
                P_new.insert(u);
            }
        }
        
        set<int> X_new;
        for (int u : X) {
            if (find(graph[v].begin(), graph[v].end(), u) != graph[v].end()) {
                X_new.insert(u);
            }
        }
        
        BronKerboschPivot(graph, P_new, R_new, X_new, cliqueCount, maxCliqueSize, largestClique,
                          depth + 1, maxDepth, cliqueSizes, /*dummyOutput*/ std::cout);
        
        P.erase(v);
        X.insert(v);
    }
}

// Main Bron-Kerbosch function using degeneracy ordering.
void BronKerboschDegeneracy(const Graph& graph, int& cliqueCount, int& maxCliqueSize, set<int>& largestClique, 
                             const unordered_map<int, int>& originalIds, map<int, int>& cliqueSizes,
                             ostream& dummyOutput) {
    int n = graph.size();
    cliqueCount = 0;
    maxCliqueSize = 0;
    
    if (n == 0) {
        return;
    }
    
    // (Debug messages are redirected to dummyOutput and thus not printed to console.)
    dummyOutput << "Computing degeneracy ordering..." << endl;
    vector<int> ordering = degeneracyOrderingBucket(graph);
    
    int degeneracy = 0;
    vector<bool> processed(n, false);
    for (int i = 0; i < ordering.size(); i++) {
        int v = ordering[i];
        processed[v] = true;
        int later_neighbors = 0;
        for (int neighbor : graph[v]) {
            if (!processed[neighbor])
                later_neighbors++;
        }
        degeneracy = max(degeneracy, later_neighbors);
    }
    dummyOutput << "Graph degeneracy: " << degeneracy << endl;
    
    unordered_map<int, int> pos;
    for (int i = 0; i < ordering.size(); i++) {
        pos[ordering[i]] = i;
    }
    
    dummyOutput << "Finding maximal cliques..." << endl;
    const int MAX_RECURSION_DEPTH = 1000;
    
    for (int i = 0; i < ordering.size(); i++) {
        int v = ordering[i];
        
        set<int> P, X;
        for (int neighbor : graph[v]) {
            if (pos[neighbor] > i) {
                P.insert(neighbor);
            }
        }
        for (int neighbor : graph[v]) {
            if (pos[neighbor] < i) {
                X.insert(neighbor);
            }
        }
        
        set<int> R = {v};
        BronKerboschPivot(graph, P, R, X, cliqueCount, maxCliqueSize, largestClique,
                          0, MAX_RECURSION_DEPTH, cliqueSizes, dummyOutput);
    }
    
    // Convert internal vertex IDs back to the original ones.
    set<int> originalLargestClique;
    for (int v : largestClique) {
        if (originalIds.count(v)) {
            originalLargestClique.insert(originalIds.at(v));
        }
    }
    largestClique = originalLargestClique;
}

// Reads a graph from file and maps arbitrary vertex labels to internal consecutive IDs.
Graph readGraphFromFile(const string& filename, int& numVertices, int& numEdges, 
                          unordered_map<int, int>& origToInternal, 
                          unordered_map<int, int>& internalToOrig, ostream& dummyOutput) {
    ifstream input(filename);
    if (!input.is_open()) {
        dummyOutput << "Error: Could not open input file " << filename << endl;
        return Graph();
    }
    
    string line;
    numVertices = 0;
    numEdges = 0;
    
    // Skip header comments.
    while (getline(input, line)) {
        if (!line.empty() && line[0] != '#') {
            break;
        }
    }
    
    stringstream ss(line);
    ss >> numVertices >> numEdges;
    
    if (numVertices <= 0 || numEdges < 0) {
        dummyOutput << "Invalid graph format: numVertices=" << numVertices 
                    << ", numEdges=" << numEdges << endl;
        return Graph();
    }
    
    set<int> vertices;
    vector<pair<int, int>> edges;
    int u, v;
    while (input >> u >> v) {
        vertices.insert(u);
        vertices.insert(v);
        edges.emplace_back(u, v);
    }
    
    int internalID = 0;
    for (int originalID : vertices) {
        origToInternal[originalID] = internalID;
        internalToOrig[internalID] = originalID;
        internalID++;
    }
    
    numVertices = vertices.size();
    Graph graph(numVertices);
    
    for (const auto& edge : edges) {
        int internalU = origToInternal[edge.first];
        int internalV = origToInternal[edge.second];
        graph[internalU].push_back(internalV);
        if (internalU != internalV) {
            graph[internalV].push_back(internalU);
        }
    }
    
    // Remove duplicate edges.
    for (auto& neighbors : graph) {
        sort(neighbors.begin(), neighbors.end());
        neighbors.erase(unique(neighbors.begin(), neighbors.end()), neighbors.end());
    }
    
    input.close();
    return graph;
}

int main() {
    try {
        // Record overall start time.
        auto overallStart = chrono::high_resolution_clock::now();

        int numVertices, numEdges;
        unordered_map<int, int> origToInternal;
        unordered_map<int, int> internalToOrig;

        // Use a dummy output stream to suppress internal logging.
        ostringstream dummyOutput;

        // Read the graph from file (e.g., "skitter.txt").
        Graph graph = readGraphFromFile("skitter.txt", numVertices, numEdges, origToInternal, internalToOrig, dummyOutput);
        if (graph.empty()) {
            cerr << "Failed to read graph." << endl;
            return 1;
        }
        
        int cliqueCount = 0, maxCliqueSize = 0;
        set<int> largestClique;
        map<int, int> cliqueSizes;

        // Run the Bron-Kerbosch clique enumeration.
        BronKerboschDegeneracy(graph, cliqueCount, maxCliqueSize, largestClique, internalToOrig, cliqueSizes, dummyOutput);
        
        // Print only the required information.
        cout << "Total Maximal Cliques: " << cliqueCount << "\n";
        cout << "Largest Clique Size: " << maxCliqueSize << "\n\n";
        cout << "Clique Size Histogram:" << "\n";
        cout << "--------------------" << "\n";
        cout << "Size | Count | Histogram" << "\n";
        
        // Replicate the histogram printing.
        int maxCount = 0;
        for (const auto &pair : cliqueSizes) {
            maxCount = max(maxCount, pair.second);
        }
        int scale = max(1, maxCount / 50);
        for (const auto &pair : cliqueSizes) {
            int size = pair.first;
            int count = pair.second;
            cout << setw(4) << size << " | " << setw(5) << count << " | ";
            int bars = (count + scale - 1) / scale;
            for (int i = 0; i < bars; i++) {
                cout << "*";
            }
            cout << "\n";
        }
        cout << "--------------------" << "\n";
        cout << "Scale: Each * represents approximately " << scale << " clique(s)" << "\n\n";
        
        // Record overall end time.
        auto overallEnd = chrono::high_resolution_clock::now();
        chrono::duration<double> overallElapsed = overallEnd - overallStart;
        cout << "Execution Time: " << overallElapsed.count() << " seconds" << "\n";
        
        return 0;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        cerr << "Unknown error occurred\n";
        return 1;
    }
}
