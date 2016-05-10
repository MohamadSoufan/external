# external
Example of POSIX message queues in C++. Creates a tree structure of 6 nodes where the root node passes its inital value down. Its children average that with their own then pass it down. Their children find the average again but then pass this up since they are at the last level. The average is computed and passed up until it reached the root node, completing a round. The root node checks if the value passed to it is within 0.01 of its previous value from the last round. If so, the root node sends a kill message which is propagated down to its the children where the children display their final value and die.

## Compile
Run `qmake` then `make`. Can also be compiled just using gcc via `g++ -lrt -Wall -o external main.cpp` but `launch.sh` will need to be modified to skip the build and launch the right binary.
