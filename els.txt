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

typedef vector<vector<int>> graph;

// Compute degeneracy ordering using a bucket-based approach
vector<int> computedegeneracyordering(const graph& graph1) {
    const int nodecount1 = graph1.size();
    vector<int> ordering1;
    vector<int> nodedeg1(nodecount1, 0);
    vector<bool> isnoderem1(nodecount1, false);
    
    int maxdeg1 = 0;
    for (int node1 = 0; node1 < nodecount1; node1++) {
        nodedeg1[node1] = graph1[node1].size();
        maxdeg1 = max(maxdeg1, nodedeg1[node1]);
    }
    
    vector<vector<int>> degbuck1(maxdeg1 + 1);
    for (int node1 = 0; node1 < nodecount1; node1++) {
        degbuck1[nodedeg1[node1]].push_back(node1);
    }
    
    for (int idx1 = 0; idx1 < nodecount1; idx1++) {
        int currentdeg1 = 0;
        while (currentdeg1 < degbuck1.size() && degbuck1[currentdeg1].empty()) {
            currentdeg1++;
        }
        
        if (currentdeg1 == degbuck1.size()) 
            break;
        
        int currentnode1 = degbuck1[currentdeg1].back();
        degbuck1[currentdeg1].pop_back();
        
        if (isnoderem1[currentnode1])
            continue;
            
        ordering1.push_back(currentnode1);
        isnoderem1[currentnode1] = true;
        
        for (int neighbor1 : graph1[currentnode1]) {
            if (!isnoderem1[neighbor1]) {
                int olddeg1 = nodedeg1[neighbor1];
                
                auto& bucket1 = degbuck1[olddeg1];
                bucket1.erase(remove(bucket1.begin(), bucket1.end(), neighbor1), bucket1.end());
                
                nodedeg1[neighbor1]--;
                degbuck1[nodedeg1[neighbor1]].push_back(neighbor1);
            }
        }
    }
    
    return ordering1;
}

// Bron-Kerbosch algorithm with pivot selection
void findmaximalcliquescliques(
    const graph& graph1, set<int>& potentialnodes1, set<int>& currentclique1, set<int>& excludednodes1,
    int& cliquecount1, int& maxcliquesize1, set<int>& largestclique1,
    int depth1, const int maxdepth1, map<int, int>& cliquesizedistr1) {
    if (depth1 > maxdepth1) { return; }
    
    if (potentialnodes1.empty() && excludednodes1.empty()) {
        cliquecount1++;
        cliquesizedistr1[currentclique1.size()]++;
        
        if (currentclique1.size() > maxcliquesize1) {
            maxcliquesize1 = currentclique1.size();
            largestclique1 = currentclique1;
        }
        return;
    }
    
    set<int> pivotcandidates1;
    pivotcandidates1.insert(potentialnodes1.begin(), potentialnodes1.end());
    pivotcandidates1.insert(excludednodes1.begin(), excludednodes1.end());
    
    int bestpivot1 = -1; int maxconnections1 = -1;
    
    for (int node1 : pivotcandidates1) {
        int connectioncount1 = 0;
        for (int potential1 : potentialnodes1) {
            if (find(graph1[node1].begin(), graph1[node1].end(), potential1) != graph1[node1].end()) {
                connectioncount1++;
            }
        }
        
        if (connectioncount1 > maxconnections1) {
            maxconnections1 = connectioncount1;
            bestpivot1 = node1;
        }
    }
    
    set<int> nodestoprocess1 = potentialnodes1;
    if (bestpivot1 != -1) {
        for (int neighbor1 : graph1[bestpivot1]) {
            nodestoprocess1.erase(neighbor1);
        }
    }
    
    for (int currentnode1 : nodestoprocess1) {
        set<int> newclique1 = currentclique1; newclique1.insert(currentnode1);
        
        set<int> newpotentialnodes1;
        for (int node1 : potentialnodes1) {
            if (find(graph1[currentnode1].begin(), graph1[currentnode1].end(), node1) != graph1[currentnode1].end()) {
                newpotentialnodes1.insert(node1);
            }
        }
        
        set<int> newexcludednodes1;
        for (int node1 : excludednodes1) {
            if (find(graph1[currentnode1].begin(), graph1[currentnode1].end(), node1) != graph1[currentnode1].end()) {
                newexcludednodes1.insert(node1);
            }
        }
        
        findmaximalcliquescliques(graph1, newpotentialnodes1, newclique1, newexcludednodes1,
            cliquecount1, maxcliquesize1, largestclique1, depth1 + 1, maxdepth1, cliquesizedistr1);
        
        potentialnodes1.erase(currentnode1);
        excludednodes1.insert(currentnode1);
    }
}

// Main clique enumeration function using degeneracy ordering
void findallmaximalcliquescliques(const graph& graph1, int& cliquecount1, int& maxcliquesize1, 
    set<int>& largestclique1, const unordered_map<int, int>& originalids1, map<int, int>& cliquesizes1) {
    const int nodecount1 = graph1.size();
    cliquecount1 = 0; maxcliquesize1 = 0;
    
    if (nodecount1 == 0) return;
    
    cout << "Computing degeneracy ordering..." << endl;
    vector<int> ordering1 = computedegeneracyordering(graph1);
    
    int degeneracy1 = 0;
    vector<bool> processednodes1(nodecount1, false);
    
    for (int idx1 = 0; idx1 < ordering1.size(); idx1++) {
        int node1 = ordering1[idx1];
        processednodes1[node1] = true;
        
        int laterneighbors1 = 0;
        for (int neighbor1 : graph1[node1]) {
            if (!processednodes1[neighbor1]) laterneighbors1++;
        }
        degeneracy1 = max(degeneracy1, laterneighbors1);
    }
    cout << "Graph degeneracy: " << degeneracy1 << endl;
    
    unordered_map<int, int> nodeposition1;
    for (int idx1 = 0; idx1 < ordering1.size(); idx1++) nodeposition1[ordering1[idx1]] = idx1;
    
    cout << "Finding maximal cliques..." << endl;
    const int maxrecursiondepth1 = 1000000;
    
    for (int idx1 = 0; idx1 < ordering1.size(); idx1++) {
        int startnode1 = ordering1[idx1];
        
        set<int> potentialnodes1, excludednodes1;
        
        for (int neighbor1 : graph1[startnode1]) {
            if (nodeposition1[neighbor1] > idx1) potentialnodes1.insert(neighbor1);
        }
        
        for (int neighbor1 : graph1[startnode1]) {
            if (nodeposition1[neighbor1] < idx1) excludednodes1.insert(neighbor1);
        }
        
        set<int> startclique1 = {startnode1};
        
        findmaximalcliquescliques(graph1, potentialnodes1, startclique1, excludednodes1,
            cliquecount1, maxcliquesize1, largestclique1, 0, maxrecursiondepth1, cliquesizes1);
    }
    
    set<int> originallargestclique1;
    for (int node1 : largestclique1) {
        if (originalids1.count(node1)) originallargestclique1.insert(originalids1.at(node1));
    }
    largestclique1 = originallargestclique1;
}

// Load graph from file and map vertex IDs
graph loadgraphfromfilefile(const string& filename1, int& vertexcount1, int& edgecount1,
    unordered_map<int, int>& origtointernal1, unordered_map<int, int>& internaltorig1) {
    ifstream inputfile1(filename1);
    if (!inputfile1.is_open()) {
        cout << "Error: Could not open input file " << filename1 << endl;
        return graph();
    }
    
    string line1;
    vertexcount1 = 0; edgecount1 = 0;
    
    // Skip comment lines
    while (getline(inputfile1, line1)) {
        if (!line1.empty() && line1[0] != '#') break;
    }
    
    stringstream headerline1(line1);
    headerline1 >> vertexcount1 >> edgecount1;
    
    if (vertexcount1 <= 0 || edgecount1 < 0) {
        cout << "Invalid graph format: vertexcount1=" << vertexcount1 
             << ", edgecount1=" << edgecount1 << endl;
        return graph();
    }
    
    set<int> uniquevertices1;
    vector<pair<int, int>> edgelist1;
    int sourcevertex1, targetvertex1;
    
    while (inputfile1 >> sourcevertex1 >> targetvertex1) {
        uniquevertices1.insert(sourcevertex1); uniquevertices1.insert(targetvertex1);
        edgelist1.emplace_back(sourcevertex1, targetvertex1);
    }
    
    int internalid1 = 0;
    for (int originalid1 : uniquevertices1) {
        origtointernal1[originalid1] = internalid1;
        internaltorig1[internalid1] = originalid1; internalid1++;
    }
    
    vertexcount1 = uniquevertices1.size();
    
    graph adjacencylists1(vertexcount1);
    
    for (const auto& edge1 : edgelist1) {
        int internalsource1 = origtointernal1[edge1.first];
        int internaltarget1 = origtointernal1[edge1.second];
        
        adjacencylists1[internalsource1].push_back(internaltarget1);
        if (internalsource1 != internaltarget1) adjacencylists1[internaltarget1].push_back(internalsource1);
    }
    
    // Remove duplicate edges
    for (auto& neighbors1 : adjacencylists1) {
        sort(neighbors1.begin(), neighbors1.end());
        neighbors1.erase(unique(neighbors1.begin(), neighbors1.end()), neighbors1.end());
    }
    
    inputfile1.close(); return adjacencylists1;
}

int main() {
    auto starttime1 = chrono::high_resolution_clock::now();

    int nodecount1, edgecount1;
    unordered_map<int, int> externaltointernalid1;
    unordered_map<int, int> internaltoexternalid1;

    cout << "Loading graph from file..." << endl;
    graph graph1 = loadgraphfromfilefile("input.txt", nodecount1, edgecount1, 
                    externaltointernalid1, internaltoexternalid1);
    
    if (graph1.empty()) { cout << "Failed to load the graph from file." << endl; return 1; }
    
    cout << "Graph loaded: " << nodecount1 << " nodes, " << edgecount1 << " edges" << endl;
    cout << "Finding cliques... (this may take a while)" << endl;
    
    int totalcliques1 = 0; int largestcliquesize1 = 0;
    set<int> largestcliquenodes1; map<int, int> cliquedistribution1;

    findallmaximalcliquescliques(graph1, totalcliques1, largestcliquesize1, largestcliquenodes1,
                        internaltoexternalid1, cliquedistribution1);
    
    cout << "\n========== RESULTS ==========\n" << endl;
    cout << "Total Maximal Cliques: " << totalcliques1 << endl;
    cout << "Largest Clique Size:   " << largestcliquesize1 << endl;
    
    cout << "\nClique Size Distribution:" << endl;
    cout << "--------------------" << endl;
    cout << "Size | Count | Histogram" << endl;
    
    int maxcount1 = 0;
    for (auto iter1 = cliquedistribution1.begin(); iter1 != cliquedistribution1.end(); ++iter1) {
        int size1 = iter1->first;
        int count1 = iter1->second;
        maxcount1 = max(maxcount1, count1);
    }
    
    int scalefactor1 = max(1, maxcount1 / 50);
    
    for (auto iter1 = cliquedistribution1.begin(); iter1 != cliquedistribution1.end(); ++iter1) {
        int size1 = iter1->first;
        int count1 = iter1->second;
        cout << setw(4) << size1 << " | " << setw(5) << count1 << " | ";
        
        int barlength1 = (count1 + scalefactor1 - 1) / scalefactor1;
        string bar1(barlength1, '*'); cout << bar1 << endl;
    }
    
    cout << "--------------------" << endl;
    cout << "Scale: Each * represents approximately " << scalefactor1 << " clique(s)" << endl;
    
    auto endtime1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedtime1 = endtime1 - starttime1;
    cout << "\nExecution Time: " << elapsedtime1.count() << " seconds" << endl;
    
    return 0;
}