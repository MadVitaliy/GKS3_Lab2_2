//
// Created by Vitalik on 08.04.2021.
//

#ifndef LABA2PART2_ZIPUNZIP_H
#define LABA2PART2_ZIPUNZIP_H

#include <vector>

template <typename A, typename B>
void zip(
        const std::vector<A> &a,
        const std::vector<B> &b,
        std::vector<std::pair<A,B>> &zipped)
{
    for(size_t i=0; i<a.size(); ++i)
    {
        zipped.push_back(std::make_pair(a[i], b[i]));
    }
}

template <typename A, typename B>
void unzip(
        const std::vector<std::pair<A, B>> &zipped,
        std::vector<A> &a,
        std::vector<B> &b)
{
    for(size_t i=0; i<a.size(); i++)
    {
        a[i] = zipped[i].first;
        b[i] = zipped[i].second;
    }
}


#endif //LABA2PART2_ZIPUNZIP_H
