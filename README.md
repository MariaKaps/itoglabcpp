# Media catalog - Final Lab

A C++ console application that implements the media catalog to encrypt and decrypt texts. It supports Russian and English alphabets, works with data in JSON format, logs operations, and provides a convenient text interface. Description

**Variant:** 10  
**Language:** C++  
**Standard:** C++17  

---
The project implements a small jSON database system, including:

data storage and download;
adding new records;
basic search by title and author
faster tag search via N-gram;
checking the database for duplicates and incorrect entries;
random data generator for testing;
benchmarking of performance.
Features
Displaying the entire database
Displays all column-formatted records.

Add new data
Adding a record with the following fields: id, title, author, year, tags, rating.

Basic search

Search by title and author with the query lowercased.
The results are sorted and the top N by rating can be displayed.
Search by tags (accelerated)

Uses the n-gram index for quick tag searches.
Allows you to execute thousands of queries in a fraction of a millisecond on a large database.
Database Validation

Displaying invalid entries with an indication of the reason (duplicate id, duplicate name, invalid fields).
Enter the number: 
Benchmarking

Estimating the search execution time.
Support for two modes: normal search and tag search.
Displays the average run time for a given number of runs.
Help

A quick summary of the available features of the program.
Test data generator (see scripts folder)

Generate random entries or choose from pre-prepared options.
Build and run
Clone the repository:
git clone https://github.com/usrnmeee/LR
cd project
Assembly via g++ (example for Linux/macOS):
g++ -std=c++17 src/*.cpp -o database_app
Starting the program:
./database_app
Example of work
Choose an action:
1 - Show full catalog
0 - Exit
3 - Filter by Tag
4 - Top-N by rating
5 - Find duplicates
6 - Show Stats
7 - Save Catalog
8 - Add New Record
0 - Exit
Enter the number: 
