SQL Threads Simulator
====================
A old (and simple) sql-simulator made during my time at the University.
We use a Round-robin scheduling to distribute queries in the threads.

## Warning: 
This is not a good code to use as an example.
So many things are outdated and poorly programmed.

## Usage
- Inside the db folder you must create the tables with information (AKA plain texts files).
- The name of the file is the name of the table.
- The tables are simple key-value store.
- Inside the db folder there is a example of a table.
- Allowed statements:
	- insert tabla_a (1020, "delete elemento a 1020")
	- select tabla_b att="agregar elemento a 1161"
	- update tabla_a (1020, "cambia el valor anterior")
	- delete tabla_b att="agregar elemento a 1161"

## Compile and Run

```
To compile just run the MakeFile
./minisql -t [thread_number] -q [query_file] -o [output_file]
Example : ./minisql -t 10 -q 1 -o 2
```