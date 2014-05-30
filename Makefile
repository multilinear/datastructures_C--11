CC=g++ 
CFLAGS=-O3 -std=c++11 -Wall -Werror
CFLAGS_THREAD=-pthread

SOURCES=avl_unittest.cpp boundedheap_unittest.cpp btree_unittest.cpp dict_unittest.cpp dlist_unittest.cpp heap_unittest.cpp list_unittest.cpp redblack_unittest.cpp rredblack_unittest.cpp ringbuffer_unittest.cpp set_unittest.cpp trivialdict_unittest.cpp ts_btree_unittest.cpp ts_ringbuffer_unittest.cpp ts_work_queue_unittest.cpp

EXES=$(SOURCES:.cpp=) 

test: all ; $(SOURCES:%.cpp=./% &&) true

all: $(EXES)

$(EXES): 

clean: ;rm $(EXES); true

avl_unittest: *.h *.cpp ; $(CC) $(CFLAGS) avl_unittest.cpp -o avl_unittest

boundedheap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) boundedheap_unittest.cpp -o boundedheap_unittest

# not using CFLAGS due to a problem with an initialized variable gcc doesn't understand
btree_unittest: *.h *.cpp ; $(CC) -O3 -std=c++11 btree_unittest.cpp -o btree_unittest

dict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dict_unittest.cpp -o dict_unittest

dlist_unittest: *.h *.cpp ; $(CC) $(CFLAGS) dlist_unittest.cpp -o dlist_unittest

heap_unittest: *.h *.cpp ; $(CC) $(CFLAGS) heap_unittest.cpp -o heap_unittest

list_unittest: *.h *.cpp ; $(CC) $(CFLAGS) list_unittest.cpp -o list_unittest

redblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) redblack_unittest.cpp -o redblack_unittest

rredblack_unittest: *.h *.cpp ; $(CC) $(CFLAGS) rredblack_unittest.cpp -o rredblack_unittest

ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) ringbuffer_unittest.cpp -o ringbuffer_unittest

trivialdict_unittest: *.h *.cpp ; $(CC) $(CFLAGS) trivialdict_unittest.cpp -o trivialdict_unittest

set_unittest: *.h *.cpp ; $(CC) $(CFLAGS) set_unittest.cpp -o set_unittest

# not using CFLAGS due to a problem with an initialized variable gcc doesn't understand
ts_btree_unittest: *.h *.cpp ;  $(CC) -O3 -std=c++11 $(CFLAGS_THREAD) ts_btree_unittest.cpp -o ts_btree_unittest

ts_ringbuffer_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_ringbuffer_unittest.cpp -o ts_ringbuffer_unittest

ts_work_queue_unittest: *.h *.cpp ; $(CC) $(CFLAGS) $(CFLAGS_THREAD) ts_work_queue_unittest.cpp -o ts_work_queue_unittest
  
