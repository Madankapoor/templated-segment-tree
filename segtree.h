#ifndef SEGTREE_H_
#define SEGTREE_H_

#include <string>

#include <stdlib.h>

#define pr std::cout
#define nl std::endl
#define Tmpl template<typename T, typename U, typename Aggregator>
#define Tmpl2 template<typename Iterator>
#define Class Segtree
#define ClassTmpl Class<T, U, Aggregator>

namespace gokul2411s {
    Tmpl
        class Class {
            public:
                /**
                 * Constructs a segment tree using the given iterable range and aggregator object.
                 */
                Tmpl2 Class(Iterator begin, Iterator end, Aggregator const & aggregator = Aggregator());

                /**
                 * Destructs the segment tree.
                 */
                ~Class();

                /**
                 * Returns the aggregated result in the closed range [l, r].
                 */
                U query(size_t l, size_t r);

                /**
                 * Overwrites all elements of the closed range [l, r] with the given value.
                 */
                void overwrite(size_t l, size_t r, T val);

                /**
                 * Increments all elements of the the closed range [l, r] with the given value.
                 */
                void increment(size_t l, size_t r, T val);
            private:
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
                    Node(U val, size_t start, size_t end) {
                        this->val = val;
                        this->start = start;
                        this->end = end;
                        overwrite_lazy = 0;
                        increment_lazy = 0;
                        has_overwrite_lazy = false;
                        has_increment_lazy = false;
                    }

                    /**
                     * Sets the overwrite lazy object, invalidating the increment lazy object.
                     */
                    void set_overwrite_lazy(T lazy) {
                        overwrite_lazy = lazy;
                        has_overwrite_lazy = true;
                        
                        increment_lazy = 0;
                        has_increment_lazy = false;
                    }

                    /**
                     * Adds to the increment lazy object.
                     */
                    void add_increment_lazy(T lazy) {
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
                };
                
                size_t tree_size_;
                char * pool_;
                Aggregator aggregator_;

                /**
                 * Recursively builds the segment tree under the node representing the
                 * closed range [l, r], and places that node at the given index.
                 */
                Tmpl2 U build(Iterator begin, Iterator end, size_t l, size_t r, size_t index);
                
                /**
                 * Recursively queries the segment tree under the node placed at the
                 * index for its contribution towards the aggregate result of the
                 * closed range [l, r].
                 */
                U query(size_t l, size_t r, size_t index);

                /**
                 * Recursively updates (overwrites or increments as specified by the update type)
                 * the segment tree using the given value under the node placed at the index,
                 * for any overlap it may have with the closed range [l, r]. 
                 */
                void update(size_t l, size_t r, T val, size_t index, UpdateType update_type);

                /**
                 * Applies any lazy objects from the given node to its children, if any. This also
                 * propagates the lazy objects to the children.
                 */
                void propagate_lazy(size_t index, Node * n);

                /**
                 * Applies overwrite on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_overwrite_and_lazy(Node * n, T val);

                /**
                 * Applies increment on the node based on the given value and sets the node's lazy accordingly.
                 */
                void apply_increment_and_lazy(Node * n, T val);

                /**
                 * Applies overwrite on the node based on the given value.
                 */
                void apply_overwrite(Node * n, T val);
                
                /**
                 * Applies increment on the node based on the given value.
                 */
                void apply_increment(Node * n, T val);

                /**
                 * Gets the node placed at the index.
                 */
                inline Node * get_node(size_t index);

                /**
                 * Gets the size of the array required to represent the segment tree. This computation
                 * is required since the segment tree is not truly a complete tree, but rather an
                 * almost complete tree.
                 */
                size_t tree_size(size_t num_items) const;

                /**
                 * Counts the number of elements in the specified closed range [start, end].
                 */
                inline size_t get_range_count(size_t start, size_t end) const;

                /**
                 * Wraps the two-element aggregation method provided by the aggregator.
                 */
                inline U aggregate(U const & a, U const & b) const;

                /**
                 * Wraps the n-times aggregation method provided by the aggregator.
                 */
                inline U aggregate_times(U const & a, size_t times) const;

                /**
                 * Gets the index of the left child of the node placed at index.
                 */
                inline size_t get_lindex(size_t index) const;

                /**
                 * Get the index of the right child of the node placed at index.
                 */
                inline size_t get_rindex(size_t index) const;

                /**
                 * Gets if a node is non-trivial i.e. if the closed range that it represents
                 * contains more than one element.
                 */
                inline bool node_non_trivial(Node const * n) const;

                /**
                 * Checks if the closed range represented by the node placed at index
                 * falls completely outside the closed range [l, r].
                 */
                inline bool node_outside_range(Node const * n, size_t l, size_t r) const;

                /**
                 * Checks if the closed range represented by the node placed at index
                 * falls completely inside the closed range [l, r].
                 */
                inline bool node_within_range(Node const * n, size_t l, size_t r) const;
        };

    Tmpl
        Tmpl2 ClassTmpl::Class(Iterator begin, Iterator end, Aggregator const & aggregator)
        : tree_size_(tree_size(end - begin)), aggregator_(aggregator) {
            size_t l = 0, r = end - begin - 1;
            if (r >= 0) {
                pool_ = (char *)calloc(tree_size_, sizeof(Node));
                build(begin, end, l, r, 0);
            }
        }

    Tmpl
        ClassTmpl::~Class() {
            Node * s = get_node(0);
            for (Node* p = s; p < s + tree_size_; p++) {
                if (p) {
                    p->~Node();
                }
            }

            free(pool_);
        }

    Tmpl
        U ClassTmpl::query(size_t l, size_t r) {
            return query(l, r, 0); 
        }

    Tmpl
        void ClassTmpl::overwrite(size_t l, size_t r, T val) {
            update(l, r, val, 0, OVERWRITE);
        }

    Tmpl
        void ClassTmpl::increment(size_t l, size_t r, T val) {
            update(l, r, val, 0, INCREMENT);
        }

    Tmpl
        Tmpl2 U ClassTmpl::build(Iterator begin, Iterator end, size_t l, size_t r, size_t index) {
            U val;
            if (l == r) {
                val = *(begin + l);
            } else {
                size_t mid = l + (r - l) / 2;
                U l_val(build(begin, end, l, mid, get_lindex(index)));
                U r_val(build(begin, end, mid + 1, r, get_rindex(index)));
                val = aggregate(l_val, r_val);
            }
            new (get_node(index)) Node(val, l, r);
            return val;
        }

    Tmpl
        U ClassTmpl::query(size_t l, size_t r, size_t index) {
            Node * n = get_node(index);
            if (node_outside_range(n, l, r)) {
                return aggregator_.null();
            }

            propagate_lazy(index, n);

            if (node_within_range(n, l, r)) {
                return n->val;
            } else {
                return aggregate(query(l, r, get_lindex(index)), query(l, r, get_rindex(index)));
            }
        }

    Tmpl
        void ClassTmpl::update(size_t l, size_t r, T val, size_t index, UpdateType update_type) {
            Node * n = get_node(index);
            if (node_outside_range(n, l, r)) {
                return; // noop
            }

            propagate_lazy(index, n);

            if (node_within_range(n, l, r)) {
                if (update_type == OVERWRITE) {
                    apply_overwrite_and_lazy(n, val);
                } else {
                    apply_increment_and_lazy(n, val);
                }
            } else {
                // node is non-trivial
                size_t lindex = get_lindex(index);
                size_t rindex = get_rindex(index);
                update(l, r, val, lindex, update_type);
                update(l, r, val, rindex, update_type);

                Node * ln = get_node(lindex); 
                Node * rn = get_node(rindex); 
                n->val = aggregate(ln->val, rn->val);
            }
        }

    Tmpl
        void ClassTmpl::propagate_lazy(size_t index, Node * n) {
            if (!node_non_trivial(n)) {
                return;
            }

            Node * ln = get_node(get_lindex(index));
            Node * rn = get_node(get_rindex(index));
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

    Tmpl
        void ClassTmpl::apply_overwrite_and_lazy(Node * n, T val) {
            apply_overwrite(n, val);
            if (node_non_trivial(n)) {
                n->set_overwrite_lazy(val);
            }
        }

    Tmpl
        void ClassTmpl::apply_increment_and_lazy(Node * n, T val) {
            apply_increment(n, val);
            if (node_non_trivial(n)) {
                n->add_increment_lazy(val);
            }
        }

    Tmpl
        void ClassTmpl::apply_overwrite(Node * n, T val) {
            n->val = aggregate_times(val, get_range_count(n->start, n->end));
        }

    Tmpl
        void ClassTmpl::apply_increment(Node * n, T val) {
            n->val += aggregate_times(val, get_range_count(n->start, n->end));
        }
    
    Tmpl
        typename ClassTmpl::Node * ClassTmpl::get_node(size_t index) {
            return (Node*)pool_ + index;
        }

    Tmpl
        size_t ClassTmpl::tree_size(size_t num_items) const {
            size_t psz = 1;
            while (true) {
                if (num_items <= psz) {
                    break;
                }
                psz *= 2;
            }

            return 2 * psz - 1;
        }

    Tmpl
        size_t ClassTmpl::get_range_count(size_t start, size_t end) const {
            return end - start + 1;
        }

    Tmpl
        U ClassTmpl::aggregate(U const & a, U const & b) const {
            return aggregator_.aggregate(a, b);
        }
    
    Tmpl
        U ClassTmpl::aggregate_times(U const & a, size_t times) const {
            return aggregator_.aggregate_times(a, times);
        }

    Tmpl
        size_t ClassTmpl::get_lindex(size_t index) const {
            return 2 * index + 1;
        }

    Tmpl
        size_t ClassTmpl::get_rindex(size_t index) const {
            return 2 * index + 2;
        }

    Tmpl
        bool ClassTmpl::node_non_trivial(Node const * n) const {
            return n->end > n->start;
        }

    Tmpl
        bool ClassTmpl::node_outside_range(Node const * n, size_t l, size_t r) const {
            return n->start > r || n->end < l;
        }
    
    Tmpl
        bool ClassTmpl::node_within_range(Node const * n, size_t l, size_t r) const {
            return n->start >= l && n->end <= r;
        }
}

#endif
