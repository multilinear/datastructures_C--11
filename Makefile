test: all ; ./avl_unittest && ./btree_unittest && ./dlist_unittest && ./list_unittest && ./rredblack_unittest && ./redblack_unittest && ./ringbuffer_unittest && ./trivialdict_unittest && ./ts_btree_unittest && ./ts_ringbuffer_unittest && ./ts_work_queue_unittest

all: avl_unittest btree_unittest dlist_unittest list_unittest redblack_unittest rredblack_unittest ringbuffer_unittest trivialdict_unittest ts_btree_unittest ts_ringbuffer_unittest ts_work_queue_unittest


avl_unittest: *.h *.cpp ; g++ -O3 --std=c++11 avl_unittest.cpp -o avl_unittest

btree_unittest: *.h *.cpp ; g++ -O3 --std=c++11 btree_unittest.cpp -o btree_unittest

dlist_unittest: *.h *.cpp ; g++ --std=c++11 dlist_unittest.cpp -o dlist_unittest

list_unittest: *.h *.cpp ; g++ --std=c++11 list_unittest.cpp -o list_unittest

redblack_unittest: *.h *.cpp ; g++ -O3 --std=c++11 redblack_unittest.cpp -o redblack_unittest

rredblack_unittest: *.h *.cpp ; g++ -O3 --std=c++11 rredblack_unittest.cpp -o rredblack_unittest

ringbuffer_unittest: *.h *.cpp ; g++ --std=c++11 ringbuffer_unittest.cpp -o ringbuffer_unittest

trivialdict_unittest: *.h *.cpp ; g++ -O3 --std=c++11 trivialdict_unittest.cpp -o trivialdict_unittest

ts_btree_unittest: *.h *.cpp ; g++ -O3 --std=c++11 -pthread ts_btree_unittest.cpp -o ts_btree_unittest

ts_ringbuffer_unittest: *.h *.cpp ; g++ --std=c++11 -pthread ts_ringbuffer_unittest.cpp -o ts_ringbuffer_unittest

ts_work_queue_unittest: *.h *.cpp ; g++ --std=c++11 -pthread ts_work_queue_unittest.cpp -o ts_work_queue_unittest

clean: ; rm avl_unittest btree_unittest dlist_unittest list_unittest redblack_unittest rredblack_unittest ringbuffer_unittest trivialdict_unittest ts_btree_unittest ts_ringbuffer_unittest ts_work_queue_unittest; true

  
