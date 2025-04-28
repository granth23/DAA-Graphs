# Graph Algorithms Project

This repository contains the C/C++ implementations of three graph algorithms along with experimental observations using three datasets. The algorithms have been optimized using the `-O1` flag. Detailed descriptions of the algorithms and experimental results are provided in the report.

## Repository Structure

```
project-root/
├── code/
│   ├── exact.cpp          // Exact algorithm implementation
│   └── coreexact.cpp      // CoreExact algorithm implementation
├── static/
│   └── files/
│       ├── report.pdf // Detailed report (algorithms description and experimental observations)
│       └── ...    
└── README.md
```

## Dataset Preparation

1. **Download the Datasets:**

   - [Link to all Datasets used](https://drive.google.com/drive/folders/1LbEqNUr8AyXq-TrD-QFGfzB5xvncFHc_?usp=drive_link)

2. **File Format**

   - Make sure the files are formatted well in the format of 

     ```bash
     <nodes> <edges> <h>
     <u> <v>
     <u> <v>
     ...
     ```


## Compilation Instructions

1. **Navigate to the `code` Directory:**

   ```bash
   cd code
   ```

2. **Compile the Source Files with g++:**

   Use the `-O1` optimization flag when compiling:

   - Compile `exact.cpp`:
     ```bash
     g++ -O1 -o exact exact.cpp
     exact.exe <filename.txt> (Windows)
     ./exact.exe <filename.txt>(Unix)
     ```
   - Compile `coreexact.cpp`:
     ```bash
     g++ -O1 -o coreexact coreexact.cpp
     coreexact.exe <filename.txt> (Windows)
     ./coreexact.exe <filename.txt>(Unix)
     ```


## Individual Contributions

- **Simran Rao, Simran Singh and Shreya Kunjankumar Mehta:**  
  - Developed the CoreExact algorithm.
  - Created the project report.

- **Sukhbodhanand Tripathi & Granth Bagadia:**  
  - Developed the Exact Algorithm
  - Created the project webpage.
  
## Project Webpage

For further details, experimental observations, and updates, please visit our project webpage:  
[https://daa-graphs.onrender.com](https://daa-graphs.onrender.com)

## Detailed Report

A comprehensive [report](https://github.com/granth23/DAA-Graphs/blob/main/static/files/report.pdf) that includes descriptions of the algorithms and experimental observations is available at:  
`static/files/report.pdf`
