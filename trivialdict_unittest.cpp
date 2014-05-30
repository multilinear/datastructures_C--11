#include <cstring>
#include <stdio.h>
#include "panic.h"
#include "trivialdict.h"

class TrivialDictDatum {
  public:
    int value;
  public:
    TrivialDictDatum() { }
    TrivialDictDatum(int i) {
      value = i;
    }
    const int val(void) {
      return value;
    }
    static const int compare(const int val1, const int val2) {
      return val1-val2;
    }
    void print(void) {
      printf("%d", value);
    }
    static TrivialDictDatum flag_value(void) {
      return TrivialDictDatum(-999);
    }
};

#define TEST_SIZE 50
int ints[TEST_SIZE];
int ints_start;
int ints_end;

void check(TrivialDict<TrivialDictDatum,int> *dict) {
  int i;
  for (i=ints_start; i<ints_end; i++) {
    if(!dict->get(ints[i])) {
      printf("problem with: %d\n", ints[i]);
      PANIC("Element not in dict anymore!");
    }
  }
}

int main(int argc, char* argv[]) {
  TrivialDict<TrivialDictDatum,int> dict(TEST_SIZE);

  int i;
  int j;
  // insert in order, then remove
  printf("Begin TrivialDict.h test\n");
  for (j=0; j<TEST_SIZE; j++) {
    for (i=0; i<j; i++) {
      dict.insert(TrivialDictDatum(i));
    }
    for (i=0; i<j; i++) {
      dict.remove(i);
    }
    if (!dict.isempty()) {
      PANIC("Dict should be empty here, but isn't");
    }
  }
  for (j=1; j<TEST_SIZE; j++) {
    // insert in reverse order, then remove in reverse order
    for (i=j; i>0; i--) {
      dict.insert(TrivialDictDatum(i));
    }
    for (i=j; i>0; i--) {
      dict.remove(i);
    }
  }
  int k;
  for (k=1; k<TEST_SIZE; k++) {
    ints_end=0;
    ints_start=0;
    for (i=0; i<k; i++) {
      bool new_v = false;
      int r;
      // find a value we haven't used yet
      while (!new_v) {
        // Note, we did not initialize rand, this is purposeful
        r = rand();
        new_v = true;
        for (j=ints_start; j<ints_end; j++) {
          if (ints[j] == r){
            new_v = false;
            break;
          }
        }
      }
      // put it in thet dict 
      dict.insert(TrivialDictDatum(r));
      // and in the list
      ints[ints_end++] = r;
      // check that everything is in the list that should be
      check(&dict);
    }
    for(i=0; i<ints_end; i++) {
      dict.remove(ints[i]);
      ints_start += 1;
      // check that everything is in the list that should be
      check(&dict);
    }
    if (!dict.isempty()) {
      PANIC("Dict should be empty here, but isn't");
    }
  }
  printf("PASS\n");
}
