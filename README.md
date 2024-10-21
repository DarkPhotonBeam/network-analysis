# Network Analysis
A series of tools for analyzing networks / graphs.

## seq_analysis
Input: 
space-seperated positive integers
Output: 
Determines if the sequence of integers is graphic (automatically sorts them).
If graphic analyzes the graph it represents and gives relevant information / stats.

### Example Graphs
There are example graphs in the `graphs` directory.
Use them like `./seq_analysis < ./graphs/<somegraph>.txt`

### Generate .graphlml File
By adding the flag `-g` to the binary, for example `./seq_analysis -g < ./graphs/fish.txt` the program
will generate a .graphml file (`out.graphml`) using the Havel-Hakimi-Algorithm.

## How to build
`make`
