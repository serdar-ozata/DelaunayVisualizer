//
// Created by serdar on 5/15/25.
//

#include "omp_quicksort.h"
#include <omp.h>
#include <algorithm>

void run_parallel_quicksort(Vector2* arr, int size) {
#pragma omp parallel
    {
#pragma omp single nowait
        {
            qs::quicksort(arr, 0, size - 1);
        }
    }
}

namespace qs {

    int partition(Vector2* arr, int low, int high) {
        const Vector2 pivot = arr[high];
        int i = low - 1;

        for(int j = low; j < high; ++j) {
            if(compare(arr[j], pivot)) {
                ++i;
                std::swap(arr[i], arr[j]);
            }
        }

        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void quicksort(Vector2* arr, int low, int high, int depth) {
        if(low < high) {
            int pi = partition(arr, low, high);
#pragma omp parallel sections if(depth < 4)
            {
#pragma omp section
                quicksort(arr, low, pi - 1, depth + 1);

#pragma omp section
                quicksort(arr, pi + 1, high, depth + 1);
            }
        }
    }
}