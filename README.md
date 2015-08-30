# templated-segment-tree

## Introduction
This repo includes some templated C++ implementations of segment trees which can work for associative aggregation functions such as sum, product, min and max. Segment trees are very fast for finding range aggregates on a iterable object (such as an array), while allowing for range updates.

Three variants of implementations are given.
 * The standard array based implementation (array_based_segtree.h) works for 1-dimensional iterables and offers range query and range update methods.
 * The stack-like implementation (tree_based_segtree.h) works for 1-dimensional iterables and offers range query and push / pop operations.
 * The node based implementation (node_based_segtree_nd.h) works for N-dimensional iterables and offers range query and range update methods. This implementation could very well have been done in array-style, but is done in the node style just for illustration.

## Time complexity
The standard implementation provided has the following runtimes.
 * Constructing a segment tree - O(n)
 * Querying for the aggregate in a range - O(log n)
 * Overwriting all elements of a range to a value - O(log n)
 * Incrementing all elements of a range to a value - O(log n)

Range updates are made possible in O(log n) time using a method called lazy propagation.

Why don't you infer the run times for the other implementations yourself (as an exercise?)

## Implementation and usage
This repo contains various implementations, whose usage can be inferred by reading the code. However, for the N-dimensional implementation in node_based_segtree.h, the usage snippets are provided below.

```
#define Type int

struct SumAggregator {
    // Returns the result of aggregating two elements.
    Type aggregate(Type const & a, Type const & b) const {
        return a + b;
    }

    // Returns the result of aggregating the input to itself n times.
    // For min and max, the output would be just the input itself.
    // For products, the output would be the nth power of the input.
    Type aggregate_times(Type const & a, size_t n) const {
        return a * n;
    }

    // Returns the null element.
    // For max you would most likely use std::numeric_limits<Type>::min().
    // For min you would most likely use std::numeric_limits<Type>::max().
    // For products you would most likely use 1.
    Type null() const {
        return 0;
    }
};

// 2-D example: (l, r) defines a point in a matrix
struct Point {
    size_t l;
    size_t r;

    Point(size_t ll, size_t rr) :
        l(ll), r(rr) {}

    size_t operator[] (size_t k) const {
        if (k == 0) { return l; }
        else { return r; }
    }

    bool operator == (Point const & other) const {
        return l == other.l && r == other.r;
    }
    
    bool operator != (Point const & other) const {
        return l != other.l || r != other.r;
    }

    Point operator - (Point const & other) const {
        return Point(l - other.l, r - other.r);
    }
    
    Point operator + (Point const & other) const {
        return Point(l + other.l, r + other.r);
    }

    Point operator / (int divisor) const {
        return Point(l / divisor, r / divisor);
    }

    void set(size_t k, size_t val) {
        if (k == 0) { l = val; }
        else { r = val; }
    }
};

#include <vector>
using namespace std;
#define vT vector<Type>
#define vvT vector<vT>

// wraps a 2D iterable and adds a few methods
struct Matrix {
    vvT * real;

    Matrix(vvT * vv) :
        real(vv) {}

    // gets size along k'th dimension
    size_t size(size_t k) const {
        if (k == 0) { return real->size(); }
        else { return real->operator[](0).size(); }
    }

    // gets the value of the matrix at the given point
    Type get(Point const & a) const {
        return real->operator[](a.l)[a.r];
    }
};

...

// the source array
vvT vv;
vv.resize(5);
for (int i = 0; i < 5; i++) {
    vv[i].resize(5);
}

// create a segment tree.
Point start(0, 0), end(4, 4);
gokul2411s::NodeBasedNdSegtree<Type, Type, SumAggregator, Point, size_t(2)> ss(matrix, start, end, SumAggregator());

// Next we overwrite each element in the range (2, 3) -> (4, 4) to 5.
Point overwrite_start(2, 3), overwrite_end(4, 4);
ss.overwrite(overwrite_start, overwrite_end, 5);

// similarly for increment

// And then we query for the sum in the range (0, 2) -> (3, 4)
Point query_start(0, 2), query_end(3, 4);
ss.query(query_start, query_end);
```
## Applicability
Lots of programming competitions assume that you know about segment trees. While writing one from scratch is a great learning experience, it can be cumbersome to get perfectly right the very first time, especially under the context of a time crunch. This implementation is well tested and can be used out of the box, saving users a lot of time. 

## Future work
This implementation is very powerful and fast. However, following caveats exist. If you see any more, please contact me, and give me your inputs. I am all ears.

 * Segment trees can also handle range updates with some fixed function over the range, rather than just a constant. This implementation does not support this.
 * Range minimum / maximum queries can be handled even faster than by using a segment tree. Segment trees will still be powerful in practice, but can be beaten in some adversarial scenarios.
 * There is no thread-safety (although this can be accomplished by wrapping the implementation with locking constructs).
