#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "quicksort.h"

#define ARRAY_SIZE 1048576

// SEE 1-2pivot.c for an explanation of the code

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
	int indices[5];
	for(int i = 0; i < 5; ++i){
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
	
	int pivot2_1;
	int pivot2_2;
	int pivot3_1;
	int pivot3_2;
	int pivot3_3;
	int pivot2_1Idx;
	int pivot2_2Idx;
	int pivot3_1Idx;
	int pivot3_2Idx;
	int pivot3_3Idx;
	
	// hard paritions (lta2, bb2, gta2 and lta3, bf3, bl3, gta3) will never be empty
	partition zeroPartition;
	partition none;
	
	partition lta2; // less than all 2 pivot
	partition gtf2; // greater than first 2 pivot
	partition bb2; // between both 2 pivot
	partition ltl2; // less than last 2 pivot
	partition gta2; // greater than all 2 pivot
	
	partition lta3; // less than all 3 pivot
	partition gtf3; // greater than first 3 pivot
	partition bf3; // between first 3 pivot
	partition ltm3; // less than middle 3 pivot
	partition bb3; // between both 3 pivot
	partition gtm3; // greater than middle 3 pivot
	partition bl3; // between last 3 pivot
	partition ltl3; // less than last 3 pivot
	partition gta3; // greater than all 3 pivot
	
	// initialize soft partitions
	zeroPartition.entryValIdx = -1;
	none.entryValIdx = -1;
	
	gtf2.entryValIdx = -1;
	ltl2.entryValIdx = -1;
	
	gtf3.entryValIdx = -1;
	ltm3.entryValIdx = -1;
	bb3.entryValIdx = -1;
	gtm3.entryValIdx = -1;
	ltl3.entryValIdx = -1;
	
	// initialize first two pivots and first three hard partitions
	{
		// bubble sort
		for(int i = 0; i < 4; ++i){
			for(int j = 4; j > i; --j){
				if(linkedValsArray[indices[j]].val < linkedValsArray[indices[j-1]].val){
					int tmp = indices[j-1];
					indices[j-1] = indices[j];
					indices[j] = tmp;
				}
			}
		}
		
		lta2.initValIdx = indices[0];
		lta2.entryValIdx = indices[0];
		linkedValsArray[indices[0]].nextValIdx = -1;
		bb2.initValIdx = indices[2];
		bb2.entryValIdx = indices[2];
		linkedValsArray[indices[2]].nextValIdx = -1;
		gta2.initValIdx = indices[4];
		gta2.entryValIdx = indices[4];
		linkedValsArray[indices[4]].nextValIdx = -1;
		
		pivot2_1 = linkedValsArray[indices[1]].val;
		pivot2_2 = linkedValsArray[indices[3]].val;
		pivot2_1Idx = indices[1];
		pivot2_2Idx = indices[3];
	}
	
	int estimatedBelowPf = 1;
	int estimatedBetweenP1P2 = 1;
	int estimatedAbovePl = 1;
	
	int estimatedBelowPm;
	int estimatedAbovePm;
	
	int estimatedElementsPlaced;
	
	// 2 pivots
	{
		TWO_PIVOTS:
		estimatedElementsPlaced = estimatedBelowPf + estimatedBetweenP1P2 + estimatedAbovePl;
		while(1){
			if(idx < 0){
				goto END_TWO_PIVOTS;
			}
			int idxNext = linkedValsArray[idx].nextValIdx;
			
			// find which partition the value falls into
			if(linkedValsArray[idx].val < pivot2_1){
				
				// if the partition is not too large, place the element in the partition. Else make a third pivot
				if(estimatedBelowPf <= estimatedElementsPlaced/2){
					linkedValsArray[idx].nextValIdx = lta2.entryValIdx;
					lta2.entryValIdx = idx;
					estimatedBelowPf += 1;
				} else {
					//create new pivot and partitions (there are more than 2 values in lta2 so 2 can be taken out)
					int indices[3] = {idx, lta2.entryValIdx, linkedValsArray[lta2.entryValIdx].nextValIdx};
					lta2.entryValIdx = linkedValsArray[indices[2]].nextValIdx;
					
					// bubble sort the 3 values
					for(int i = 0; i < 2; ++i){
						for(int j = 0; j < 2; ++j){
							int idx = indices[j];
							int idxNext = indices[j+1];
							if(linkedValsArray[idx].val > linkedValsArray[idxNext].val){
								indices[j+1] = idx;
								indices[j] = idxNext;
							}
						}
					}
					
					pivot3_1Idx = indices[1];
					pivot3_1 = linkedValsArray[indices[1]].val;
					lta3.initValIdx = indices[0];
					lta3.entryValIdx = indices[0];
					linkedValsArray[indices[0]].nextValIdx = -1;
					bf3.initValIdx = indices[2];
					bf3.entryValIdx = indices[2];
					linkedValsArray[indices[2]].nextValIdx = -1;
					
					// populate the remaining partitions and pivots with values
					// lta3
					gtf3 = zeroPartition;
					// bf3
					ltm3 = lta2;
					bb3 = zeroPartition;
					gtm3 = gtf2;
					bl3 = bb2;
					ltl3 = ltl2;
					gta3 = gta2;
					
					pivot3_2Idx = pivot2_1Idx;
					pivot3_3Idx = pivot2_2Idx;
					pivot3_2 = pivot2_1;
					pivot3_3 = pivot2_2;
					
					idx = idxNext;
					
					estimatedBelowPm = 2;
					estimatedAbovePm = 2;
					
					goto THREE_PIVOTS;
				}
			} else if(linkedValsArray[idx].val < pivot2_2){
				
				// if the partition is not too large, place the element in the partition. Else make a third pivot
				if(estimatedBetweenP1P2 <= estimatedElementsPlaced/2){
					linkedValsArray[idx].nextValIdx = bb2.entryValIdx;
					bb2.entryValIdx = idx;
					estimatedBetweenP1P2 += 1;
				} else {
					//create new pivot and partitions (there are more than 2 values in lta2 so 2 can be taken out)
					int indices[3] = {idx, bb2.entryValIdx, linkedValsArray[bb2.entryValIdx].nextValIdx};
					bb2.entryValIdx = linkedValsArray[indices[2]].nextValIdx;
					
					// bubble sort the 3 values
					for(int i = 0; i < 2; ++i){
						for(int j = 0; j < 2; ++j){
							int idx = indices[j];
							int idxNext = indices[j+1];
							if(linkedValsArray[idx].val > linkedValsArray[idxNext].val){
								indices[j+1] = idx;
								indices[j] = idxNext;
							}
						}
					}
					
					pivot3_2Idx = indices[1];
					pivot3_2 = linkedValsArray[indices[1]].val;
					bf3.initValIdx = indices[0];
					bf3.entryValIdx = indices[0];
					linkedValsArray[indices[0]].nextValIdx = -1;
					bl3.initValIdx = indices[2];
					bl3.entryValIdx = indices[2];
					linkedValsArray[indices[2]].nextValIdx = -1;
					
					// populate the remaining partitions and pivots with values
					lta3 = lta2;
					gtf3 = gtf2;
					// bf3
					ltm3 = zeroPartition;
					bb3 = bb2;
					gtm3 = zeroPartition;
					// bl3
					ltl3 = ltl2;
					gta3 = gta2;
					
					pivot3_1Idx = pivot2_1Idx;
					pivot3_3Idx = pivot2_2Idx;
					pivot3_1 = pivot2_1;
					pivot3_3 = pivot2_2;
					
					idx = idxNext;
					
					estimatedBelowPm = 2;
					estimatedAbovePm = 2;
					
					goto THREE_PIVOTS;
				}
			} else {
				// if the partition is not too large, place the element in the partition. Else make a third pivot
				if(estimatedAbovePl <= estimatedElementsPlaced/2){
					linkedValsArray[idx].nextValIdx = gta2.entryValIdx;
					gta2.entryValIdx = idx;
					estimatedAbovePl += 1;
				} else {
					
					//create new pivot and partitions (there are more than 2 values in lta2 so 2 can be taken out)
					int indices[3] = {idx, gta2.entryValIdx, linkedValsArray[gta2.entryValIdx].nextValIdx};
					gta2.entryValIdx = linkedValsArray[indices[2]].nextValIdx;
					
					// bubble sort the 3 values
					for(int i = 0; i < 2; ++i){
						for(int j = 0; j < 2; ++j){
							int idx = indices[j];
							int idxNext = indices[j+1];
							if(linkedValsArray[idx].val > linkedValsArray[idxNext].val){
								indices[j+1] = idx;
								indices[j] = idxNext;
							}
						}
					}
					
					pivot3_3Idx = indices[1];
					pivot3_3 = linkedValsArray[indices[1]].val;
					bl3.initValIdx = indices[0];
					bl3.entryValIdx = indices[0];
					linkedValsArray[indices[0]].nextValIdx = -1;
					gta3.initValIdx = indices[2];
					gta3.entryValIdx = indices[2];
					linkedValsArray[indices[2]].nextValIdx = -1;
					
					// populate the remaining partitions and pivots with values
					lta3 = lta2;
					gtf3 = gtf2;
					bf3 = bb2;
					ltm3 = ltl2;
					bb3 = zeroPartition;
					gtm3 = gta2;
					// bl3
					ltl3 = zeroPartition;
					// gta3
					
					pivot3_1Idx = pivot2_1Idx;
					pivot3_2Idx = pivot2_2Idx;
					pivot3_1 = pivot2_1;
					pivot3_2 = pivot2_2;
					
					idx = idxNext;
					
					estimatedBelowPm = 2;
					estimatedAbovePm = 2;
					
					goto THREE_PIVOTS;
				}
			}
			
			idx = idxNext;
			estimatedElementsPlaced += 1;
		}
	}
	
	// 3 pivots
	{
		THREE_PIVOTS:
		estimatedElementsPlaced = estimatedBelowPm + estimatedAbovePm;
		while(1){
			if(idx < 0){
				goto END_THREE_PIVOTS;
			}
			
			int idxNext = linkedValsArray[idx].nextValIdx;
			
			// find which partition the value falls into
			if(linkedValsArray[idx].val < pivot3_2){
				
				// place the element
				if(linkedValsArray[idx].val < pivot3_1){
					linkedValsArray[idx].nextValIdx = lta3.entryValIdx;
					lta3.entryValIdx = idx;
				} else {
					linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
					bf3.entryValIdx = idx;
				}
				
				// if the element didn't fit, swap to two pivots
				if(estimatedBelowPm <= estimatedElementsPlaced * 2 / 3){
					estimatedBelowPm += 1;
				} else {
					// lta consists of lta
					lta2 = lta3;
					
					// gtf consists of gtf + bb, 
					gtf2 = gtf3;
					if(bb3.entryValIdx != -1){
						if(gtf2.entryValIdx != -1){
							linkedValsArray[gtf2.initValIdx].nextValIdx = bb3.entryValIdx;
							gtf2.initValIdx = bb3.initValIdx;
						} else {
							gtf2 = bb3; 
						}
					}
					
					// bb consists of bf
					bb2 = bf3;
					// ltl consists of ltm
					ltl2 = ltm3;
					
					// gta consists of gta + gtm + bl
					gta2 = gta3;
					linkedValsArray[gta2.initValIdx].nextValIdx = bl3.entryValIdx;
					gta2.initValIdx = bl3.initValIdx;
					if(gtm3.entryValIdx != -1){
						linkedValsArray[gta2.initValIdx].nextValIdx = gtm3.entryValIdx;
						gta2.initValIdx = gtm3.initValIdx;
					}	
					
					// none consists of none + ltl
					if(ltl3.entryValIdx != -1){
						if(none.entryValIdx != -1){
							linkedValsArray[none.initValIdx].nextValIdx = ltl3.entryValIdx;
							none.initValIdx = ltl3.initValIdx;
						} else {
							none = ltl3;
						}
					}
					
					// pivot 1 and 2 remain the same, pivot 3 gets added to gta
					pivot2_1 = pivot3_1;
					pivot2_2 = pivot3_2;
					pivot2_1Idx = pivot3_1Idx;
					pivot2_2Idx = pivot3_2Idx;
					
					linkedValsArray[pivot3_3Idx].nextValIdx = gta2.entryValIdx;
					gta2.entryValIdx = pivot3_3Idx;
					
					idx = idxNext;
					
					estimatedBelowPf = 1;
					estimatedBetweenP1P2 = 1;
					estimatedAbovePl = 1;
					
					goto TWO_PIVOTS;
				}
			} else {
				// place the element
				if(linkedValsArray[idx].val < pivot3_3){
					linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
					bl3.entryValIdx = idx;
				} else {
					linkedValsArray[idx].nextValIdx = gta3.entryValIdx;
					gta3.entryValIdx = idx;
				}
					
				// if the element didn't fit, swap to two pivots
				if(estimatedAbovePm <= estimatedElementsPlaced * 2 / 3){
					estimatedAbovePm += 1;
				} else {
					
					// lta consists of lta + ltm + bf
					lta2 = lta3;
					linkedValsArray[lta2.initValIdx].nextValIdx = bf3.entryValIdx;
					lta2.initValIdx = bf3.initValIdx;
					if(ltm3.entryValIdx != -1){
						linkedValsArray[lta2.initValIdx].nextValIdx = ltm3.entryValIdx;
						lta2.initValIdx = ltm3.initValIdx;
					}
					
					// gtf consists of gtm
					gtf2 = gtm3;
					
					// bb consists of bl
					bb2 = bl3;
					
					// ltl consists of ltl + bb
					ltl2 = ltl3;
					if(bb3.entryValIdx != -1){
						if(ltl2.entryValIdx != -1){
							linkedValsArray[ltl2.initValIdx].nextValIdx = bb3.entryValIdx;
							ltl2.initValIdx = bb3.initValIdx;
						} else {
							ltl2 = bb3; 
						}
					}
					
					// gta remains the same
					gta2 = gta3;
					
					// none consists of none + gtf
					if(gtf3.entryValIdx != -1){
						if(none.entryValIdx != -1){
							linkedValsArray[none.initValIdx].nextValIdx = gtf3.entryValIdx;
							none.initValIdx = gtf3.initValIdx;
						} else {
							none = gtf3;
						}
					}
					
					// new pivots 1 and 2 are old pivots 2 and 3, old pivot 1 gets added to lta
					pivot2_1 = pivot3_2;
					pivot2_2 = pivot3_3;
					pivot2_1Idx = pivot3_2Idx;
					pivot2_2Idx = pivot3_3Idx;
					
					linkedValsArray[pivot3_1Idx].nextValIdx = lta2.entryValIdx;
					lta2.entryValIdx = pivot3_1Idx;
					
					idx = idxNext;
					
					estimatedBelowPf = 1;
					estimatedBetweenP1P2 = 1;
					estimatedAbovePl = 1;
					
					goto TWO_PIVOTS;
				}
			}
			
			idx = idxNext;
			estimatedElementsPlaced += 1;
		}
	}
	
	int valsPlaced;
	
	END_TWO_PIVOTS:	
	
	// place elements from gtf
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
	
	// place elements from ltl
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
	
	// sort partitions

	valsPlaced = __twoThreePivotSort(vals, destinationIdx, lta2.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot2_1;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, bb2.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot2_2;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, gta2.entryValIdx, linkedValsArray);
	
	return valsPlaced;
	
	END_THREE_PIVOTS:
	
	// place elements from gtf
	idx = gtf3.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_2){
			linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
			bf3.entryValIdx = idx;
		} else if(linkedValsArray[idx].val < pivot3_3){
			linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
			bl3.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = gta3.entryValIdx;
			gta3.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from ltm
	idx = ltm3.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_1){
			linkedValsArray[idx].nextValIdx = lta3.entryValIdx;
			lta3.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
			bf3.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from bb
	idx = bb3.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_2){
			linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
			bf3.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
			bl3.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from gtm
	idx = gtm3.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_3){
			linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
			bl3.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = gta3.entryValIdx;
			gta3.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from ltl
	idx = ltl3.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_1){
			linkedValsArray[idx].nextValIdx = lta3.entryValIdx;
			lta3.entryValIdx = idx;
		} else if(linkedValsArray[idx].val < pivot3_2){
			linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
			bf3.entryValIdx = idx;
		} else {
			linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
			bl3.entryValIdx = idx;
		}
		idx = idxNext;
	}
	
	// place elements from none
	idx = none.entryValIdx;
	while(idx != -1){
		int idxNext = linkedValsArray[idx].nextValIdx;
		if(linkedValsArray[idx].val < pivot3_2){
			if(linkedValsArray[idx].val < pivot3_1){
				linkedValsArray[idx].nextValIdx = lta3.entryValIdx;
				lta3.entryValIdx = idx;
			} else {
				linkedValsArray[idx].nextValIdx = bf3.entryValIdx;
				bf3.entryValIdx = idx;
			}
		} else {
			if(linkedValsArray[idx].val < pivot3_3){
				linkedValsArray[idx].nextValIdx = bl3.entryValIdx;
				bl3.entryValIdx = idx;
			} else {
				linkedValsArray[idx].nextValIdx = gta3.entryValIdx;
				gta3.entryValIdx = idx;
			}
		}
		idx = idxNext;
	}
	
	// sort partitions
	
	valsPlaced = __twoThreePivotSort(vals, destinationIdx, lta3.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot3_1;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, bf3.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot3_2;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, bl3.entryValIdx, linkedValsArray);
	
	vals[destinationIdx + valsPlaced] = pivot3_3;
	valsPlaced += 1;
	
	valsPlaced += __twoThreePivotSort(vals, destinationIdx + valsPlaced, gta3.entryValIdx, linkedValsArray);
	
	return valsPlaced;
}
