datastructures_C--11
====================

What this library is:
To you, this is a well tested carefully developed robust set of libraries.
It includes some algorithms which are rarely seen correctly in their full
implementation. My goal with this library is to try and write the best possible
version of various datastructures and algorithms. It's a place for me to play
with ideas, both in library APIs for these algorithms as well as testing
various implementation details.

How to use it:
First, notice there's a "dict" and then other dict implementations...
"Dict" is intended as a simple interface hiding most of the details
of the underlying implementation. The backing implementation has been
chosen based on benchmarking and other tests. If on the other hand you want
to get in to the details (e.g. using external allocation etc.) you can bypass
the more abstract interfaces lke "dict" "set" or "queue"
There are multiple implementations of some algorithms. I want to keep around 
the alternate implementations, but for each problem I'm selecting the best 
algorithm for common use. Notes are included in headers of several, for example 
AVL vs. RB trees (see notes below).

What this library is NOT:
Readability is often secondary to speed in this library. To compare algorithms
I have to write the fastest versions I can, often trading off readability. This
is crazy fun research time, not enterprize development time. My intent is to 
unittest the HECK out of these so they are extremely reliable, to accomplish 
the goals of correctness.

Readability notes:
Some experiments are actually experiments in readability as well though,
rredblack is such an experiment. The question was "How does an SML style 
implementation of a red black tree compare in readability vs. performance". 
The answer is "Not bad, but not great either"... The idea was to consider 
the utility of immutable datastores, but the result is actually a rather
interesting, extremely legible, implementation with lower storage requirements
at the cost of speed and stack usage.

Performance and "Best" versions of things:
This started with a simple linked-list, which after much thought in college I
realized HAS a best implementation (meaning there are no tradeoffs to be made
with the possible exception of thread-safety). 

I then went looking to see if the same existed for other datastructures. There
is not from what I can tell, although you can get close, as there are fewer
decisions to be made than most believe (e.g. if a datastructure CAN be 
implemented with external allocation it should be, because you can wrap it 
trivially to make it internally allocated anyway). 

This library has also been used for my personal investigations in to which
datastructures are *actually* the fastest. Apparently I was scooped by a few 
years, but this library proves that RB trees are slower than AVL trees (It has 
an *extremely* optimized RB-tree, which is still beaten by a far simpler AVL tree).

So, this is my personal playground for experimenting with datastructures and
language features in an attempt to write the best simple datastructures I can. 

For further performance notes see the file "PERFORMANCE"

Known deficiencies:
  - The array types here are kind of silly... vector would be better due to often
  having native compiler support and features like move semantics and "emplace"
  already idealy implemented. std::array replaces my static arrays as well.
  - Move semantics particularly impact the dict instantiation of btree, as this
  stores an actual object, and the object is reconstructed whenever it's moved,
  btree does not use array.h though, so it needs to be fixed seperately
