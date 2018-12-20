/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)  2017-02-12 
 *
 * A collection of sorting algorithms
 */
 
#include "panic.h"
#include "array.h"
#include <cstdint>
#include "math.h"

#ifndef SORT_H
#define SORT_H

// Sorts are smallest -> largest
// Comparitors are a-b (so if a < b then result is negative)

// Implemented mostly for correctness tests
// Though might be useful on some smaller data
// This sort is stable, and in place
// Use this if:
//   You probably shouldn't, but it might be faster on very small datasets
template <typename AT, typename C>
void selection_sort(AT *a) {
  size_t smallest;
  size_t i,j;
  for (i = 0; i < a->size(); i++) {
    smallest = i;
    for (j = i+1; j < a->size(); j++) {
      if (C::compare((*a)[j], (*a)[smallest]) < 0) { 
        smallest = j;
      }
    }
    std::swap((*a)[i],(*a)[smallest]);
  }
}

// Useful for lists likely to have only a few inversions
// O(N^2), \Omega(N)
// expected O(N^2) on random list
// On list with constant number of inversions O(N)
// This sort is stable, and in place
// Use this if:
//   You know your list is almost sorted, it just has some inversions
template <typename AT, typename C>
void bubble_sort(AT *a) {
  size_t i;
  bool swapped = true;
  if (a->size() < 2) {
    return;
  }
  while (swapped) { 
    swapped = false;
    for (i = 0; i+1 < a->size(); i++) {
      if (C::compare((*a)[i+1], (*a)[i]) < 0) { 
        std::swap((*a)[i],(*a)[i+1]);
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
      std::swap((*a)[bottom],(*a)[top]);
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
      std::swap((*a)[left], (*a)[right]);
    }
  }
  std::swap((*a)[bottom],(*a)[right]);
  if (right > bottom) {
    quick_sort_helper<AT, C>(a, bottom, right-1);
  }
  if (right < top) {
    quick_sort_helper<AT, C>(a, right+1, top);
  }
}

// This sort is not stable, but is in place
// O(N^2), \Omega(Nlog(N))
// expected O(Nlog(N)) on random list
// Fastest average runtime (tested)
// Use this if:
//   Your not worried about worst-case runtime, just average
template <typename AT, typename C>
void quick_sort(AT *a) {
  if (a->size() <= 0) {
    return;
  }
  quick_sort_helper<AT, C>(a, 0, a->size()-1);
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
// \Theta(Nlog(N))
// Second fastest average runtime (tested)
// Believed to be the fastest worst-case runtime
// Use this if:
//   You care about worst-case runtime, and not-in-place is okay
template <typename AT, typename TAT, typename C>
void merge_sort(AT *in, TAT *tmp) {
  size_t chunk = 1;
  size_t len = in->size();
  #ifdef SORT_DEBUG
  if (tmp->size() < in->size()) {
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

// This sort is not stable, but is in place
// \Theta(Nlog(N))
// This is the slowest of the Nlog(N) sorts
// Use this only if you:
// 1: need a stable, in place sort OR
// 2: need an in-place, comparison-based sort, and are prioritizing worst-case
template <typename AT, typename C>
void heap_sort(AT *in) {
	// *** first we build the heap
	// push of in.size()-1 is noop
	if (in->size() <= 1) {
		return;
	}
	size_t len = in->size();
	for (size_t i=1; i<len; i++) {
		// bubble up i
		size_t j = i;
		size_t parent;
		while (j != 0) {
			parent = (j-1)/2;
			int c = C::compare((*in)[parent], (*in)[j]);
			if (c<0) {
				std::swap((*in)[parent],(*in)[j]);
			} else {
				break;
			}
			j = parent;
		}
	}
	// *** then we take the heap apart
  // Again when heap is size 1, we ignore it (the loop below skips index 0)
	for (size_t k = len-1; k>0; k--) {
		// pop the smallest element off the heap (and make the heap 1 element smaller)
		std::swap((*in)[k],(*in)[ 0]);
		// now bubble the new value down
		size_t i = 0;
		size_t j;
		while(true) {
			size_t left = 2*i + 1;
			size_t right = 2*i + 2;
			if (right < k) {
				// find the smaller value
				int c = C::compare((*in)[left], (*in)[right]);
				if (c < 0) {
					j = right;
				} else {
					j = left;
				}
			} else if (left < k) {
				j = left;
			} else {
				break;
			}
			// and if i is larger than j we need to swap
			// if not we're done
			int c = C::compare((*in)[i], (*in)[j]);
			if (c < 0) {
				std::swap((*in)[i],(*in)[j]);
			} else {
				break;
			}
			// go to where we moved the data and try again
			i = j;
		}
	}
}

template <typename AT>
void bradix_sort_helper(AT *in, size_t o_low, size_t o_high, uint32_t bit) {
  //printf("%li, %li, %i\n", o_low, o_high, bit);
  size_t low = o_low;
  size_t high = o_high;
  // Note: We sort assuming an unsigned type
  do {
    if((*in)[low] & bit) {
      std::swap((*in)[low],(*in)[high]); 
      high--;
    } else {
      low++;
    }
  } while (low != high);
  // At this point we haven't actually looked at the value of in[low] yet
  if ((*in)[low] & bit) {
    if (low > 0) {
      low--;
    }
  } else {
    high++;
  }
  bit >>= 1;
  if (!bit) {
    return;
  }
  if (o_low < low) {
    bradix_sort_helper<AT>(in, o_low, low, bit);
  }
  if (high < o_high) {
    bradix_sort_helper<AT>(in, high, o_high, bit);
  }
}

// Average and worst-case are O(log(sozeof(VT))N)
// where VT is the type of the values stored inside AT
// Testing with random 32-bit unsigned integers:
//   This is slower than quicksort average case, but it's faster worst-case
//   This is slower than mergesort average case, but it's in-place
//   This is faster than heapsort average case on large datA
// So: Use this if you are sorting numbers and need an in-place sort
// and care about worst-case performance
// 
// This is a simple binary radix sort, it is NOT stable (stability was thrown
// out in favor of being in place).
// Note that this is designed for primative *unsigned* types. It will not
// Work correctly on signed types. This is both due to the meaning of
// two's compliment, as well as sign extension.
template <typename AT>
void bradix_sort(AT *in) {
  if (in->size() <= 1) {
    return;
  }
  bradix_sort_helper<AT>(in, 0, in->size()-1, 1 << (8*sizeof(decltype((*in)[0]))-1));
}

template <typename AAT, typename BAT, uint32_t mod>
void radix_sort_helper(AAT *in, BAT *out, uint32_t shift) {
  size_t arena[mod+1];
  size_t in_len = in->size();
  // Zero the count table
  for (size_t i=0; i<mod+1; i++) {
    arena[i] = 0; 
  }
  // Count occurences
  // Note the "+1" here, this makes summation much simpler later
  for (size_t i=0; i<in_len; i++) {
    arena[(((uint32_t) (*in)[i] >> shift) & (mod-1)) + 1]++;
  }
  // Sum the account table to make indices
  size_t sum = arena[0];
  for (size_t i=1; i<mod+1; i++) {
    sum += arena[i];
    arena[i] = sum;
  }
  // Place everything at those indices
  for (size_t i=0; i<in_len; i++) {
    size_t index = ((uint32_t)((*in)[i] >> shift)) & (mod-1); 
    index = arena[index]++;
    (*out)[index] = (*in)[i];
  }
}

template <typename AAT, typename BAT, const uint32_t arity_bits>
void radix_sort(AAT *in, BAT *buf) {
  // Assuming you won't use 32 bits of radix :P
  // We could use 16, but 32 bit computations are faster
  const uint32_t mod = 1<<arity_bits;
  uint32_t shift = 0;
  if (in->size() < 2) {
    return;
  }
  while(true) {
    radix_sort_helper<AAT,BAT,mod>(in, buf, shift);
    shift += arity_bits;
    if (!(typename AAT::value_type)(1lu<<shift)) {
      array_copy<AAT, BAT>(in, buf) ;
      break;
    }
    radix_sort_helper<BAT,AAT,mod>(buf, in, shift);
    shift += arity_bits;
    if (!(typename AAT::value_type)(1lu<<shift)) {
      break;
    }
  }
}

// Default sorting algorithms
template <typename AT, typename C>
void sort(AT *a) {
  AT b(a->size());
  merge_sort(a, &b);
}

template <typename AT, typename BT>
void fast_sort(AT *a, BT *b) {
  class IntCompare {
  public:
    static int32_t compare(uint32_t v1, uint32_t v2) {
      return v1 - v2;
    }
  };
  if (a->size() <=20) {
    heap_sort<AT, IntCompare>(a);
  } else {
    radix_sort<AT, BT, 6>(a, b);
  }
}


#endif // SORT_H
