CC=g++ 
CFLAGS=-O3 -std=c++11 -Wall -Werror
CFLAGS_THREAD=-pthread

SOURCES=array_unittest.cpp avlhashtable_unittest.cpp avl_unittest.cpp bheap_unittest.cpp boundedheap_unittest.cpp btreehashtable_unittest.cpp btree_unittest.cpp dict_unittest.cpp dlist_unittest.cpp ochashtable_unittest.cpp heap_unittest.cpp list_unittest.cpp queue_unittest.cpp redblack_unittest.cpp ringbuffer_unittest.cpp rredblack_unittest.cpp set_unittest.cpp sort_unittest.cpp trivialdict_unittest.cpp ts_btree_unittest.cpp ts_ringbuffer_unittest.cpp ts_work_queue_unittest.cpp

EXES=$(SOURCES:.cpp=) 

test: all ; $(SOURCES:%.cpp=./% &&) true

all: $(EXES)

$(EXES): 

clean: ;rm $(EXES); true

array_unittest: *.h *.cpp ; $(CC) $(CFLAGS) array_unittest.cpp -o array_unittest

avl_unittest: *.h *.cpp ; $(CC) $(CFLAGS) avl_unittest.cpp -o avl_unittest

avlhashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) avlhashtable_unittest.cpp -o avlhashtable_unittest

bheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) bheap_unittest.cpp -o bheap_unittest

boundedheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) boundedheap_unittest.cpp -o boundedheap_unittest

queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) queue_unittest.cpp -o queue_unittest

btreehashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) btreehashtable_unittest.cpp -o btreehashtable_unittest

btree_unittest: *.h *.cpp ; $(CC) $(CFLAGS) btree_unittest.cpp -o btree_unittest

dict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dict_unittest.cpp -o dict_unittest

dlist_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dlist_unittest.cpp -o dlist_unittest

ochashtable_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ochashtable_unittest.cpp -o ochashtable_unittest

heap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) heap_unittest.cpp -o heap_unittest

list_unittest: *.h *.cpp ; $(CC) $(CFLAGS) list_unittest.cpp -o list_unittest

redblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) redblack_unittest.cpp -o redblack_unittest

ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ringbuffer_unittest.cpp -o ringbuffer_unittest

rredblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) rredblack_unittest.cpp -o rredblack_unittest

trivialdict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) trivialdict_unittest.cpp -o trivialdict_unittest

set_unittest: *.h *.cpp ; $(CC) $(CFLAGS) set_unittest.cpp -o set_unittest

sort_unittest: *.h *.cpp ; $(CC) $(CFLAGS) sort_unittest.cpp -o sort_unittest

ts_btree_unittest: *.h *.cpp ;  $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_btree_unittest.cpp -o ts_btree_unittest

ts_ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_ringbuffer_unittest.cpp -o ts_ringbuffer_unittest

ts_work_queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_work_queue_unittest.cpp -o ts_work_queue_unittest
  
