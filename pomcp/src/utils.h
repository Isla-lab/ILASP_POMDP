#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "coord.h"
#include "memorypool.h"
#include <algorithm>
#include <random>
#include <chrono>

#define LargeInteger 1000000
#define Infinity 1e+10
#define Tiny 1e-10

#ifdef DEBUG
#define safe_cast dynamic_cast
#else
#define safe_cast static_cast
#endif

namespace UTILS
{

inline int Sign(int x)
{
    return (x > 0) - (x < 0);
}

inline int Random(int max)
{
    return rand() % max;
}

inline int Random(int min, int max)
{
    return rand() % (max - min) + min;
}

inline double RandomDouble(double min, double max)
{
    return (double) rand() / RAND_MAX * (max - min) + min;
}

inline void RandomSeed(int seed)
{
    srand(seed);
}

inline bool Bernoulli(double p)
{
    return rand() < p * RAND_MAX;
}

inline bool Near(double x, double y, double tol)
{
    return fabs(x - y) <= tol;
}

inline bool CheckFlag(int flags, int bit) { return (flags & (1 << bit)) != 0; }

inline void SetFlag(int& flags, int bit) { flags = (flags | (1 << bit)); }

template<class T>
inline bool Contains(std::vector<T>& vec, const T& item)
{
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

void UnitTest();

}

constexpr int pow2(int i) { return 1 << i; }

enum Classification {
    TRUE_POSITIVE,
    TRUE_NEGATIVE,
    FALSE_POSITIVE,
    FALSE_NEGATIVE
};

class Timer {
public:
    Timer() : start(std::chrono::steady_clock::now()) {}
    double elapsed() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - start).count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start;
};

#endif // UTILS_H
