#include <stdio.h>
#include "sort.h"

#ifndef MEDIAN_FIND_H
#define MEDIAN_FIND_H

void my_print_array2(Array<uint32_t> *a) {
  printf("[");
  for (size_t i=0; i<a->len(); i++) {
    printf("%u,", (*a)[i]);
  }
  printf("]\n");
}
void my_print_array(Array<size_t> *a) {
  printf("[");
  for (size_t i=0; i<a->len(); i++) {
    printf("%lu,", (*a)[i]);
  }
  printf("]\n");
}
void my_print_array3(Array<uint32_t> *a, Array<size_t> *ind) {
  printf("[");
  for (size_t i=0; i<ind->len(); i++) {
    printf("%u,", (*a)[(*ind)[i]]);
  }
  printf("]\n");
}


// Forward declaration for recursion
template<typename AT, typename C, bool linear>
size_t findkth_helper(AT *a, Array<size_t> *ind, size_t k, size_t bottom, size_t top);
 
// Must be called on an array of length >= 2
template<typename AT, typename C>
size_t findkth_pivot_helper(AT *a, Array<size_t> *ind, size_t bottom, size_t top) {
  const size_t chunk = 5;
  size_t len = top-bottom+1;
  // Once it's small enough just pick a pivot
  // Any error here is just a constant factor, to be compared to the speed of
  // sorting the subarrays
 
  if (len < chunk*2) {
    // It doesn't much matter what we return, just a pivot between bottom and top
    // We return the index of the element in "ind" array, so we don't lose
    // track of where it is there.
    return bottom;
  }
    
  // Sort each chunk of 5
  for (size_t i=bottom; i<=top; i+=chunk) {
    for (size_t j=i; j<i+chunk && j<=top; j++) {
      for (size_t k=j+1; k<i+chunk && k<=top; k++) {
        if (C::compare((*a)[(*ind)[j]], (*a)[(*ind)[k]]) > 0) {
          ind->swap(j,k);
        }
      }
    }
  }
  // Shift all the medians to the beginning of the array portion we're in
  // This way we can recurse on the medians
  for (size_t i=0; i<len/chunk; i++) {
    ind->swap(bottom+i, bottom+(i*chunk)+(chunk/2));
  }
  len = len/chunk;
  top = bottom+len-1;
  return findkth_helper<AT, C, true>(a, ind, (len-1)/2, bottom, top);
}

template<typename AT, typename C, bool linear>
size_t findkth_helper(AT *a, Array<size_t> *ind, size_t k, size_t orig_bottom, size_t orig_top) {
  // Handle pathalogical cases
  if (k >= orig_top-orig_bottom+1) {
    printf("k=%lu orig_top-orig_bottom+1=%lu\n", k, orig_top-orig_bottom+1);
    PANIC("K is greater than array length");
  }
  size_t bottom=orig_bottom;
  size_t top=orig_top;
  while (true) {
    if (bottom == top) {
      return bottom;
    }
    // Get the index of the pivot in ind[] 
    size_t pivot;
    if (linear) {
      pivot=findkth_pivot_helper<AT, C>(a, ind, bottom, top);
      // Get the pivot out of the way
      ind->swap(bottom, pivot);
    } else {
      pivot=bottom;
    }
    // Partition using the pivot (now located at bottom)
    // Get the index of the pivot in ind, if ind were ordered
    size_t j = top;
    size_t i = bottom+1;
    while (i != j) {
      if (C::compare((*a)[(*ind)[i]], (*a)[(*ind)[bottom]]) > 0) {
        ind->swap(i,j);
        j--;
      } else { 
        i++;
      } 
    }
    // We still need to consider i/j
    if (C::compare((*a)[(*ind)[i]], (*a)[(*ind)[bottom]]) > 0) {
      // This means the i should be in the upper group
      i--; 
    } 
    // Put the pivot back in the middle
    ind->swap(i, bottom);
    if (i == k+orig_bottom) {
      return i;
    }
    if (i > k+orig_bottom) {
      // bottom half is too big, so it must be in the bottom half
      top = i-1;
    } else {
      // top half is too big, so it must be in the top half
      bottom = i+1;
    }
  }
} 

template<typename AT, typename C, bool linear>
size_t findkth(AT *a, size_t k) {
  // Build an index array
  Array<size_t> ind(a->len());
  for (size_t i=0;i<ind.len(); i++) {
    ind[i] = i;
  }
  size_t i = findkth_helper<AT, C, linear>(a, &ind, k, 0, a->len()-1);
  return ind[i];
}
 
template<typename AT, typename C>
size_t medianfind(AT *a) {
  return findkth<AT, C>(a, a->len()/2);
}
 
#endif