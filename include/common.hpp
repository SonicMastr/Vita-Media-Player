#pragma once
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <kernel.h>
#include <vdsuite/common/kernel/dmacmgr.h>
#include <scetypes.h>

inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems, int count) {
    std::stringstream ss(s);
    std::string item;

    int i = 0;

    while (i < count) {
        if (i != (count - 1))
            getline(ss, item, delim);
        else
            getline(ss, item);
        elems.push_back(item);
        i++;
    }
    return elems;
}
