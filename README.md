# Data Structure and Algorithm: Connecting Towns

This was a project written in C++ for the course _Data Structure and Algorithm_ in my University. The goal of the project is to implement a graph data structure, with different Graph Traversal and Path Finding algorithms. The program can be opened as a [QtCreator Project](https://www.qt.io/product/development-tools) and compile using Qt's Compiler.

## How to use the program?

First, open the `prog2` directory as a new QtCreator project. Then, build and Run the program in QtCreator. You can use the provided files as input for the commands.
The available commands are in the `datastructures.hh` header file. The file also includes brief time complexity estimation for each of the command.

You can also choose the command by selecting in the drop-down menu `Command:` on the interface.

## Algorithm highlight

The program implemented a non-directed tree data structure, with each node represent a town, and towns are connected with roads for the town-road relationship (road tree). It also has a directed tree data structure, that represents the tax master - tax vassals relationships between each town (tax tree).

To traverse the directed tax tree, I used Breadth First Search and Depth First Search. These t2 algorithms are sufficient enough for the desired result of simply traversing down the tree.

For the road tree, user might want to find the shortest route between 2 towns. The path finding is done using Dijkstra's Algorithm. The project also introduces a Minimal Cost Spanning Tree problem, which wa solved using Kruskal's Algorithm.

## What did I learn?

I learned how to implement different types of data structures (list, linked list, stack, queue, deck, array, heap, binary tree, graph, map, hash map, etc.), how they work, what are the differences between them, and the time complexity of different operations performed on the given data structure.

I learned many algorithms, such as sorting algorithm, how they work, and their time complexity (bubble sort, quick sort, binary sort, merge sort, etc.). I also learned about graph and tree algorithms, such as traversing algorithm, merging, reversing, path finding algorithms.

All in all, this course greatly enhance my knowledge in coding with C++. I got familiar with pointers, how to design a sufficient data structure that suits my need, and recursive functions. I also learned a little Functional Programming, which later helps me a lot with programming in JavaScript and Functional Programming in Haskell.

## Short explanation of tests in prg2 (phase 2)

- test-skeleton-in/out.txt: Test functionality required for submission in Plussa
- smalltest-\*-in/out.txt: A really simple minimal tests for operations (divided into separate test files, because most operations are non-compulsory)
- example-\*-in/out.txt: The assignment specification's example, divided into separate files for each operation
- biggertest-\*-in/out.txt: Bigger tests testing operations with town data from towns-data.txt (divided into separate test files, because most operations are non-compulsory)
- example-in/out.txt: The assignment specification's example run
- perftest-\*.txt: These tests test the performance of various operations with growing number of towns and roads, using the main program perftest command

## Short explanation of tests in prg1 (phase 1)

- test-skeleton-in/out.txt: Test functionality required for skeleton (raakile) submissions (if you do only phase 1). This test **has to pass** correctly before Plussa accepts your submission (regardless of whether you just submit a skeleton or complete phase 1)
- simpletest-in/out.txt: A really simple minimal test containing all compulsory operations
- example-compulsory-in/out.txt: The assignment specification's example, but with only compulsory operations
- basictest-in/out.txt: A little bigger test testing compulsory operations with the town data in the assignment specification example
- biggertest-in/out.txt: A bigger test testing compulsory operations with town data from towns-data.txt an towns-vassals-data.txt
- noncompulsory-in/out.txt: A bigger test testing non-compulsory operations with town data from towns-data.txt an towns-vassals-data.txt
- example-in/out.txt: The assignment specification's example run
- perftest-\*.txt: These tests test the performance of various operations with growing number of towns, using the main program perftest command
