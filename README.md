# Graph Algorithms Project

This repository contains the C/C++ implementations of three graph algorithms along with experimental observations using three datasets. The algorithms have been optimized using the `-O3` flag. Detailed descriptions of the algorithms and experimental results are provided in the report.

## Repository Structure

```
project-root/
├── code/
│   ├── els.cpp        // ELS algorithm implementation
│   ├── tomita.cpp     // Tomita algorithm implementation
│   └── chiba.cpp      // Chiba algorithm implementation

├── static/
│   └── files/
│       ├── report.pdf // Detailed report (algorithms description and experimental observations)
│       └── ...    
└── README.md
```

## Dataset Preparation

1. **Download the Datasets:**

   - [Email-Enron dataset](https://snap.stanford.edu/data/email-Enron.html)
   - [AS-Skitter dataset](https://snap.stanford.edu/data/as-Skitter.html)
   - [Wiki-Vote dataset](https://snap.stanford.edu/data/wiki-Vote.html)

2. **Extract and Organize Files:**

   - Download the ZIP file for each dataset.
   - Unzip each file.
   - Locate the text file containing the graph data from each dataset.

3. **Place and Rename Files:**

   - Move the text files into the `code` directory.
   - Rename the files as follows:
     - Enron dataset → `enron.txt`
     - Wiki-Vote dataset → `wiki.txt`
     - AS-Skitter dataset → `skitter.txt`

4. **File Format Adjustments:**

   - **Remove Comments:**  
     Remove any comments at the top of each file.
     
   - **Set Header Format:**  
     Ensure the first non-comment line contains the number of nodes and edges in the following format:
     ```
     NODES    EDGES
     ```
     For example, for `WIKI.TXT`, the file should begin with a line similar to:
     ```
     7115    103689
     ```
     followed by the list of edges:
     ```
     30    1412
     30    3352
     ...
     ```
    The actual node and edge values are provided in the original TXT file.

## Compilation Instructions

1. **Navigate to the `code` Directory:**

   ```bash
   cd code
   ```

2. **Modify Dataset File Names in Source Files:**

   - **ELS Algorithm (`els.cpp`):**
     - Open `els.cpp` and go to **line 288**.
     - Change the dataset file name to one of: `skitter.txt`, `enron.txt`, or `wiki.txt` as needed.
     
   - **Tomita Algorithm (`tomita.cpp`):**
     - Open `tomita.cpp` and go to **line 150**.
     - Change the dataset file name to one of: `skitter.txt`, `enron.txt`, or `wiki.txt` as needed.
     
   - **Chiba Algorithm (`chiba.cpp`):**
     - Open `chiba.cpp` and go to **line 14**.
     - Change the dataset file name to one of: `skitter.txt`, `enron.txt`, or `wiki.txt` as needed.

3. **Compile the Source Files with g++:**

   Use the `-O3` optimization flag when compiling:

   - Compile `els.cpp`:
     ```bash
     g++ -O3 -o els els.cpp
     els.exe (Windows)
     ./els.exe (Unix)
     ```
   - Compile `tomita.cpp`:
     ```bash
     g++ -O3 -o tomita tomita.cpp
     tomita.exe (Windows)
     ./tomita.exe (Unix)
     ```
   - Compile `chiba.cpp`:
     ```bash
     g++ -O3 -o chiba chiba.cpp
     chiba.exe (Windows)
     ./chiba.exe (Unix)
     ```


## Individual Contributions

- **Simran Rao and Simran Singh:**  
  - Developed the worst-case time complexity for generating all maximal cliques and computational experiments by Etsuji Tomitaa, Akira Tanakaa and Haruhisa Takahashi

- **Shreya Kunjankumar Mehta**  
  - Developed the Arboricity and Subgraph Listing Algorithms by Norishige Chiba and Takao Nishizekvf

- **Sukhbodhanand Tripathi & Granth Bagadia:**  
  - Developed the Listing All Maximal Cliques in Sparse Graphs in Near-optimal Time Algorithm by David Eppstein, Maarten Loffler, and Darren Strash
  - Created the project webpage.
  
## Project Webpage

For further details, experimental observations, and updates, please visit our project webpage:  
[https://daa-graphs.onrender.com](https://daa-graphs.onrender.com)

## Detailed Report

A comprehensive [report](https://github.com/granth23/DAA-Graphs/blob/main/static/files/daa-report.pdf) that includes descriptions of the algorithms and experimental observations is available at:  
`static/files/daa-report.pdf`
