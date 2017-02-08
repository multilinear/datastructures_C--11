// Sorts are smallest -> largest
// Comparitors are a-b (so if a < b then result is negative)

#ifndef SORT_H
#define SORT_H

// Implemented mostly for correctness tests
// Though might be useful on some smaller data
// This sort is stable, and in place
template <typename AT, typename C>
void selection_sort(AT *a) {
  size_t smallest;
  size_t i,j;
  for (i = 0; i < a->len(); i++) {
    smallest = i;
    for (j = i+1; j < a->len(); j++) {
      if (C::compare((*a)[j], (*a)[smallest]) < 0) { 
        smallest = j;
      }
    }
    a->swap(i, smallest);
  }
}

// Useful for lists likely to have only a few inversions
// O(N^2), \Theta(N^2) on random list
// On list with constant number of inversions O(N)
// This sort is stable, and in place
template <typename AT, typename C>
void bubble_sort(AT *a) {
  size_t i;
  bool swapped = true;
  if (a->len() < 2) {
    return;
  }
  while (swapped) { 
    swapped = false;
    for (i = 0; i+1 < a->len(); i++) {
      if (C::compare((*a)[i+1], (*a)[i]) < 0) { 
        a->swap(i, i+1);
        swapped = true;
      }
    }
  }
}

template <typename AT, typename C>
void quick_sort_helper(AT *a, size_t bottom, size_t top) {
  if (bottom >= top) {
    return;
  }
  if (bottom + 1 == top) {
    if (C::compare((*a)[bottom], (*a)[top]) > 0) {
      a->swap(bottom, top);
    }
    return;
  }
  size_t left = bottom+1;
  size_t right = top;
  while (left < right) {
    while (C::compare((*a)[left], (*a)[bottom]) <= 0 && left < right) {
      left++;
    }
    while (C::compare((*a)[right], (*a)[bottom]) > 0) {
      right--;
    }
    if (left < right) {
      a->swap(left, right);
    }
  }
  a->swap(bottom, right);
  if (right > bottom) {
    quick_sort_helper<AT, C>(a, bottom, right-1);
  }
  if (right < top) {
    quick_sort_helper<AT, C>(a, right+1, top);
  }
}

// This sort is not stable, but is in place
// O(N^2), \Theta(Nlog(N))
template <typename AT, typename C>
void quick_sort(AT *a) {
  if (a->len() <= 0) {
    return;
  }
  quick_sort_helper<AT, C>(a, 0, a->len()-1);
}

// This is a trick that lets us write this code once and call it
// twice from "merge_sort", so it works if AAT and BAT differ.
template <typename AAT, typename BAT, typename C>
void merge_sort_helper(AAT *a, BAT *b, size_t chunk, size_t len) {
  size_t i;
  size_t j;
  size_t output = 0;
  for(size_t base = 0; base < len; base += 2*chunk) {
    i = base;
    j = base+chunk;
    size_t ie = j;
    size_t je = j+chunk; 
    if (je > len) {
      je = len;
      if (ie > len) {
        ie = len;
      }
    }
    while (i < ie && j < je) {
      if (C::compare((*a)[i], (*a)[j]) <= 0) {
        (*b)[output++] = (*a)[i++];
      } else {
        (*b)[output++] = (*a)[j++];
      }
    }
    while (i < ie) {
      (*b)[output++] = (*a)[i++];
    }
    while (j < je) {
      (*b)[output++] = (*a)[j++];
    }
  }
}

// This sort is stable, but not in place
// O(Nlog(N), \Theta(Nlog(N))
//
template <typename AT, typename TAT, typename C>
void merge_sort(AT *in, TAT *tmp) {
  size_t chunk = 1;
  size_t len = in->len();
  #ifdef SORT_DEBUG
  if (tmp->len() < in->len()) {
    PANIC("merge_sort, tmp is not large enough\n");
  }
  #endif
  // We use loop unrolling so we can deal with in and tmp being different types
  while (true) {
    if (chunk >= len) {
      break;
    }
    merge_sort_helper<AT, TAT, C>(in, tmp, chunk, len);
    chunk = 2*chunk;
    if (chunk >= len) {
      // TODO: If we did this copy first, tmp only has to be half the size of in
      array_copy<AT, TAT>(in, tmp) ;
      break;
    }
    merge_sort_helper<TAT, AT, C>(tmp, in, chunk, len);
    chunk = 2*chunk;
  }
}
 
#endif // SORT_H
