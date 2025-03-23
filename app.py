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
    {"name": "Tomita's Algorithm", "complexity": "O\\left(3^{\\frac{n}{3}}\\right)", "source": "Tomita"},
    {"name": "ELS's Algorithm", "complexity": "O\\left(d n 3^{\\frac{d}{3}}\\right)", "source": "ELS"},
    {"name": "Chiba's Algorithm", "complexity": "O\\left(a G(m)\\right)", "source": "Chiba"}
]


@app.route('/')
def home():
    return render_template('index.html', students=students, graph_algos=graph_algos)

@app.route('/source/<algo_name>')
def source(algo_name):
    filename = f"{algo_name.lower()}.txt"
    try:
        # Open and read the content of the file
        with open(filename, "r") as file:
            source_code = file.read()
    except FileNotFoundError:
        source_code = "Algorithm not found"
    return render_template('source.html', algo_name=algo_name, source_code=source_code)

if __name__ == '__main__':
    app.run(debug=True)
