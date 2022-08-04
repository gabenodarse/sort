// Gabe Nodarse

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "quicksort.h"

#define ARRAY_SIZE 1048576

// ~~~~~ UNDERSTANDING THE CODE ~~~~~

// struct partition defines partitions. Partitions split up the array (to be sorted). Partitions are simply defined by the entryValIdx, 
	// the index of the last element added to the partition, and the initValIdx, the index of the first element added to the partition.
	// Rather than storing a pointer to an array of elements, struct partition is defined as such because the elements are linked 
	// (forming a linked list). By keeping track of both the initial and entry indices, partitions can easily be appended to another.
	
// struct linkedVal defines values that are within a partition. A linked value contains the actual value of the element, and the index
	// of the next value in the linked list. If there is no next value, the nextValIdx is set to an index of -1
	
// function twoThreePivotSort is the entry point to the algorithm. All that happens here is that an initial partition is defined. This
	// "partition" contains all elements of the array to be sorted. This initial partition is defined because the algorithm sorts
	// partitions, not arrays

// ~~~~~ function __twoThreePivotSort ~~~~~

// with all the elements copied over from the original array, the original array can be altered in place. Thus, all sorted elements
	// are placed directly into their correct location in the original (argument vals). Argument destinationIdx defines the location where
	// the partition is placed. The partition containing all elements is placed at destination index 0. All further partitions are placed
	// at their designated destination index, e.g. for a partition of elements greater than a pivot at destination index 50, the
	// destination index of the partition is 51.
	
// the first part of this function extracts 3 elements from the partition: 1 for the pivot, and 1 for each of the sub partitions. 
	// 3 or fewer elements in the partition represent the "base case" in which the function can return immediately
	// (after bubble sorting the elements)

// next the pivots (both for 1 and 2 pivot cases) and sub partitions (again for both 1 and 2 pivot cases) are declared.
	// zeroPartition is an empty partition used to blank out partitions which are meant to be empty
	// none is a partition for elements that have no knowledge of where they fall in relation to the pivots. (this partition is
		// populated when pivots are swapped in and out and elements which used to be sorted relative to an old pivot are no longer sorted)
	// gtf2 and ltl2 are "soft partitions", for elements where there is some knowledge of where they fall relative to pivots, but not
		// complete knowledge. (gtf2 - greater than the first of two pivots, ltl2 - less than the last of 2 pivots)
		
// next the 3 extracted elements are sorted and assigned to either a pivot or partition

// the "estimated" variables are for keeping track of how many elements fall in each partition. If too many fall into a partition, the
	// number of pivots is swapped in hopes for a more evenly distributed number of elements in each partition
	
// ONE_PIVOT: the loop breaks as soon as there are no more elements in a partition to be sorted. Within the loop the first step is to find
	// the sub partition that an element falls in. Once the subpartition is found, a check occurs to see if too many elements are in the
	// the sub partition. If there are not too many elements, add the element to the subpartition and update tallies of elements in the
	// partition. If there are too many elements, swap to two pivots (nitty gritty details excluded, see: code)
	
// TWO_PIVOTS: similar to ONE_PIVOT

// END_ONE_PIVOT and END_TWO_PIVOTS: first, sort all elements from none and the soft partitions, so that all elements now have a place in
	// in relation the pivot(s). Then, recursively call __twoThreePivotSort to sort the sub partitions in order, keeping track of how 
	// many elements were placed so the destination index remains accurate. Pivots are also placed directly into their destination, once
	// their destination is determined

struct partition{
	int initValIdx;
	int entryValIdx;
};

struct linkedVal{
	int val;
	int nextValIdx;
};

typedef struct partition partition;
typedef struct linkedVal linkedVal;

void twoThreePivotSort(int* vals, int len, linkedVal* linkedValsArray);

int main(){
	int* vals;
	int* vals2;
	
	vals = (int*) malloc( sizeof(int) * ARRAY_SIZE );
	vals2 = (int*) malloc( sizeof(int) * ARRAY_SIZE );
	
	srand(time(NULL));
	for(int i = 0; i < ARRAY_SIZE; i++){
		vals[i] = rand();
		vals2[i] = vals[i];
	}
	
	// sort and time
	clock_t timeStart;
	clock_t timeEnd;
	double timeTaken;
	
	timeStart = clock();
	quickSort(vals, ARRAY_SIZE);
	timeEnd = clock();
	timeTaken = (double)(timeEnd - timeStart) / CLOCKS_PER_SEC;
	printf("Time taken: %f\n", timeTaken);
	
	linkedVal* linkedValsArray = (linkedVal*) malloc( sizeof(linkedVal) * ARRAY_SIZE );
	timeStart = clock();
	twoThreePivotSort(vals2, ARRAY_SIZE, linkedValsArray);
	timeEnd = clock();
	timeTaken = (double)(timeEnd - timeStart) / CLOCKS_PER_SEC;
	printf("Time taken: %f\n", timeTaken);
	
	// validate
	for(int i = 0; i < ARRAY_SIZE; i++){
		assert(vals[i] == vals2[i]);
	}
	
	printf("sorted");
	free(linkedValsArray);
	free(vals);
	free(vals2);
}

int __twoThreePivotSort(int* vals, int destinationIdx, int idx, linkedVal* linkedValsArray);

void twoThreePivotSort(int* vals, int len, linkedVal* linkedValsArray){
	for(int i = 0; i < len; i++){
		linkedValsArray[i].nextValIdx = i-1;
		linkedValsArray[i].val = vals[i];
	}
	
	__twoThreePivotSort(vals, 0, len-1, linkedValsArray);
}

// returns how many elements were placed
int __twoThreePivotSort(int* vals, int destinationIdx, int idx, linkedVal* linkedValsArray){
	
	int indices[3];
	for(int i = 0; i < 3; ++i){
		indices[i] = idx;
		idx = linkedValsArray[idx].nextValIdx;

		if(idx == -1){
			// bubble sort
			for(int j = 0; j < i; ++j){
				for(int k = i; k > j; --k){
					if(linkedValsArray[indices[k]].val < linkedValsArray[indices[k-1]].val){
						int tmp = indices[k-1];
						indices[k-1] = indices[k];
						indices[k] = tmp;
					}
				}
			}
			
			for(int j = 0; j <= i; ++j){
				vals[destinationIdx] = linkedValsArray[indices[j]].val;
				destinationIdx += 1;
			}
			
			return i+1;
		}
	}
	
	int pivot1;
	int pivot2_1;
	int pivot2_2;
	int pivot1Idx;
	int pivot2_1Idx;
	int pivot2_2Idx;
	
	partition zeroPartition;
	partition none;
	
	partition lt1; // less than pivot
	partition gt1; // greater than pivot
	
	partition lta2; // less than all 2 pivot
	partition gtf2; // greater than first 2 pivot
	partition bb2; // between both 2 pivot
	partition ltl2; // less than last 2 pivot
	partition gta2; // greater than all 2 pivot
	
	// initialize soft partitions
	zeroPartition.entryValIdx = -1;
	none.entryValIdx = -1;
	
	gtf2.entryValIdx = -1; // >:< ?????
	ltl2.entryValIdx = -1;
	
	// initialize first pivot and first partitions
	{
		// bubble sort
		for(int i = 0; i < 2; ++i){
			for(int j = 2; j > i; --j){
				if(linkedValsArray[indices[j]].val < linkedValsArray[indices[j-1]].val){
					int tmp = indices[j-1];
					indices[j-1] = indices[j];
					indices[j] = tmp;
				}
			}
		}
		
		lt1.initValIdx = indices[0];
		lt1.entryValIdx = indices[0];
		linkedValsArray[indices[0]].nextValIdx = -1;
		gt1.initValIdx = indices[2];
		gt1.entryValIdx = indices[2];
		linkedValsArray[indices[2]].nextValIdx = -1;
		
		pivot1 = linkedValsArray[indices[1]].val;
		pivot1Idx = indices[1];
	}
	
	int estimatedBelowP1;
	int estimatedAboveP1;
	
	int estimatedBelowPf;
	int estimatedBetweenP1P2;
	int estimatedAbovePl;
	
	int estimatedElementsPlaced;
	
	// 1 pivot
	{
		ONE_PIVOT:
		estimatedBelowP1 = 1;
		estimatedAboveP1 = 1;
		estimatedElementsPlaced = 2;
		while(1){
			if(idx < 0){
				goto END_ONE_PIVOT;
			}
			int idxNext = linkedValsArray[idx].nextValIdx;
			
			// find which partition the value falls into
			if(linkedValsArray[idx].val < pivot1){
				if(estimatedBelowP1 <= estimatedElementsPlaced * 2 / 3){
					linkedValsArray[idx].nextValIdx = lt1.entryValIdx;
					lt1.entryValIdx = idx;
					estimatedBelowP1 += 1;
				} else {
					//create new pivot and partitions using the newly inserted value and 2 values from lt1
					int indices[3] = {idx, lt1.entryValIdx, linkedValsArray[lt1.entryValIdx].nextValIdx};
					lt1.entryValIdx = linkedValsArray[indices[2]].nextValIdx;
					
					// bubble sort the 3 values
					for(int i = 0; i < 2; ++i){
						for(int j = 2; j > i; --j){
							if(linkedValsArray[indices[j]].val < linkedValsArray[indices[j-1]].val){
								int tmp = indices[j-1];
								indices[j-1] = indices[j];
								indices[j] = tmp;
							}
						}
					}
					
					pivot2_1Idx = indices[1];
					pivot2_1 = linkedValsArray[indices[1]].val;
					lta2.initValIdx = indices[0];
					lta2.entryValIdx = indices[0];
					linkedValsArray[indices[0]].nextValIdx = -1;
					bb2.initValIdx = indices[2];
					bb2.entryValIdx = indices[2];
					linkedValsArray[indices[2]].nextValIdx = -1;
					
					// populate the remaining partitions and pivots with values
					gtf2 = zeroPartition;
					ltl2 = lt1;
					gta2 = gt1;
					pivot2_2Idx = pivot1Idx;
					pivot2_2 = pivot1;
					
					idx = idxNext;
					
					goto TWO_PIVOTS;
				}
			} else {
				if(estimatedAboveP1 <= estimatedElementsPlaced * 2 / 3){
					linkedValsArray[idx].nextValIdx = gt1.entryValIdx;
					gt1.entryValIdx = idx;
					estimatedAboveP1 += 1;
				} else {
					//create new pivot and partitions using the newly inserted value and 2 values from gt1
					int indices[3] = {idx, gt1.entryValIdx, linkedValsArray[gt1.entryValIdx].nextValIdx};
					gt1.entryValIdx = linkedValsArray[indices[2]].nextValIdx;
					
					// bubble sort the 3 values
					for(int i = 0; i < 2; ++i){
						for(int j = 2; j > i; --j){
							if(linkedValsArray[indices[j]].val < linkedValsArray[indices[j-1]].val){
								int tmp = indices[j-1];
								indices[j-1] = indices[j];
								indices[j] = tmp;
							}
						}
					}
					
					pivot2_2Idx = indices[1];
					pivot2_2 = linkedValsArray[indices[1]].val;
					bb2.initValIdx = indices[0];
					bb2.entryValIdx = indices[0];
					linkedValsArray[indices[0]].nextValIdx = -1;
					gta2.initValIdx = indices[2];
					gta2.entryValIdx = indices[2];
					linkedValsArray[indices[2]].nextValIdx = -1;
					
					// populate the remaining partitions and pivots with values
					lta2 = lt1;
					gtf2 = gt1;
					ltl2 = zeroPartition;
					pivot2_1Idx = pivot1Idx;
					pivot2_1 = pivot1;
					
					idx = idxNext;
					
					goto TWO_PIVOTS;
				}
			}
			
			idx = idxNext;
			estimatedElementsPlaced += 1;
		}
	}
	
	// 2 pivots
	{
		TWO_PIVOTS:
		estimatedBelowPf = 1;
		estimatedBetweenP1P2 = 1;
		estimatedAbovePl = 1;
					
		estimatedElementsPlaced = 3;
		while(1){
			if(idx < 0){
				goto END_TWO_PIVOTS;
			}
			
			int idxNext = linkedValsArray[idx].nextValIdx;
			
			// find which partition the value falls into
			if(linkedValsArray[idx].val < pivot2_1){
				linkedValsArray[idx].nextValIdx = lta2.entryValIdx;
				lta2.entryValIdx = idx;
				
				if(estimatedBelowPf <= estimatedElementsPlaced / 2){
					estimatedBelowPf += 1;
				} else {
					// ltl2 is added to none
					if(ltl2.entryValIdx != -1){
						if(none.entryValIdx != -1){
							linkedValsArray[none.initValIdx].nextValIdx = ltl2.entryValIdx;
							none.initValIdx = ltl2.initValIdx;
						} else {
							none = ltl2;
						}
					}
					
					// set lt1
					lt1 = lta2;
					
					// set gt1
					gt1 = gta2;
					linkedValsArray[gt1.initValIdx].nextValIdx = bb2.entryValIdx;
					if(gtf2.entryValIdx != -1){
						linkedValsArray[bb2.initValIdx].nextValIdx = gtf2.entryValIdx;
						gt1.initValIdx = gtf2.initValIdx;
					} else {
						gt1.initValIdx = bb2.initValIdx;
					}
					
					
					// remove second pivot
					linkedValsArray[pivot2_2Idx].nextValIdx = gt1.entryValIdx;
					gt1.entryValIdx = pivot2_2Idx;
					pivot1 = pivot2_1;
					pivot1Idx = pivot2_1Idx;
					
					idx = idxNext;
					goto ONE_PIVOT;
				}
			} else if(linkedValsArray[idx].val < pivot2_2){
				if(estimatedBetweenP1P2 <= estimatedElementsPlaced / 2){
					linkedValsArray[idx].nextValIdx = bb2.entryValIdx;
					bb2.entryValIdx = idx;
					estimatedBetweenP1P2 += 1;
				} else {
					// bb2, ltl2, and gtf2 get added to none
					if(none.entryValIdx != -1){
						linkedValsArray[none.initValIdx].nextValIdx = bb2.entryValIdx;
						none.initValIdx = bb2.initValIdx;
					} else {
						none = bb2;
					}
					if(gtf2.entryValIdx != -1){
						linkedValsArray[none.initValIdx].nextValIdx = gtf2.entryValIdx;
						none.initValIdx = gtf2.initValIdx;
					}
					if(ltl2.entryValIdx != -1){
						linkedValsArray[none.initValIdx].nextValIdx = ltl2.entryValIdx;
						none.initValIdx = ltl2.initValIdx;
					}
					
					// set lt1
					lt1 = lta2;
					
					// set gt1
					gt1 = gta2;
					
					// remove pivots
					linkedValsArray[pivot2_1Idx].nextValIdx = lt1.entryValIdx;
					lt1.entryValIdx = pivot2_1Idx;
					linkedValsArray[pivot2_2Idx].nextValIdx = gt1.entryValIdx;
					gt1.entryValIdx = pivot2_2Idx;
					pivot1 = linkedValsArray[idx].val;
					pivot1Idx = idx;
					
					idx = idxNext;
					goto ONE_PIVOT;
				}
			} else {
				linkedValsArray[idx].nextValIdx = gta2.entryValIdx;
				gta2.entryValIdx = idx;
				
				if(estimatedAbovePl <= estimatedElementsPlaced / 2){
					estimatedAbovePl += 1;
				} else {
					// gtf2 is added to none
					if(gtf2.entryValIdx != -1){
						if(none.entryValIdx != -1){
							linkedValsArray[none.initValIdx].nextValIdx = gtf2.entryValIdx;
							none.initValIdx = gtf2.initValIdx;
						} else {
							none = gtf2;
						}
					}
					
					// set lt1
					lt1 = lta2;
					linkedValsArray[lt1.initValIdx].nextValIdx = bb2.entryValIdx;
					if(ltl2.entryValIdx != -1){
						linkedValsArray[bb2.initValIdx].nextValIdx = ltl2.entryValIdx;
						lt1.initValIdx = ltl2.initValIdx;
					} else {
						lt1.initValIdx = bb2.initValIdx;
					}
					
					// set gt1
					gt1 = gta2;
					
					// remove first pivot
					linkedValsArray[pivot2_1Idx].nextValIdx = lt1.entryValIdx;
					lt1.entryValIdx = pivot2_1Idx;
					pivot1 = pivot2_2;
					pivot1Idx = pivot2_2Idx;
					
					idx = idxNext;
					goto ONE_PIVOT;
				}
			}
			
			idx = idxNext;
			estimatedElementsPlaced += 1;
		}
	}
	
	int valsPlaced;
	
	END_ONE_PIVOT:	
	
	// place elements from none
	idx = none.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot1){
			linkedValsArray[idx].nextValIdx = lt1.entryValIdx;
			lt1.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = gt1.entryValIdx;
			gt1.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// sort partitions

	valsPlaced = __twoThreePivotSort(vals, destinationIdx, lt1.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot1;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, gt1.entryValIdx, linkedValsArray);
	
	return valsPlaced;
	
	
	
	END_TWO_PIVOTS:
	// place elements from none
	idx = none.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot2_1){
			linkedValsArray[idx].nextValIdx = lta2.entryValIdx;
			lta2.entryValIdx = idx;
		} else if(linkedValsArray[idx].val < pivot2_2){
			linkedValsArray[idx].nextValIdx = bb2.entryValIdx;
			bb2.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = gta2.entryValIdx;
			gta2.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from gtf2
	idx = gtf2.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot2_2){
			linkedValsArray[idx].nextValIdx = bb2.entryValIdx;
			bb2.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = gta2.entryValIdx;
			gta2.entryValIdx = idx;
		}
		idx = idxNext;
	}

	// place elements from ltl2
	idx = ltl2.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot2_1){
			linkedValsArray[idx].nextValIdx = lta2.entryValIdx;
			lta2.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = bb2.entryValIdx;
			bb2.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// sort partitions
	
	valsPlaced = __twoThreePivotSort(vals, destinationIdx, lta2.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot2_1;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, bb2.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot2_2;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, gta2.entryValIdx, linkedValsArray);
	
	return valsPlaced;
}
