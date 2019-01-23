/* Copyright:  Matthew Brewer (mbrewer@smalladventures.net)
 *
 * A radix string-sorting algorithm 
 * Note: As implemented this is ASCII only
 */
 
#include "array.h"
#include "math.h"
#include <string>
#include <vector>
#include <cstdint>

#ifndef STRING_SORT_H
#define STRING_SORT_H

using std::vector;
using std::string;

// Limited to ASCII
const size_t buckets = 128;

void print_table(vector<size_t> &t) {
  printf ("[");
  for (size_t i=0; i<t.size(); i++) {
    printf ("%lu, ", t[i]);
  }
  printf ("]\n");
}

void print_vec(vector<string*> &v, size_t start, size_t end) {
  printf ("[");
  for (size_t i=start; i<end; i++) {
    printf ("%s, ", v[i]->c_str());
  }
  printf ("]\n");
}

inline bool radix_sort_helper(vector<string*> &in, vector<string*> &out, size_t start, size_t end, vector<size_t> &slice_out, size_t byte, size_t &slice_i) {
  size_t arena[buckets+2];
  bool done = true;
  bool used_only_first_bucket = true;
  // Zero the count table
  for (size_t i=0; i<buckets+2; i++) {
    arena[i] = 0; 
  }
  // Count occurences
  // Note the "+1" here, this makes summation simpler later
  arena[0] = 0;
  for (size_t i=start; i<end; i++) {
    if (in[i]->size() <= byte) {
      arena[1]++;
    } else {
      arena[((uint32_t)((*(in[i]))[byte]))+2]++;
      used_only_first_bucket = false;
    }
  }
  // Sum the account table to make indices
  arena[0] = start;
  size_t sum = arena[0];
  for (size_t i=1; i<buckets+2; i++) {
    //if (arena[i] > 1) done = false;
    sum += arena[i];
    // Also update our slice table, but only for non-empty buckets
    // (which become non-empty slices)
    if (arena[i]) {
      slice_out[slice_i++] = sum;
    }
    arena[i] = sum;
  }
  // Place everything at those indices
  for (size_t i=start; i<end; i++) {
    int index;
    if (in[i]->size() <= byte) {
      index=0;
    } else {
      index = ((uint32_t)((*(in[i]))[byte]))+1;
    }
    index = arena[index]++;
    out[index] = in[i];
  }
  return done || used_only_first_bucket;
}


void radix_sort(vector<string*> &in, vector<string*> &out, vector<size_t> &slice_in, vector<size_t> &slice_out) {
  //printf("SORTING ****************\n");
  if (in.size() < 2) {
    return;
  }
  uint32_t byte = 0;
  slice_in[0] = in.size();
  bool done = false;
  while(!done) {
    size_t slice;
    size_t sstart;
    size_t new_slice;
    /*printf("byte=%lu\n", byte);
    print_vec(in, 0, in.size());
    print_table(slice_in);*/
 
    // Move from in -> out
    done = true;
    sstart=0;
    slice=0;
    new_slice=0;
    while(sstart < in.size()) {
      size_t ssend = slice_in[slice];
      done &= radix_sort_helper(in, out, sstart, ssend, slice_out, byte, new_slice);
      sstart = ssend;
      slice++;
    }
    byte++;

    if (done) {
      for(size_t i=0; i<in.size(); i++) {
        in[i] = out[i];
      }
      return;
    }
    /*printf("byte=%lu\n", byte);
    print_vec(out, 0, out.size());
    print_table(slice_out);*/
    
    // Move from out -> in
    // Slice table currntly goes upwards
    done = true;
    sstart=0;
    slice=0;
    new_slice=0;
    while(sstart < in.size()) {
      size_t ssend = slice_out[slice];
      done &= radix_sort_helper(out, in, sstart, ssend, slice_in, byte, new_slice);
      sstart = ssend;
      slice++;
    }
    byte++;
  }
}

#endif // STRING_SORT_H
