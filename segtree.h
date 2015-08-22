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
                Tmpl2 Class(Iterator begin, Iterator end, Aggregator const & aggregator = Aggregator());
                ~Class();
                U query(size_t start, size_t end);
                void overwrite(size_t pos, T val);
                void overwrite(size_t l, size_t r, T val);
            private:
                enum LazyType {
                    NONE,
                    OVERWRITE,
                    INCREMENT
                };

                struct Node {
                    U val;
                    size_t start;
                    size_t end;
                    T lazy;
                    LazyType lazy_type;

                    Node(U val, size_t start, size_t end) {
                        this->val = val;
                        this->start = start;
                        this->end = end;
                        lazy = 0;
                        lazy_type = NONE;
                    }

                    void set_lazy(T lazy, LazyType lazy_type) {
                        this->lazy = lazy;
                        this->lazy_type = lazy_type;
                    }

                    void reset_lazy() {
                        lazy_type = NONE;
                    }
                };
                
                size_t tree_size_;
                char * pool_;
                Aggregator aggregator_;
                Tmpl2 U build(Iterator begin, Iterator end, size_t l, size_t r, size_t index);
                U query(size_t start, size_t end, size_t index);
                void overwrite(size_t l, size_t r, T val, size_t index);
                void propagate_lazy(size_t index, Node * n);
                void apply_lazy(Node * n, T lazy, LazyType lazy_type);
                inline Node * get_node(size_t index);
                size_t tree_size(size_t num_items) const;
                inline size_t get_range_count(size_t start, size_t end) const;
                inline U aggregate(U const & a, U const & b) const;
                inline U aggregate_times(U const & a, size_t times) const;
                inline size_t get_lindex(size_t index) const;
                inline size_t get_rindex(size_t index) const;
                inline bool node_non_trivial(Node const * n) const;
                inline bool node_outside_range(Node const * n, size_t l, size_t r) const;
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
        void ClassTmpl::overwrite(size_t pos, T val) {
            overwrite(pos, pos, val, 0);
        }

    Tmpl
        void ClassTmpl::overwrite(size_t l, size_t r, T val) {
            overwrite(l, r, val, 0);
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
        void ClassTmpl::overwrite(size_t l, size_t r, T val, size_t index) {
            Node * n = get_node(index);
            if (node_outside_range(n, l, r)) {
                return; // noop
            }

            propagate_lazy(index, n);

            if (node_within_range(n, l, r)) {
                n->val = aggregate_times(val, get_range_count(n->start, n->end));
                if (node_non_trivial(n)) {
                    n->set_lazy(val, OVERWRITE);
                }
            } else {
                // node is non-trivial
                size_t lindex = get_lindex(index);
                size_t rindex = get_rindex(index);
                overwrite(l, r, val, lindex);
                overwrite(l, r, val, rindex);

                Node * ln = get_node(lindex); 
                Node * rn = get_node(rindex); 
                n->val = aggregate(ln->val, rn->val);
            }
        }

    Tmpl
        void ClassTmpl::propagate_lazy(size_t index, Node * n) {
            if (n->lazy_type != NONE) {
                Node * ln = get_node(get_lindex(index));
                apply_lazy(ln, n->lazy, n->lazy_type);
                
                Node * rn = get_node(get_rindex(index));
                apply_lazy(rn, n->lazy, n->lazy_type);
                
                n->reset_lazy();
            }
        }

    Tmpl
        void ClassTmpl::apply_lazy(Node * n, T lazy, LazyType lazy_type) {
            U patch = aggregate_times(lazy, get_range_count(n->start, n->end));
            if (lazy_type == OVERWRITE) {
                n->val = patch; 
            } else if (lazy_type == INCREMENT) {
                n->val += patch;
            }
            if (node_non_trivial(n)) {
                n->set_lazy(lazy, lazy_type);
            }
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
