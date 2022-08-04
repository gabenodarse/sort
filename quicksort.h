

void quickSort(int* vals, int len);

void quickSort(int* vals, int len){
	if(len <= 1){
		return;
	}
	
	int pivot = vals[0];
	int i = 1;
	int j = len - 1;
	while(i != j){
		if(vals[i] > pivot){
			int tmp = vals[i];
			vals[i] = vals[j];
			vals[j] = tmp;
			j -= 1;
		} else {
			i += 1;
		}
	}
	
	if(vals[i] > pivot){
		vals[0] = vals[i-1];
		vals[i-1] = pivot;
		quickSort(vals, i-1);
		quickSort(&vals[i], len - i);
	} else {
		vals[0] = vals[i];
		vals[i] = pivot;
		quickSort(vals, i);
		quickSort(&vals[i+1], len-i-1);
	}
}

void medianQuickSort(int* vals, int len){
	if(len <= 1){
		return;
	}
	
	// choose median
	if(vals[0] < vals[1]){
		if(vals[0] >= vals[2]){
			// vals[0] is median
		} else if(vals[1] < vals[2]){
			int tmp = vals[0];
			vals[0] = vals[1];
			vals[1] = tmp;
		} else {
			int tmp = vals[0];
			vals[0] = vals[2];
			vals[2] = tmp;
		}
	} else {
		if(vals[0] < vals[2]){
			// vals[0] is median
		} else if(vals[1] >= vals[2]){
			int tmp = vals[0];
			vals[0] = vals[1];
			vals[1] = tmp;
		} else {
			int tmp = vals[0];
			vals[0] = vals[2];
			vals[2] = tmp;
		}
	}
	
	// sort
	int pivot = vals[0];
	int i = 1;
	int j = len - 1;
	while(i != j){
		if(vals[i] > pivot){
			int tmp = vals[i];
			vals[i] = vals[j];
			vals[j] = tmp;
			j -= 1;
		} else {
			i += 1;
		}
	}
	
	if(vals[i] > pivot){
		vals[0] = vals[i-1];
		vals[i-1] = pivot;
		quickSort(vals, i-1);
		quickSort(&vals[i], len - i);
	} else {
		vals[0] = vals[i];
		vals[i] = pivot;
		quickSort(vals, i);
		quickSort(&vals[i+1], len-i-1);
	}
}

