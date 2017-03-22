CC=g++ 
CFLAGS=-O3 -std=c++11 -Wall -Werror
CFLAGS_THREAD=-pthread

# Build lists
UNITTESTS=array avlhashtable avl bheap boundedheap btreehashtable btree dict dlist ochashtable heap list queue redblack ringbuffer rredblack set sort trivialdict ts_btree ts_ringbuffer ts_work_queue
HEAPS_BENCHMARKS=bheap.cpp boundedheap.cpp heap_dcarray.cpp
DICTS_BENCHMARKS=avlhashtable.cpp avl.cpp btreehashtable.cpp btree.cpp ochashtable.cpp redblack.cpp rredblack.cpp ts_btree.cpp
BENCHMARKS=$(HEAPS_BENCHMARKS) $(DICTS_BENCHMARKS) dict_benchmark sort_benchmark

UNITTEST_EXES=$(UNITTESTS:%=%_unittest) 
BENCHMARK_EXES=$(BENCHMARKS:.cpp=_benchmark)

# Generic rule

all: benchmarks tests

clean: ;rm $(UNITTEST_EXES); rm $(BENCHMARK_EXES); true

# build targets

tests: $(UNITTEST_EXES)

test: tests; $(UNITTESTS:%=./%_unittest &&) true

heaps_benchmarks: $(HEAPS_BENCHMARKS:.cpp=_benchmark)

heaps_benchmark: heaps_benchmarks; $(HEAPS_BENCHMARKS:%.cpp=bash -c "time ./%_benchmark" &&) true

dicts_benchmarks: $(DICTS_BENCHMARKS:.cpp=_benchmark)

dicts_benchmark: dicts_benchmarks; $(DICTS_BENCHMARKS:%.cpp=bash -c "time ./%_benchmark" &&) true

benchmarks: $(BENCHMARK_EXES)

benchmark: benchmarks; $(BENCHMARKS:%.cpp=./%_benchmark &&) true

# specific build rules

array_unittest: *.h *.cpp ; $(CC) $(CFLAGS) array_unittest.cpp -o array_unittest

avl_unittest: *.h *.cpp ; $(CC) $(CFLAGS) avl_unittest.cpp -o avl_unittest
avl_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVL externaldict_benchmark.cpp -o avl_benchmark

avlhashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) avlhashtable_unittest.cpp -o avlhashtable_unittest
avlhashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVLHASHTABLE externaldict_benchmark.cpp -o avlhashtable_benchmark

bheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) bheap_unittest.cpp -o bheap_unittest
bheap_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BHEAP heap_benchmark.cpp -o bheap_benchmark

boundedheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) boundedheap_unittest.cpp -o boundedheap_unittest
boundedheap_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BOUNDEDHEAP heap_benchmark.cpp -o boundedheap_benchmark

queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) queue_unittest.cpp -o queue_unittest

btree_unittest: *.h *.cpp ; $(CC) $(CFLAGS) btree_unittest.cpp -o btree_unittest
btree_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREE internaldict_benchmark.cpp -o btree_benchmark

btreehashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) btreehashtable_unittest.cpp -o btreehashtable_unittest
btreehashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREEHASHTABLE internaldict_benchmark.cpp -o btreehashtable_benchmark


dict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dict_unittest.cpp -o dict_unittest
dict_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) dict_benchmark.cpp -o dict_benchmark

dlist_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dlist_unittest.cpp -o dlist_unittest

ochashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ochashtable_unittest.cpp -o ochashtable_unittest
ochashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_OCHASHTABLE externaldict_benchmark.cpp -o ochashtable_benchmark

heap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) heap_unittest.cpp -o heap_unittest
heap_dictarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_DICTARRAY heap_benchmark.cpp -o heap_dictarray_benchmark
heap_dcarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_DCARRAY heap_benchmark.cpp -o heap_dcarray_benchmark
heap_treearray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_TREEARRAY heap_benchmark.cpp -o heap_treearray_benchmark
heap_uarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_UARRAY heap_benchmark.cpp -o heap_uarray_benchmark

list_unittest: *.h *.cpp ; $(CC) $(CFLAGS) list_unittest.cpp -o list_unittest

redblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) redblack_unittest.cpp -o redblack_unittest
redblack_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_REDBLACK externaldict_benchmark.cpp -o redblack_benchmark

ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ringbuffer_unittest.cpp -o ringbuffer_unittest

rredblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) rredblack_unittest.cpp -o rredblack_unittest
rredblack_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_RREDBLACK externaldict_benchmark.cpp -o rredblack_benchmark

trivialdict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) trivialdict_unittest.cpp -o trivialdict_unittest

set_unittest: *.h *.cpp ; $(CC) $(CFLAGS) set_unittest.cpp -o set_unittest

sort_unittest: *.h *.cpp ; $(CC) $(CFLAGS) sort_unittest.cpp -o sort_unittest
sort_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) sort_benchmark.cpp -o sort_benchmark

ts_btree_unittest: *.h *.cpp ;  $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_btree_unittest.cpp -o ts_btree_unittest
ts_btree_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_TS_BTREE internaldict_benchmark.cpp -o ts_btree_benchmark

ts_ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_ringbuffer_unittest.cpp -o ts_ringbuffer_unittest

ts_work_queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_work_queue_unittest.cpp -o ts_work_queue_unittest


