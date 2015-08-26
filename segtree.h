#ifndef SEGTREE_H_
#define SEGTREE_H_

#include <string>

#include <stdlib.h>

namespace gokul2411s {
    template<typename T, typename U, typename Aggregator>
        class Segtree {
            public:
                /**
                 * Constructs a segment tree using the given iterable range and aggregator object.
                 */
                Segtree(Aggregator const & aggregator = Aggregator()) :
                    aggregator_(aggregator) {}

                /**
                 * Destructs the segment tree.
                 */
                virtual ~Segtree() {}

                /**
                 * Returns the aggregated result in the closed range [l, r].
                 */
                U query(size_t l, size_t r) {
                    query(l, r, root_);
                }

                /**
                 * Overwrites all elements of the closed range [l, r] with the given value.
                 */
                void overwrite(size_t l, size_t r, T const & val);

                /**
                 * Increments all elements of the the closed range [l, r] with the given value.
                 */
                void increment(size_t l, size_t r, T const & val);
            protected:
                enum UpdateType {
                    OVERWRITE,
                    INCREMENT
                };

                /**
                 * Encapsulates a value and a closed range for which that value applies.
                 *
                 * Also maintains two objects (one for overwrite updates and another for
                 * increment updates) that are used to propagate updates to children nodes lazily.
                 */
                struct Node {
                    U val;
                    size_t start;
                    size_t end;
                    T overwrite_lazy; 
                    T increment_lazy;
                    bool has_overwrite_lazy;
                    bool has_increment_lazy;

                    /**
                     * Constructs a node with the given value and the closed range.
                     */
                    Node(U const & nval, size_t nstart, size_t nend) :
                        val(nval),
                        start(nstart),
                        end(nend),
                        overwrite_lazy(0),
                        increment_lazy(0),
                        has_overwrite_lazy(false),
                        has_increment_lazy(false) {}

                    /**
                     * Sets the overwrite lazy object, invalidating the increment lazy object.
                     */
                    void set_overwrite_lazy(T const & lazy) {
                        overwrite_lazy = lazy;
                        has_overwrite_lazy = true;

                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }

                    /**
                     * Adds to the increment lazy object.
                     */
                    void add_increment_lazy(T const & lazy) {
                        increment_lazy += lazy;
                        has_increment_lazy = true;
                    }

                    /**
                     * Resets the lazy objects back to zero.
                     */
                    void reset_lazy() {
                        overwrite_lazy = 0;
                        has_overwrite_lazy = false;

                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }

                    /**
                     * Gets if a node is non-trivial i.e. if the closed range that it represents
                     * contains more than one element.
                     */
                    bool non_trivial() const {
                        return end > start;
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely outside the closed range [l, r].
                     */
                    bool outside_range(size_t l, size_t r) const {
                        return start > r || end < l;
                    }

                    /**
                     * Checks if the closed range represented by the node
                     * falls completely inside the closed range [l, r].
                     */
                    bool within_range(size_t l, size_t r) const {
                        return start >= l && end <= r;
                    }
                };

                /**
                 * Gets the left child of the given node.
                 */
                virtual Node * get_left_child(Node * n) = 0;

                /**
                 * Gets the right child of the given node.
                 */
                virtual Node * get_right_child(Node * n) = 0;

                Node * root_;                
                Aggregator aggregator_;

                /**
                 * Wraps the null method provided by the aggregator.
                 */
                U aggregator_null() const {
                    return aggregator_.null();   
                }

                /**
                 * Wraps the two-element aggregation method provided by the aggregator.
                 */
                U aggregate(U const & a, U const & b) const {
                    return aggregator_.aggregate(a, b);
                }

                /**
                 * Wraps the n-times aggregation method provided by the aggregator.
                 */
                U aggregate_times(U const & a, size_t times) const {
                    return aggregator_.aggregate_times(a, times);
                }

                /**
                 * Applies overwrite on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_overwrite_and_lazy(Node * n, T const & val) {
                    apply_overwrite(n, val);
                    if (n->non_trivial()) {
                        n->set_overwrite_lazy(val);
                    }
                }

                /**
                 * Applies increment on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_increment_and_lazy(Node * n, T const & val) {
                    apply_increment(n, val);
                    if (n->non_trivial()) {
                        n->add_increment_lazy(val);
                    }
                }

                /**
                 * Gets the update that would be applied on the node.
                 */
                U get_update_value(Node const * n, T const & val) const {
                    return aggregate_times(val, n->end - n->start + 1);
                }

                /**
                 * Applies overwrite on the node based on the given value.
                 */
                void apply_overwrite(Node * n, T const & val) {
                    n->val = get_update_value(n, val);
                }

                /**
                 * Applies increment on the node based on the given value.
                 */
                void apply_increment(Node * n, T const & val) {
                    n->val += get_update_value(n, val);
                }

                /**
                 * Applies any lazy objects from the given node to its children, if any. This also
                 * propagates the lazy objects to the children.
                 */
                void propagate_lazy(Node * n) {
                    if (!n->non_trivial()) {
                        return;
                    }

                    Node * ln = get_left_child(n);
                    Node * rn = get_right_child(n);
                    if (n->has_overwrite_lazy) {
                        apply_overwrite_and_lazy(ln, n->overwrite_lazy);
                        apply_overwrite_and_lazy(rn, n->overwrite_lazy);
                    }

                    if (n->has_increment_lazy) {
                        apply_increment_and_lazy(ln, n->increment_lazy); 
                        apply_increment_and_lazy(rn, n->increment_lazy); 
                    }

                    n->reset_lazy();
                }

                /**
                 * Recursively queries the segment tree under the node for
                 * for its contribution towards the aggregate result of the
                 * closed range [l, r].
                 */
                U query(size_t l, size_t r, Node * n) {
                    if (n->outside_range(l, r)) {
                        return aggregator_null();
                    }

                    propagate_lazy(n);

                    if (n->within_range(l, r)) {
                        return n->val;
                    } else {
                        return aggregate(query(l, r, get_left_child(n)), query(l, r, get_right_child(n)));
                    }
                }

                /**
                 * Recursively updates (overwrites or increments as specified by the update type)
                 * the segment tree using the given value under the node,
                 * for any overlap it may have with the closed range [l, r]. 
                 */
                void update(size_t l, size_t r, T const & val, Node * n, UpdateType update_type) {
                    if (n->outside_range(l, r)) {
                        return; // noop
                    }

                    propagate_lazy(n);

                    if (n->within_range(l, r)) {
                        // prefer if loop over function pointers, since most users will either call one of either increment
                        // or update most of the time, and the processor will be able to guess well. function pointers will
                        // also screw up with the compiler's optimization mechanisms.
                        if (update_type == OVERWRITE) {
                            apply_overwrite_and_lazy(n, val);
                        } else {
                            apply_increment_and_lazy(n, val);
                        }
                    } else {
                        // node is non-trivial
                        Node * ln = get_left_child(n);
                        Node * rn = get_right_child(n);
                        update(l, r, val, ln, update_type);
                        update(l, r, val, rn, update_type);
                        n->val = aggregate(ln->val, rn->val);
                    }
                }
        };

    template<typename T, typename U, typename Aggregator> 
        void Segtree<T, U, Aggregator>::overwrite(size_t l, size_t r, T const & val) {
            update(l, r, val, root_, OVERWRITE);
        }

    template<typename T, typename U, typename Aggregator> 
        void Segtree<T, U, Aggregator>::increment(size_t l, size_t r, T const & val) {
            update(l, r, val, root_, INCREMENT);
        }
}

#endif
