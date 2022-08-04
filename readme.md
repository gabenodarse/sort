### PREFACE
I first discovered this algorithm a few years ago when heavily analyzing the problem of sorting. I recommend reading through this readme before trying to read through the code; the coes uses several linked lists, branching if statements, a few gotos, and 500 lines to actualize an idea which is more elegant. The code is not as difficult to understand once the algorithm is understood.

### BENCHMARKS
Here are the benchmarks on my machine.

|Number of Elements|Quick sort|1-2 pivot sort|2-3 pivot sort|
|---|---|---|---|
|2^20|0.163|0.271|0.215|
|2^22|1.194|1.604|1.328|
|2^24|13.300|8.940|7.529|
|2^26|189.04|51.950|41.997|

### OBSERVATIONS
The benchmarks evidence that 1-2 pivot sort has a significantly lower coefficient for the linearithmic scaling "nlog(n)" term than quicksort, but also a significantly larger overhead coefficient for the linear scaling "n" term than quicksort. The results are not adjusted for cache miss rates or pipelineability or any other processor tricks.

### HOW IT WORKS

Let's preface the explanation by saying that quicksort on random values is a very fast algorithm. It only falters because pivots don't always happen to be near the center. Consider that half the time the choice of pivot will either fall below 25% of elements or above 75% of elements, resulting in imbalanced partitions. Choosing a median of 3 or 9 or whatever does not help with this problem (a median of 3 is statistically as likely to result in a bad median if the array is large). What would help would be the medians of medians algorithm used in conjunction with quick sort, but that is a sufficiently complicated algorithm that the overhead would be very high.

1-2 pivot sort at its core is quick sort. 

The algorithm starts by choosing a pivot. (For initialization purposes three elements are chosen, because partitions must be non-zero in length. The 3 elements are sorted, and element 2 is made a pivot while elements 1 and 3 are put into partitions). The algorithm then proceeds similarly to quicksort, with elements compared against the pivots and then placed in the correct partitions.

The heart of 1-2 pivot sort is what happens when partition sizes are imbalanced. When one partition contains more than 2/3 of the placed elements, that partition is split by a new pivot. As a result there are 2 pivots and 3 partitions, each partition expected to hold 1/3 of the elements. Two pivots can also be converted into one: when over 1/2 of elements fall into any one partition, a pivot is taken away. When there are three partitions and over 1/2 of the elements falls in the first partition, the second pivot is taken away, resulting in two partitions, each roughly containing 1/2 the elements, separated by 1 pivot. Similarly, when over 1/2 of elements fall in the third partition, pivot 1 is taken away and partitions 1 and 2 are merged. When over 1/2 of elements fall in the middle partition, both old pivots are discarded and a pivot from the old middle partition is chosen to separate the elements into two new partitions.

Adding/removing pivots poses an obvious problem: what happens to elements that were sorted before a change of pivot? The solution is soft partitions. Soft partitions store elements which don't have complete knowledge of their sorted position in relation to all pivots. In 1-2 pivot sort these soft partitions are gtf2 (greater than the first of 2 pivots, i.e. in either partition 2 or 3), ltl2 (less than the last of 2 pivots, i.e. in either partition 1 or 2), and none (no information on where these elements are in relation to the pivots, i.e. essentially unsorted.) In this way maximum information is retained when pivots are exchanged. Once all elements are sorted and the pivots are set in stone, elements in soft partitions are then placed into hard partitions. 

Despite displacing elements when adding and removing pivots, the algorithm is guaranteed to run in linearithmic time. This is because 1. it is guaranteed that more elements are placed than displaced 2. no partition can be too large because large partitions are split

Partitions are implemented as linked lists, allowing for them to be appended to one another trivially.

A fun observation is that promoting more pivot swaps speeds up the algorithm. This is despite swapping pivots being a high overhead operation and despite displaced elements have to go through redundant comparisons. Pivot swaps are promoted by resetting the partition estimated sizes after each swap of pivot, so it is easier for a partition to accumulate the threshold amount of elements for a pivot change.

For very large arrays, it is recommended to find a second processor and parallelize merge sort over actually using 1-2 pivot sort.
