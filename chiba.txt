#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <iomanip>

using namespace std;

unordered_map<int, int> reverse_renumber;
string path = "enron.txt";

int maxlen = 0;
int numberofmaximalcliques = 0;
int n; // Global number of vertices
vector<int> S, T, C; // Global S, T, C vectors
vector<unordered_set<int>> adj; // Global adjacency list
unordered_map<int,int> distribution;

void update(int i) { 
    // Base case: if we've processed all vertices, count clique.
    if (i == n) {
        numberofmaximalcliques++;
        int len = count(C.begin(), C.end(), 1); // Clique size = number of 1's in C
        maxlen = max(maxlen, len);
        distribution[len]++;
        return;
    }
    // Recursive case.
    else {
        // Build C-N(i) and C∩N(i).
        vector<int> Cminusni, Cintersectni;
        for (int j = 0; j < n; j++) {
            if (C[j] == 1 && adj[i].find(j) == adj[i].end()) {
                Cminusni.push_back(j);
            }
        }
        for (int j : adj[i]) {
            if (C[j] == 1) {
                Cintersectni.push_back(j);
            }
        }
        // If C-N(i) is nonempty, try not adding vertex i.
        if (!Cminusni.empty()) {
            update(i + 1);
        }
        // 2. Compute T[y] = |N(y) ∩ C ∩ N(i)| for y ∉ C ∪ {i}
        for (int x : Cintersectni) {
            for (int y : adj[x]) {
                if (C[y] == 0 && y != i) {
                    T[y]++;
                }
            }
        }
        // 3. Compute S[y] = |N(y) ∩ (C - N(i))| for y ∉ C.
        for (int x : Cminusni) {
            for (int y : adj[x]) {
                if (C[y] == 0) {
                    S[y]++;
                }
            }
        }
        int flag = 1;
        // 4. Maximality test.
        for (int y : adj[i]) {
            if (C[y] == 0 && y < i && T[y] == Cintersectni.size()) {
                flag = 0;
                break;
            }
        }
        // 5. Lexicographical test.
        long long p = Cminusni.size();
        sort(Cminusni.begin(), Cminusni.end());
        // 6.
        for (int k = 0; k < p; k++) {
            for (int y : adj[Cminusni[k]]) {
                if (C[y] == 0 && y < i && T[y] == Cintersectni.size()) {
                    if (y >= Cminusni[k]) {
                        S[y]--;
                    } else {
                        if (k == 0 || y >= Cminusni[k - 1]) {
                            if (S[y] + k == p) {
                                if (k > 0 && y >= Cminusni[k - 1]) {
                                    flag = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        // 7.
        if (!Cintersectni.empty() && p != 0) {
            for (int y = 0; y < n; y++) {
                if (C[y] == 0 && y < i && T[y] == Cintersectni.size() && S[y] == 0) {
                    if (Cminusni[p - 1] < y) {
                        flag = 0;
                        break;
                    }
                }
            }
        } else if (p != 0 && Cminusni[p - 1] < i - 1) {
            flag = 0;
        }
        // 8. Reinitialize T.
        for (int x : Cintersectni) {
            for (int y : adj[x]) {
                if (C[y] == 0 && y != i) {
                    T[y] = 0;
                }
            }
        }
        // 9. Reinitialize S.
        for (int x : Cminusni) {
            for (int y : adj[x]) {
                if (C[y] == 0) {
                    S[y] = 0;
                }
            }
        }
        // 10. If tests pass, update C to reflect addition of vertex i.
        if (flag) {
            for (int x : Cintersectni) {
                C[x] = 2;
            }
            C[i] = 2;
            for (int j = 0; j < n; j++) {
                if (C[j] == 1) C[j] = 0;
                if (C[j] == 2) C[j] = 1;
            }
            update(i + 1);
            C[i] = 0;
            for (int x : Cminusni) {
                C[x] = 1;
            }
        }
    }
}

int main() {
    clock_t start_time = clock();

    ifstream infile(path);
    infile >> n;
    int edges;
    infile >> edges;

    // Read the graph, using a map-of-sets to avoid duplicate edges.
    unordered_map<int, unordered_set<int>> tempadj;
    for (int i = 0; i < edges; i++) {
        int u, v;
        infile >> u >> v;
        tempadj[u].insert(v);
        tempadj[v].insert(u);
    }
    infile.close();

    // Sort vertices by degree.
    vector<pair<int, unordered_set<int>>> adjvec(tempadj.begin(), tempadj.end());
    sort(adjvec.begin(), adjvec.end(), [](const auto& a, const auto& b) {
        return a.second.size() < b.second.size();
    });
    tempadj.clear();
    n = adjvec.size(); // Number of vertices with at least one edge

    // Renumber vertices so that they range from 0 to n-1.
    unordered_map<int, int> renumber;
    long long number = 0;
    for (int i = 0; i < adjvec.size(); i++) {
        renumber[adjvec[i].first] = number++;
    }
    for (auto i : renumber) {
        reverse_renumber[i.second] = i.first;
    }

    // Create the new adjacency list using the renumbered vertex IDs.
    adj.assign(n, unordered_set<int>());
    for (int i = 0; i < adjvec.size(); i++) {
        for (int j : adjvec[i].second) {
            adj[renumber[adjvec[i].first]].insert(renumber[j]);
        }
    }
    adjvec.clear();

    S.assign(n, 0);
    T.assign(n, 0);
    C.assign(n, 0);
    C[0] = 1;

    // No extra printing here.
    update(1);

    clock_t end_time = clock();
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    
    // Print only the required summary output.
    cout << "Total Maximal Cliques: " << numberofmaximalcliques << "\n";
    cout << "Largest Clique Size: " << maxlen << "\n\n";
    
    cout << "Clique Size Histogram:" << "\n";
    cout << "--------------------" << "\n";
    cout << "Size | Count | Histogram" << "\n";
    cout << "--------------------" << "\n";
    
    // Build a sorted vector of clique size counts.
    vector<pair<int, int>> sortedDistribution(distribution.begin(), distribution.end());
    sort(sortedDistribution.begin(), sortedDistribution.end());
    
    int maxCount = 0;
    for (const auto &p : sortedDistribution) {
        maxCount = max(maxCount, p.second);
    }
    int scale = max(1, maxCount / 50);
    
    for (const auto &p : sortedDistribution) {
        int size = p.first;
        int count = p.second;
        cout << setw(4) << size << " | " << setw(5) << count << " | ";
        int bars = (count + scale - 1) / scale;
        for (int i = 0; i < bars; i++) {
            cout << "*";
        }
        cout << "\n";
    }
    
    cout << "--------------------" << "\n";
    cout << "Scale: Each * represents approximately " << scale << " clique(s)" << "\n\n";
    cout << "Execution Time: " << elapsed_time << " seconds" << "\n";
    
    return 0;
}
