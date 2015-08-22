# templated-segment-tree

## Introduction
This repo includes a templated C++ implementation of segment trees which can work for common aggregation functions such as sum, product, min and max. Segment trees are very fast for finding range aggregates on a iterable object (such as an array), while allowing for range updates. The only constraint is that the number of elements in the iterable should remain fixed.

## Time complexity
The implementation provided has the following runtimes.
 * Constructing a segment tree - O(n)
 * Querying for the aggregate in a range - O(log n)
 * Overwriting all elements of a range to a value - O(log n)
 * Incrementing all elements of a range to a value - O(log n)

Range updates are made possible in O(log n) time using a method called lazy propagation.

## Implementation and usage
The class SegTree, available within the gokul2411s namespace is defined in segtree.h. It is templated on three parameters.

 * the type of object contained in the original iterable
 * the type of aggregate requested in queries (this allows aggregates to have a different type)
 * the type of aggregator, a class that defines how an aggregator works. The following shows a short code snippet.

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

// Here is our source iterable (in this case an array).
Type a[5] = {1, 2, 3, 4, 5};

// Here we create a segment tree.
gokul2411s::Segtree<Type, Type, SumAggregator> ss(a, a+5, SumAggregator() /* optional */);

// Next we overwrite each element in the range [2, 3] to 5.
ss.overwrite(2, 3, 5);

// Next we increment each element in the range [2, 4] by 3.
ss.increment(2, 4, 3);

// And then we query for the sum in the range[0, 4], which should return 27 ( = 1 + 2 + 8 + 8 + 8).
ss.query(0, 4);
```
