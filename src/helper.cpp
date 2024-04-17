#include <vector>
#include <iostream>
#include "helper.h"

#ifndef INCLUDE_HELPER
#define INCLUDE_HELPER

void split(const std::string &source,
           const std::string &deliminator,
           std::vector<std::string> &l,
           int count)
{
    // std::cout << "spliting " << source << " !!" << std::endl;
    size_t pointer = 0;

    while (count != 0)
    {
        size_t prevPointer = pointer;
        pointer = source.find(deliminator, prevPointer);

        l.emplace_back(
            source.substr(prevPointer, pointer - prevPointer));

        if (pointer >= source.length())
            break;

        pointer += deliminator.length();
        count--;
    }
}

#endif