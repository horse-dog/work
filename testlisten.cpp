#include <iostream>
#include <list>
#include <unordered_map>
using namespace std;


class LRUCache {
protected:
    struct DLinkedNode {
        int _M_key;
        int _M_val;
        DLinkedNode* _M_prev = 0;
        DLinkedNode* _M_next = 0;
    };

    struct DLinkedList {
        DLinkedNode _M_head;
        DLinkedNode _M_tail;
        
        DLinkedList() {
            _M_head._M_next = &_M_tail;
            _M_tail._M_prev = &_M_head;
        }

        ~DLinkedList() {
            DLinkedNode* __node = _M_head._M_next;
            while (__node != &_M_tail) {
                DLinkedNode* __next = __node->_M_next;
                delete __node;
                __node = __next;
            }
        }

        DLinkedNode* Front() const {
            return _M_head._M_next == &_M_tail ? 0 : _M_head._M_next;
        }

        DLinkedNode* Back() const {
            return _M_tail._M_prev == &_M_head ? 0 : _M_tail._M_prev;
        }

        void MoveToHead(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushFront(__node);
        }

        void MoveToTail(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushBack(__node);
        }

        void PushFront(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            _M_pushFront(__node);
        }

        void PushBack(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            _M_pushBack(__node);
        }

        void PopFront() {
            if (!Empty()) {
                DLinkedNode* __head = _M_head._M_next;
                _M_remove(__head);
                delete __head;
            }
        }

        void PopBack() {
            if (!Empty()) {
                DLinkedNode* __tail = _M_tail._M_prev;
                _M_remove(__tail);
                delete __tail;
            }
        }

        void Remove(DLinkedNode* __node) {
            _M_remove(__node);
            delete __node;
        }

        void MoveToAnotherHead(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushFront(__node);
        }

        void MoveToAnotherTail(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushBack(__node);
        }

        bool Empty() const {
            return _M_head._M_next == &_M_tail;
        }

    protected:
        void _M_remove(DLinkedNode* __node) {
            if (__node == 0) return;
            // __prev <-> __node <-> __next.
            DLinkedNode* __prev = __node->_M_prev;
            DLinkedNode* __next = __node->_M_next;
            __prev->_M_next = __next;
            __next->_M_prev = __prev;
        }

        void _M_pushFront(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_prev = &_M_head;
            __node->_M_next = _M_head._M_next;
            _M_head._M_next->_M_prev = __node;
            _M_head._M_next = __node;
        }
        
        void _M_pushBack(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_next = &_M_tail;
            __node->_M_prev = _M_tail._M_prev;
            _M_tail._M_prev->_M_next = __node;
            _M_tail._M_prev = __node;
        }
    };

public:
    LRUCache(int capacity) { _M_cap = capacity; }

    int get(int key) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            return -1;
        }
        // key is exist.
        DLinkedNode* __node = __it->second;
        _M_lru_list.MoveToHead(__node);
        return __node->_M_val;
    }

    void put(int key, int value) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            if (_M_cache.size() == _M_cap) {
                DLinkedNode* __last = _M_lru_list.Back();
                _M_cache.erase(__last->_M_key);
                _M_lru_list.PopBack();
            }
            _M_lru_list.PushFront(key, value);
            _M_cache.emplace(key, _M_lru_list.Front());
        } else {
            // key is exist, refresh it.
            DLinkedNode* __node = __it->second;
            __node->_M_val = value;
            _M_lru_list.MoveToHead(__node);
        }
    }

private:
    int _M_cap; // always > 0.
    std::unordered_map<int, DLinkedNode*> _M_cache;
    DLinkedList _M_lru_list;
};


class LFUCache {
protected:
    struct DLinkedNode {
        int _M_key;
        int _M_val;
        int _M_use_count;
        DLinkedNode* _M_prev = 0;
        DLinkedNode* _M_next = 0;
    };

    struct DLinkedList {
        DLinkedNode _M_head;
        DLinkedNode _M_tail;
        
        DLinkedList() {
            _M_head._M_next = &_M_tail;
            _M_tail._M_prev = &_M_head;
        }

        ~DLinkedList() {
            DLinkedNode* __node = _M_head._M_next;
            while (__node != &_M_tail) {
                DLinkedNode* __next = __node->_M_next;
                delete __node;
                __node = __next;
            }
        }

        DLinkedNode* Front() const {
            return _M_head._M_next == &_M_tail ? 0 : _M_head._M_next;
        }

        DLinkedNode* Back() const {
            return _M_tail._M_prev == &_M_head ? 0 : _M_tail._M_prev;
        }

        void MoveToHead(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushFront(__node);
        }

        void MoveToTail(DLinkedNode* __node) {
            _M_remove(__node);
            _M_pushBack(__node);
        }

        void PushFront(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            __node->_M_use_count = 1;
            _M_pushFront(__node);
        }

        void PushBack(int __k, int __v) {
            DLinkedNode* __node = new DLinkedNode{};
            __node->_M_key = __k;
            __node->_M_val = __v;
            __node->_M_use_count = 1;
            _M_pushBack(__node);
        }

        void PopFront() {
            if (!Empty()) {
                DLinkedNode* __head = _M_head._M_next;
                _M_remove(__head);
                delete __head;
            }
        }

        void PopBack() {
            if (!Empty()) {
                DLinkedNode* __tail = _M_tail._M_prev;
                _M_remove(__tail);
                delete __tail;
            }
        }

        void Remove(DLinkedNode* __node) {
            _M_remove(__node);
            delete __node;
        }

        void MoveToAnotherHead(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushFront(__node);
        }

        void MoveToAnotherTail(DLinkedNode* __node, DLinkedList& __list) {
            _M_remove(__node);
            __list._M_pushBack(__node);
        }

        bool Empty() const {
            return _M_head._M_next == &_M_tail;
        }

    protected:
        void _M_remove(DLinkedNode* __node) {
            if (__node == 0) return;
            // __prev <-> __node <-> __next.
            DLinkedNode* __prev = __node->_M_prev;
            DLinkedNode* __next = __node->_M_next;
            __prev->_M_next = __next;
            __next->_M_prev = __prev;
        }

        void _M_pushFront(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_prev = &_M_head;
            __node->_M_next = _M_head._M_next;
            _M_head._M_next->_M_prev = __node;
            _M_head._M_next = __node;
        }
        
        void _M_pushBack(DLinkedNode* __node) {
            if (__node == 0) return;
            __node->_M_next = &_M_tail;
            __node->_M_prev = _M_tail._M_prev;
            _M_tail._M_prev->_M_next = __node;
            _M_tail._M_prev = __node;
        }
    };

public:
    LFUCache(int capacity) {
        _M_capacity = capacity;
    }
    
    int get(int key) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            return -1;
        }
        DLinkedNode* __node = __it->second;
        int __freq = __node->_M_use_count++;
        int __value = __node->_M_val;
        auto&& __list1 = _M_freq_cache[__freq];
        auto&& __list2 = _M_freq_cache[__freq + 1];
        __list1.MoveToAnotherHead(__node, __list2);
        if (_M_cur_min_freq == __freq && __list1.Empty()) {
            _M_cur_min_freq += 1;
        }
        return __value;
    }
    
    void put(int key, int value) {
        auto __it = _M_cache.find(key);
        if (__it == _M_cache.end()) {
            if (_M_cache.size() == _M_capacity) {
                auto&& __list = _M_freq_cache[_M_cur_min_freq];
                int __key = __list.Back()->_M_key;
                _M_cache.erase(__key);
                __list.PopBack();
            }
            auto&& __list = _M_freq_cache[1];
            __list.PushFront(key, value);
            _M_cache.emplace(key, __list.Front());
            _M_cur_min_freq = 1;
        } else {
            DLinkedNode* __node = __it->second;
            int __freq = __node->_M_use_count++;
            __node->_M_val = value;
            auto&& __list1 = _M_freq_cache[__freq];
            auto&& __list2 = _M_freq_cache[__freq + 1];
            __list1.MoveToAnotherHead(__node, __list2);
            if (_M_cur_min_freq == __freq && __list1.Empty()) {
                _M_cur_min_freq += 1;
            }
        }
    }

protected:
    std::unordered_map<int, DLinkedNode*> _M_cache;
    std::unordered_map<int, DLinkedList> _M_freq_cache;
    int _M_cur_min_freq;
    int _M_capacity;
};
