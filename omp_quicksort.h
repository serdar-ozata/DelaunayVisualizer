//
// Created by serdar on 5/15/25.
//

#ifndef OMP_QUICKSORT_H
#define OMP_QUICKSORT_H
#include "typedef.h"



namespace qs {
    inline bool compare(const Vector2& a, const Vector2& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
    int partition(Vector2* arr, int low, int high);
    void quicksort(Vector2* arr, int low, int high, int depth = 0);
}

// Function to run parallel quicksort
void run_parallel_quicksort(Vector2* arr, int size);

#endif //OMP_QUICKSORT_H
