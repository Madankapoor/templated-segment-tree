#ifndef SEGTREE_H_
#define SEGTREE_H_

#include <string>

#include <stdlib.h>

#define pr std::cout
#define nl std::endl
#define Tmpl template<typename T, typename U, typename Evaluator>
#define Tmpl2 template<typename Iterator>
#define Class Segtree
#define ClassTmpl Class<T, U, Evaluator>

namespace gokul2411s {
    Tmpl
        class Class {
            public:
                Tmpl2 Class(Iterator begin, Iterator end, Evaluator const & evaluator = Evaluator());
                ~Class();
                U query(size_t start, size_t end);
                void update(size_t pos, T val);
                void update(size_t l, size_t r, T val);
            private:
                struct Node {
                    U val;
                    size_t start;
                    size_t end;
                    T lazy;
                    bool has_lazy;
                    Node(U val, size_t start, size_t end) {
                        this->val = val;
                        this->start = start;
                        this->end = end;
                        this->lazy = 0;
                        this->has_lazy = false;
                    }

                    void set_lazy(T lazy, bool reset = false) {
                        this->lazy = lazy;
                        has_lazy = !reset;
                    }
                };
                
                size_t tree_size_;
                char * pool_;
                Evaluator evaluator_;
                Tmpl2 U build(Iterator begin, Iterator end, size_t l, size_t r, size_t index);
                U query(size_t start, size_t end, size_t index);
                void update(size_t l, size_t r, T val, size_t index);
                void apply_lazy(size_t index, Node * n);
                inline Node * get_node(size_t index);
                size_t tree_size(size_t num_items) const;
                inline U evaluate(U const & a, U const & b) const;
                inline U evaluate_times(U const & a, size_t times) const;
                inline size_t get_lindex(size_t index) const;
                inline size_t get_rindex(size_t index) const;
                inline bool node_non_trivial(Node const * n) const;
                inline bool node_outside_range(Node const * n, size_t l, size_t r) const;
                inline bool node_within_range(Node const * n, size_t l, size_t r) const;
        };

    Tmpl
        Tmpl2 ClassTmpl::Class(Iterator begin, Iterator end, Evaluator const & evaluator)
        : tree_size_(tree_size(end - begin)), evaluator_(evaluator) {
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
        void ClassTmpl::update(size_t pos, T val) {
            update(pos, pos, val, 0);
        }

    Tmpl
        void ClassTmpl::update(size_t l, size_t r, T val) {
            update(l, r, val, 0);
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
                val = evaluate(l_val, r_val);
            }
            new (get_node(index)) Node(val, l, r);
            return val;
        }

    Tmpl
        U ClassTmpl::query(size_t l, size_t r, size_t index) {
            Node * n = get_node(index);
            if (node_outside_range(n, l, r)) {
                return evaluator_.null();
            }

            apply_lazy(index, n);

            if (node_within_range(n, l, r)) {
                return n->val;
            } else {
                return evaluate(query(l, r, get_lindex(index)), query(l, r, get_rindex(index)));
            }
        }

    Tmpl
        void ClassTmpl::update(size_t l, size_t r, T val, size_t index) {
            Node * n = get_node(index);
            if (node_outside_range(n, l, r)) {
                return; // noop
            }

            apply_lazy(index, n);

            if (node_within_range(n, l, r)) {
                n->val = evaluate_times(val, n->end - n->start + 1);
                if (node_non_trivial(n)) {
                    n->set_lazy(val);
                }
            } else {
                // node is non-trivial
                size_t lindex = get_lindex(index);
                size_t rindex = get_rindex(index);
                update(l, r, val, lindex);
                update(l, r, val, rindex);

                Node * ln = get_node(lindex); 
                Node * rn = get_node(rindex); 
                n->val = evaluate(ln->val, rn->val);
            }
        }

    Tmpl
        void ClassTmpl::apply_lazy(size_t index, Node * n) {
            if (n->has_lazy) {
                Node * ln = get_node(get_lindex(index));
                ln->val = evaluate_times(n->lazy, ln->end - ln->start + 1);
                if (node_non_trivial(ln)) {
                    ln->set_lazy(n->lazy);
                }
                Node * rn = get_node(get_rindex(index));
                rn->val = evaluate_times(n->lazy, rn->end - rn->start + 1);
                if (node_non_trivial(rn)) {
                    rn->set_lazy(n->lazy);
                }
                n->set_lazy(0, true);
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
        U ClassTmpl::evaluate(U const & a, U const & b) const {
            return evaluator_.evaluate(a, b);
        }
    
    Tmpl
        U ClassTmpl::evaluate_times(U const & a, size_t times) const {
            return evaluator_.evaluate_times(a, times);
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
