void swap(__global int* a, __global int* b) {
    __private int temp = *a;
    *a = *b;
    *b = temp;
}

__kernel void bubble_sort(__global int* data, __private int size) {
    __private int swaps = 0;
    for (int i=0; i<size-1; ++i) {
        swaps = 0;
        for (int j=0; j < size-i-1; ++j) {
            if (data[j] > data[j + 1]) {
                // swap(&data[j], &data[j+1]);
                int temp = data[j + 1];
                data[j + 1] = data[j];
                data[j] = temp;
                ++swaps;
            }
        }

        if (!swaps) {
            break;
        }
    }    
}
