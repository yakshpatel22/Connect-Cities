# Connect-Cities
A  C++ program designed to build roads connecting cities represented as vertices in a terrain. It uses a greedy algorithm to determine the best, immediate road options to connect cities. Below is a description of what the code does, with a focus on data structure and algorithm (DSA) concepts:

Description:-
This C++ program reads terrain data from a text file and uses a greedy algorithm to build roads connecting cities within the terrain. The primary tasks and functionalities of this program include:

1. File Input and Parsing: The program takes the name of a terrain data input file as a command-line argument and reads data about the terrain, including the number of vertices, edges, and cities.
2. City and Vertex Data Processing: The code reads information about each vertex (location) and city, including their coordinates and properties (city or non-city).
3. Road (Edge) Construction: It constructs roads (edges) to connect cities within the terrain while minimizing the overall cost. The algorithm iteratively selects the next city to connect, considering the distance to the destination city and the cost of the road.
4. Output Generation: The program generates an output text file, which includes information about the terrain with the newly built roads.
5. Calculating Distances: The program calculates distances between vertices using Euclidean distance. This is a fundamental DSA concept.
6. Greedy Algorithm: The core algorithm used in the program is a greedy algorithm. It makes choices that are locally optimal at each step to reach a global optimal solution.
7. Data Structures: The code uses data structures like vectors to store information about roads and their costs.

What I've Learned:
1. Greedy Algorithm: I've learned how to implement and apply a greedy algorithm for solving a problem. Greedy algorithms make the best choice at each step, resulting in a solution that is often close to optimal.
2. Data Processing: I've gained experience in reading and processing data from text files. This is a fundamental skill in software development.
3. Vector Usage: The code uses vectors to manage and store road information efficiently. Understanding how to work with data structures is crucial in DSA.
4. Distance Calculation: The program demonstrates distance calculation using the Euclidean distance formula, a fundamental concept in DSA.
5. Algorithmic Thinking: Building roads between cities demonstrates algorithmic thinking and problem-solving, core aspects of computer science.
6. Optimization: The program optimizes the choice of roads to minimize costs, which is a key concept in algorithm design.
7. File I/O: I've learned about reading from and writing to files, an essential skill for working with data in real-world applications.

This project combines DSA concepts, algorithmic thinking, and practical programming skills to solve a real-world problem of connecting cities with roads while optimizing costs. It serves as an excellent example of applying data structures and algorithms to address a meaningful task.
