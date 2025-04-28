#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include <map>
#include <set>
#include <fstream>
#include <unordered_map>

using namespace std;

struct Edge {
    int to, rev;
    double cap;
    Edge(int t, int r, double c) : to(t), rev(r), cap(c) {}
};

class MaxFlow {
public:
    MaxFlow(int n) : adj(n), level(n), ptr(n) {}
    void addEdge(int from, int to, double cap) {
        adj[from].emplace_back(to, adj[to].size(), cap);
        adj[to].emplace_back(from, adj[from].size()-1, 0.0);
    }
    double dinic(int s, int t) {
        double flow = 0.0;
        while (bfs(s, t)) {
            fill(ptr.begin(), ptr.end(), 0);
            while (double f = dfs(s, t, 1e18))
                flow += f;
        }
        return flow;
    }
    vector<vector<Edge>> adj;
private:
    vector<int> level, ptr;
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto &e : adj[u]) {
                if (e.cap > 1e-9 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                    if (e.to == t) return true;
                }
            }
        }
        return false;
    }
    double dfs(int u, int t, double flow) {
        if (u == t) return flow;
        for (; ptr[u] < adj[u].size(); ptr[u]++) {
            auto &e = adj[u][ptr[u]];
            if (e.cap > 1e-9 && level[e.to] == level[u] + 1) {
                double f = dfs(e.to, t, min(flow, e.cap));
                if (f > 1e-9) {
                    e.cap -= f;
                    adj[e.to][e.rev].cap += f;
                    return f;
                }
            }
        }
        return 0.0;
    }
};

// Graph and clique structures
vector<vector<int>> adj;
int n, m, h;
vector<int> clique_degree;
vector<vector<int>> h_minus_1_cliques;
unordered_map<string, int> clique_index;
vector<vector<int>> clique_extensions;

// Node label mapping
unordered_map<int, int> label_to_index;
vector<int> index_to_label;

// Convert clique to a key string
string clique_to_key(const vector<int>& clique) {
    string key;
    for (int v : clique) key += to_string(v) + "_";
    return key;
}

bool can_form_clique(int v, const vector<int>& vertices) {
    for (int u : vertices) {
        if (!binary_search(adj[v].begin(), adj[v].end(), u)) return false;
    }
    return true;
}

void enumerate_cliques(vector<int>& current_clique, int k, int start_idx) {
    if (k == 0) {
        string key = clique_to_key(current_clique);
        if (clique_index.find(key) == clique_index.end()) {
            clique_index[key] = h_minus_1_cliques.size();
            h_minus_1_cliques.push_back(current_clique);
            vector<int> extensions;
            for (int v = 0; v < n; v++) {
                if (find(current_clique.begin(), current_clique.end(), v) == current_clique.end() && 
                    can_form_clique(v, current_clique)) {
                    extensions.push_back(v);
                }
            }
            clique_extensions.push_back(extensions);
        }
        return;
    }
    for (int i = start_idx; i < n; i++) {
        if (can_form_clique(i, current_clique)) {
            current_clique.push_back(i);
            enumerate_cliques(current_clique, k-1, i+1);
            current_clique.pop_back();
        }
    }
}

void generate_h_minus_1_cliques() {
    h_minus_1_cliques.clear();
    clique_index.clear();
    clique_extensions.clear();

    if (h == 2) {
        for (int v = 0; v < n; v++) {
            h_minus_1_cliques.push_back({v});
            clique_extensions.push_back(adj[v]);
        }
    } else {
        vector<int> current_clique;
        enumerate_cliques(current_clique, h-1, 0);
    }
}

void compute_clique_degree() {
    clique_degree.assign(n, 0);
    for (int i = 0; i < h_minus_1_cliques.size(); i++) {
        const auto& clique = h_minus_1_cliques[i];
        const auto& extensions = clique_extensions[i];
        for (int v : extensions) {
            clique_degree[v]++;
            for (int u : clique) clique_degree[u]++;
        }
    }
}

int main(int argc, char* argv[]) {
    // Use input file by user else break
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening " << argv[1] << endl; 
        return 1;
    }

    infile >> n >> m >> h;
    cout<<"h = " << h << endl;
    set<int> unique_labels;
    vector<pair<int, int>> raw_edges;
    while(!infile.eof()) {
        int u, v; // INPUT FORMAT WITHOUT WEIGHTS
        infile >> u >> v; 
        // int u, v, w; // INPUT FORMAT WITH WEIGHTS
        // infile >> u >> v >> w; // Read weights but ignore them for caida
        raw_edges.emplace_back(u, v);
        unique_labels.insert(u);
        unique_labels.insert(v);
    }
    infile.close();

    clock_t starttime = clock();

    int idx = 0;
    for (int label : unique_labels) {
        label_to_index[label] = idx++;
        index_to_label.push_back(label);
    }
    n = unique_labels.size();
    adj.resize(n);
    for (auto [u, v] : raw_edges) {
        int ui = label_to_index[u], vi = label_to_index[v];
        adj[ui].push_back(vi);
        adj[vi].push_back(ui);
    }
    for (int i = 0; i < n; i++) sort(adj[i].begin(), adj[i].end());

    cout << "Loaded graph with " << n << " vertices and " << m << " edges\n";

    generate_h_minus_1_cliques();
    // cout << "Generated " << h_minus_1_cliques.size() << " (" << (h-1) << ")-cliques\n";

    compute_clique_degree();

    double l = 0.0, u = *max_element(clique_degree.begin(), clique_degree.end());
    vector<int> best_subgraph;
    cout << "Starting binary search with bounds [" << l << ", " << u << "]\n";

    while (u - l >= 1.0 / (n * (h-1))) {
        double alpha = (l + u) / 2;
        // cout << "Trying alpha = " << alpha << endl;

        int total_nodes = 2 + n + h_minus_1_cliques.size();
        MaxFlow mf(total_nodes);
        int source = 0, sink = 1;

        for (int v = 0; v < n; v++) {
            mf.addEdge(source, 2 + v, clique_degree[v]);
            mf.addEdge(2 + v, sink, alpha * h);
        }

        for (int i = 0; i < h_minus_1_cliques.size(); i++) {
            int clique_node = 2 + n + i;
            for (int v : h_minus_1_cliques[i]) {
                mf.addEdge(clique_node, 2 + v, 1e9);
            }
            for (int v : clique_extensions[i]) {
                mf.addEdge(2 + v, clique_node, 1.0);
            }
        }

        double flow = mf.dinic(source, sink);

        vector<bool> in_S(total_nodes, false);
        queue<int> q;
        q.push(source);
        in_S[source] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto &e : mf.adj[u]) {
                if (e.cap > 1e-9 && !in_S[e.to]) {
                    in_S[e.to] = true;
                    q.push(e.to);
                }
            }
        }

        vector<int> subgraph;
        for (int v = 0; v < n; v++) {
            if (in_S[2 + v]) subgraph.push_back(v);
        }

        // cout << "Found subgraph of size " << subgraph.size() << endl;

        if (subgraph.empty()) {
            u = alpha;
        } else {
            l = alpha;
            if (subgraph.size() > best_subgraph.size() || best_subgraph.empty()) {
                best_subgraph = subgraph;
            }
        }
    }

    double density = 0;
    if (!best_subgraph.empty()) {
        int total_cliques = 0;
        set<int> subgraph_set(best_subgraph.begin(), best_subgraph.end());
        set<vector<int>> unique_cliques;

        for (int i = 0; i < h_minus_1_cliques.size(); i++) {
            const auto& clique = h_minus_1_cliques[i];
            const auto& extensions = clique_extensions[i];

            bool clique_in_subgraph = all_of(clique.begin(), clique.end(), [&](int v) {
                return subgraph_set.count(v);
            });

            if (clique_in_subgraph) {
                for (int v : extensions) {
                    if (subgraph_set.count(v)) {
                        vector<int> full_clique = clique;
                        full_clique.push_back(v);
                        sort(full_clique.begin(), full_clique.end());
                        unique_cliques.insert(full_clique);
                    }
                }
            }
        }

        density = (double)unique_cliques.size() / best_subgraph.size();
    }

    clock_t endtime = clock();
    double elapsed_time = (double)(endtime - starttime) / CLOCKS_PER_SEC;
    cout << "Elapsed time: " << elapsed_time << " seconds\n";
    cout << "Densest subgraph size: " << best_subgraph.size() << endl;
    cout << "Density: " << density << endl;
    cout << "Nodes:";
    for (int v : best_subgraph) {
        cout << " " << index_to_label[v];
    }
    cout << endl;

    return 0;
}
