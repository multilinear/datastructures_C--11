#include "panic.h"
#include "string_sort.h"
#include <string>
#include <vector>
#include <cstdint>
#include "stdio.h"

using std::vector;
using std::string;

void print_array(vector<string*> &v) {
  printf("[");
  for (size_t i=0; i<v.size(); i++) {
    printf("\"%s\", ", v[i]->c_str());
  }
  printf("]\n");
}

void call_sort(vector<string*> &input, vector<string*> &tmp, vector<size_t> &tmp2, vector<size_t> &tmp3) {
  tmp.resize(input.size());
  tmp2.resize(input.size());
  tmp3.resize(input.size());
  radix_sort(input, tmp, tmp2, tmp3);
}

void check_sorted(vector<string*> &input) {
  for (size_t i=1; i<input.size(); i++) {
    if (*input[i] < *input[i-1]) {
      printf("%s index %lu is < %s index %lu\n", input[i]->c_str(), i, input[i-1]->c_str(), i-1);
      print_array(input);
      PANIC("Array is not sorted");
    }
  }
}

int main(){
  printf("Begin StringSort.h unittest\n");
  vector<string*> input;
  vector<string*> tmp;
  vector<size_t> tmp2;
  vector<size_t> tmp3;
  input.push_back(new string("A"));
  input.push_back(new string("B"));
  input.push_back(new string("Q"));
  input.push_back(new string("BB"));
  input.push_back(new string("AA"));
  input.push_back(new string("BA"));
  input.push_back(new string("AB"));
  input.push_back(new string(""));
  input.push_back(new string("Z"));
  input.push_back(new string("ZZZZWERG"));
  print_array(input);
  call_sort(input, tmp, tmp2, tmp3);
  check_sorted(input);
  printf("PASSED\n");
}

