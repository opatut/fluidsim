#ifndef _QUICKSET_HPP
#define _QUICKSET_HPP

#include <stdio.h> 
#include <cstdlib> 
#include <cassert>
#include <cstring>

template <typename T>
struct QuickSet {
    QuickSet(unsigned int s) {
        size = s; 
        buf = (T*)calloc(size, sizeof(T));
    }

    void remove(unsigned int index) {
        // move last element to index position
        memcpy(&buf[index], &buf[count-1], sizeof(T));
        // shorten array by one
        count--;
    }

    T* allocate() {
        assert(count < size);
        count++;
        return &buf[count-1];
    }

    bool insert(const T& item) {
        if(count >= size) return false;
        T* t = allocate();
        memcpy(t, &item, sizeof(T));
        return true;
    }

    T& operator[](int index) {
        assert(index < size && index >= 0);
        return buf[index];   
    }

    // Members
    unsigned int count = 0;
    unsigned int size;
    T* buf;
};

#endif