#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <map>
#include <iomanip>

using namespace std;

class FindingCliques {
public:
    vector<int> Q;
    vector<vector<int> > adjlist;
    unordered_map<int, int> nodes;
    int count = 0;
    int maxsize = 0;
    int cliqueCount = 0;
    map<int, int> histogram; // Stores frequency of clique sizes
    int numNodes, numEdges;
    
    FindingCliques(const string& fname) {
        ifstream file(fname);
        file >> numNodes >> numEdges;
        
        adjlist.resize(numNodes);
        
        int u, v;
        for (int i = 0; i < numEdges; i++) {
            file >> u >> v;
            
            if (nodes.find(u) == nodes.end()) {
                nodes[u] = count++;
            }
            if (nodes.find(v) == nodes.end()) {
                nodes[v] = count++;
            }
            
            int u1 = nodes[u];
            int v1 = nodes[v];
            
            adjlist[u1].push_back(v1);
            adjlist[v1].push_back(u1);
        }
        
        for (auto &a : adjlist) {
            sort(a.begin(), a.end());
        }
    }
    
    void findCliques() {
        vector<int> vertices(count);
        iota(vertices.begin(), vertices.end(), 0);
        expand(vertices, vertices);
        
        cout << "Total Maximal Cliques: " << cliqueCount << endl;
        cout << "Largest Clique Size: " << maxsize << endl;
    }
    
private:
    vector<int> intersect(const vector<int>& A, const vector<int>& B) {
        vector<int> result;
        set_intersection(A.begin(), A.end(), B.begin(), B.end(), back_inserter(result));
        return result;
    }
    
    vector<int> diff(const vector<int>& A, const vector<int>& B) {
        vector<int> result;
        set_difference(A.begin(), A.end(), B.begin(), B.end(), back_inserter(result));
        return result;
    }
    
    int select(const vector<int>& CAND) {
        int u = -1, maxDegree = -1;
        for (int v : CAND) {
            int degree = adjlist[v].size();
            if (degree > maxDegree) {
                maxDegree = degree;
                u = v;
            }
        }
        return u;
    }
    
    void expand(vector<int> SUBG, vector<int> CAND) {
        if (SUBG.empty()) {
            if (!Q.empty()) {
                cliqueCount++;
                int currsize = Q.size();
                maxsize = max(maxsize, currsize);
                histogram[currsize]++;
            }
            return;
        }
        
        int u = select(CAND);
        vector<int> EXTU = diff(CAND, adjlist[u]);
        
        for (int i = EXTU.size() - 1; i >= 0; i--) {
            int q = EXTU[i];
            Q.push_back(q);
            vector<int> SUBGq = intersect(SUBG, adjlist[q]);
            vector<int> CANDq = intersect(CAND, adjlist[q]);
            
            expand(SUBGq, CANDq);
            
            Q.pop_back();
            CAND.erase(remove(CAND.begin(), CAND.end(), q), CAND.end());
        }
    }
};

// Print a formatted histogram of clique sizes to the provided output stream.
void printCliqueHistogram(const map<int, int>& cliqueSizes, ostream& output) {
    if (cliqueSizes.empty()) {
        output << "No cliques found to create histogram." << endl;
        return;
    }
    
    output << "\nClique Size Histogram:" << endl;
    output << "--------------------" << endl;
    output << "Size | Count | Histogram" << endl;
    output << "--------------------" << endl;
    
    int maxCount = 0;
    for (const auto& pair : cliqueSizes) {
        maxCount = max(maxCount, pair.second);
    }
    int scale = max(1, maxCount / 50);
    
    for (const auto& pair : cliqueSizes) {
        int size = pair.first;
        int count = pair.second;
        output << setw(4) << size << " | " << setw(5) << count << " | ";
        int bars = (count + scale - 1) / scale;
        for (int i = 0; i < bars; i++) {
            output << "*";
        }
        output << endl;
    }
    output << "--------------------" << endl;
    output << "Scale: Each * represents approximately " << scale << " clique(s)" << endl;
}

int main() {
    clock_t start_time = clock();
    
    string fname = "enron.txt";
    FindingCliques finder(fname);
    finder.findCliques();
    
    // Print the formatted histogram to the console
    printCliqueHistogram(finder.histogram, cout);
    
    clock_t end_time = clock();
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    cout << "\nExecution Time: " << elapsed_time << " seconds" << endl;
    
    return 0;
}
