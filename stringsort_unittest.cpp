#include "panic.h"
#include "string_sort.h"
#include <string>
#include <vector>
#include <cstdint>
#include "stdio.h"
#include "permutations.h"

using std::vector;
using std::string;

vector<string*> tmp;
vector<size_t> tmp2;
vector<size_t> tmp3;

void print_array(vector<string*> &v) {
  printf("[");
  for (size_t i=0; i<v.size(); i++) {
    printf("\"%s\", ", v[i]->c_str());
  }
  printf("]\n");
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

void call_sort(vector<string*> *input, int _unused) {
  tmp.resize(input->size());
  tmp2.resize(input->size());
  tmp3.resize(input->size());
  radix_sort(*input, tmp, tmp2, tmp3);
  check_sorted(*input);
}

string* generate_random_string() {
  string *s = new string();
  while (rand()%5 != 1) {
    s->push_back(((char)(rand()%26)) + 'A');
  }
  return s;
}

int main(){
  vector<string*> input;
  printf("Begin StringSort.h unittest\n");
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
  call_sort(&input, 0);

  input.resize(0);
  for (size_t i=0; i<8; i++) {
    input.push_back(generate_random_string());
  }
  permutations<vector<string*>, int>(&input, call_sort, 0);

  for (size_t i=0; i<1000; i++) {
    input.resize(0);
    for (size_t j=0; j<i; j++) {
      input.push_back(generate_random_string());
    }
    call_sort(&input, 0);
  }

  printf("PASSED\n");
}

