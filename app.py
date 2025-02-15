from flask import Flask, render_template

app = Flask(__name__)

# Student Data
students = [
    {"name": "Simran Sesha Rao", "roll_no": "2022A7PS0002H"},
    {"name": "Simran Singh", "roll_no": "2022A7PS0003H"},
    {"name": "Shreya Kunjankumar Mehta", "roll_no": "2022A7PS0115H"},
    {"name": "Sukhbodhanand Tripathi", "roll_no": "2022A7PS0187H"},
    {"name": "Granth Bagadia", "roll_no": "2022A7PS0217H"},
]

# Graph Algorithms Report
graph_algos = [
    {"name": "Dijkstra's Algorithm", "complexity": "O(V^2) or O(E + V log V)", "runtime": "Efficient for shortest path", "source": "dijkstra"},
    {"name": "Kruskal's Algorithm", "complexity": "O(E log E)", "runtime": "Used for Minimum Spanning Tree", "source": "kruskal"},
    {"name": "Floyd-Warshall Algorithm", "complexity": "O(V^3)", "runtime": "All-pairs shortest paths", "source": "floyd_warshall"}
]

@app.route('/')
def home():
    return render_template('index.html', students=students, graph_algos=graph_algos)

@app.route('/source/<algo_name>')
def source(algo_name):
    algo_sources = {
        "dijkstra": "# Dijkstra's Algorithm\ndef dijkstra(graph, src):\n    pass",
        "kruskal": "# Kruskal's Algorithm\ndef kruskal(edges, nodes):\n    pass",
        "floyd_warshall": "# Floyd-Warshall Algorithm\ndef floyd_warshall(graph):\n    pass"
    }
    source_code = algo_sources.get(algo_name.lower(), "Algorithm not found")
    return render_template('source.html', algo_name=algo_name, source_code=source_code)

if __name__ == '__main__':
    app.run(debug=True)
