// Sorts are smallest -> largest
// Comparitors are a-b (so if a < b then result is negative)

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

// This sort is not stable, but is in place
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

template <typename AT, typename C>
void quick_sort(AT *a) {
  if (a->len() <= 0) {
    return;
  }
  quick_sort_helper<AT, C>(a, 0, a->len()-1);
}

/*template <typename AT, typename C>
void merge_sort_helper(AT *a, AT *b) {
  for 
}

template <typename AT, typename C>
void merge_sort(AT *a) {
  Array tmp; 
  AT *b;
}*/
  
