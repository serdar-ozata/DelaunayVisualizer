//
// Created by serdar on 5/15/25.
//

#ifndef OMP_QUICKSORT_H
#define OMP_QUICKSORT_H
#include "typedef.h"



namespace qs {
    template <typename T>
    bool compare(const T& a, const T& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
    template <typename T>
    int partition(T* arr, const int low, const int high) {
        const T pivot = arr[high];
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
    template <typename T>
    void quicksort(T* arr, const int low, int high, const int depth) {
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

// Function to run parallel quicksort
template <typename T>
void run_parallel_quicksort(T* arr, const int size) {
    {
#pragma omp parallel
        {
#pragma omp single nowait
            {
                qs::quicksort(arr, 0, size - 1, 0);
            }
        }
    }
}

#endif //OMP_QUICKSORT_H
