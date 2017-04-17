CC=g++ 
CFLAGS=-O3 -std=c++11 -Wall -Werror
CFLAGS_THREAD=-pthread

TEST_ITERATIONS=10000
TEST_SIZE=10000

# Build lists
UNITTESTS=array uarray staticarray staticuarray dictarray treearray treeuarray dcuarray zeroarray avlhashtable avl bheap boundedheap boundedhashtable btreehashtable btree dict dlist ochashtable hashtable heap list queue redblack ringbuffer rredblack set sort ts_btree ts_ringbuffer ts_work_queue
HEAPS_BENCHMARKS=bheap.cpp boundedheap.cpp heap_dcarray.cpp
# We leave out dlist 'cause it takes forever (you can add it for smaller tests)
DICTS_BENCHMARKS=avlhashtable.cpp btree.cpp ochashtable.cpp hashtable.cpp
# These are less interesting, but you can add them in if you're curious
#btreehashtable.cpp 
#rredblack.cpp 
#ts_btree.cpp
#boundedhashtable.cpp 
#avl.cpp
#redblack.cpp
BENCHMARKS=$(HEAPS_BENCHMARKS) $(DICTS_BENCHMARKS) dict_benchmark sort_benchmark

UNITTEST_EXES=$(UNITTESTS:%=%_unittest) 
BENCHMARK_EXES=$(BENCHMARKS:.cpp=_benchmark)

# Generic rule

all: benchmarks test

clean: ;rm $(UNITTEST_EXES); rm $(BENCHMARK_EXES); true

# build targets

tests: $(UNITTEST_EXES)

test: tests; $(UNITTESTS:%=./%_unittest &&) true

heaps_benchmarks: $(HEAPS_BENCHMARKS:.cpp=_benchmark)

heaps_benchmark: heaps_benchmarks; $(HEAPS_BENCHMARKS:%.cpp=./%_benchmark &&) true

dicts_benchmarks: $(DICTS_BENCHMARKS:.cpp=_benchmark)

dicts_benchmark: dicts_benchmarks; $(DICTS_BENCHMARKS:%.cpp=./%_benchmark &&) true

benchmarks: $(BENCHMARK_EXES)

benchmark: benchmarks; $(BENCHMARKS:%.cpp=./%_benchmark &&) true

# specific build rules

array_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_ARRAY array_unittest.cpp -o array_unittest
uarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_UARRAY array_unittest.cpp -o uarray_unittest
staticarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_STATICARRAY array_unittest.cpp -o staticarray_unittest
staticuarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_STATICUARRAY array_unittest.cpp -o staticuarray_unittest
dictarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_DICTARRAY array_unittest.cpp -o dictarray_unittest
treearray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_TREEARRAY array_unittest.cpp -o treearray_unittest
treeuarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_TREEUARRAY array_unittest.cpp -o treeuarray_unittest
dcuarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_DCUARRAY array_unittest.cpp -o dcuarray_unittest
zeroarray_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_ZEROARRAY array_unittest.cpp -o zeroarray_unittest

avl_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVL externaldict_unittest.cpp -o avl_unittest
avl_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVL -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o avl_benchmark

avlhashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVLHASHTABLE externaldict_unittest.cpp -o avlhashtable_unittest
avlhashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_AVLHASHTABLE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o avlhashtable_benchmark

bheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) bheap_unittest.cpp -o bheap_unittest
bheap_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BHEAP -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o bheap_benchmark

boundedhashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BOUNDEDHASHTABLE externaldict_unittest.cpp -o boundedhashtable_unittest
boundedhashtable_benchmark  : *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} -DTEST_BOUNDEDHASHTABLE externaldict_benchmark.cpp -o boundedhashtable_benchmark

boundedheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) boundedheap_unittest.cpp -o boundedheap_unittest
boundedheap_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BOUNDEDHEAP -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o boundedheap_benchmark

queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) queue_unittest.cpp -o queue_unittest

btree_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREE internaldict_unittest.cpp -o btree_unittest
btree_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} internaldict_benchmark.cpp -o btree_benchmark

btreehashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREEHASHTABLE internaldict_unittest.cpp -o btreehashtable_unittest
btreehashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_BTREEHASHTABLE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} internaldict_benchmark.cpp -o btreehashtable_benchmark

dict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dict_unittest.cpp -o dict_unittest
dict_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} dict_benchmark.cpp -o dict_benchmark

dlist_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dlist_unittest.cpp -o dlist_unittest
dlist_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_DLIST -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o dlist_benchmark

ochashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_OCHASHTABLE externaldict_unittest.cpp -o ochashtable_unittest
ochashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_OCHASHTABLE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o ochashtable_benchmark

hashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HASHTABLE internaldict_unittest.cpp -o hashtable_unittest
hashtable_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HASHTABLE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} internaldict_benchmark.cpp -o hashtable_benchmark


heap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) heap_unittest.cpp -o heap_unittest
heap_dictarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_DICTARRAY -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o heap_dictarray_benchmark
heap_dcarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_DCARRAY -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o heap_dcarray_benchmark
heap_treearray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_TREEARRAY -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o heap_treearray_benchmark
heap_uarray_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_HEAP_UARRAY -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} heap_benchmark.cpp -o heap_uarray_benchmark

list_unittest: *.h *.cpp ; $(CC) $(CFLAGS) list_unittest.cpp -o list_unittest

redblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_REDBLACK externaldict_unittest.cpp -o redblack_unittest
redblack_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_REDBLACK -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o redblack_benchmark

ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ringbuffer_unittest.cpp -o ringbuffer_unittest

rredblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_RREDBLACK externaldict_unittest.cpp -o rredblack_unittest
rredblack_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_RREDBLACK -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} externaldict_benchmark.cpp -o rredblack_benchmark

set_unittest: *.h *.cpp ; $(CC) $(CFLAGS) set_unittest.cpp -o set_unittest

sort_unittest: *.h *.cpp ; $(CC) $(CFLAGS) sort_unittest.cpp -o sort_unittest
sort_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} sort_benchmark.cpp -o sort_benchmark

ts_btree_unittest: *.h *.cpp ;  $(CC) $(CFLAGS) $(CFLAGS_THREAD) -DTEST_TS_BTREE internaldict_unittest.cpp -o ts_btree_unittest
ts_btree_benchmark: *.h *.cpp ; $(CC) $(CFLAGS) -DTEST_TS_BTREE -DTEST_ITERATIONS=${TEST_ITERATIONS} -DTEST_SIZE=${TEST_SIZE} internaldict_benchmark.cpp -o ts_btree_benchmark

ts_ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_ringbuffer_unittest.cpp -o ts_ringbuffer_unittest

ts_work_queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_work_queue_unittest.cpp -o ts_work_queue_unittest


