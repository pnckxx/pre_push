//
// Created by pnck on 2018/11/19.
// NOTE: just don't care about this file, it's not for you to read
//

#include <catch.hpp>
#include <type_traits>
#include <iostream>
#include <functional>
#include <random>
#include <list>
#include <deque>

template<typename C1, typename C2>
struct same_member_sig : std::is_same<C1, C2> {};

template<typename R, typename C1, typename C2>
struct same_member_sig<R C1::*, R C2::*> : std::true_type {};


enum FEATURES {
    HAS_FOO = 1 << 10,
    HAS_OPERATOR_PLUS = 1 << 11,
    HAS_OPERATOR_INC = 1 << 12,
    HAS_OPERATOR_SUBSCRIPT = 1 << 13,
    HAS_OPERATOR_DEREF = 1 << 14
};

template<typename T>
class has_foo {
    void _ref(int n) {}
    template<typename U>
    static constexpr size_t _help(
            typename std::enable_if<same_member_sig<decltype(&U::foo), decltype(&has_foo::_ref)>::value, int>::type) noexcept { return HAS_FOO + 1; }
    template<typename U>
    static constexpr size_t _help(decltype(&U::foo)) noexcept { return HAS_FOO; }
    template<typename U>
    static constexpr size_t _help(...) noexcept { return 0; }
public:
    static constexpr size_t value = _help<T>(0);
};

template<typename T>
class has_plus {
    template<typename U>
    static constexpr size_t _help(decltype(&U::operator+)) noexcept { return HAS_OPERATOR_PLUS; }
    template<typename U>
    static constexpr size_t _help(...) noexcept { return 0; }
public:
    static constexpr size_t value = _help<T>(0);
};


template<typename T>
class has_inc {
    template<typename U>
    static constexpr size_t _help(decltype(&U::operator++)) noexcept { return HAS_OPERATOR_INC; }
    template<typename U>
    static constexpr size_t _help(...) noexcept { return 0; }
public:
    static constexpr size_t value = _help<T>(0);
};


template<typename T>
class has_subscript {
    template<typename U>
    static constexpr size_t _help(decltype(&U::operator[])) noexcept { return HAS_OPERATOR_SUBSCRIPT; }
    template<typename U>
    static constexpr size_t _help(...) noexcept { return 0; }
public:
    static constexpr size_t value = _help<T>(0);
};

template<typename T>
class has_dereference {
    template<typename U>
    static constexpr size_t _help(decltype(&U::operator*)) noexcept { return HAS_OPERATOR_DEREF; }
    template<typename U>
    static constexpr size_t _help(...) noexcept { return 0; }
public:
    static constexpr size_t value = _help<T>(0);
};


template<size_t n>
struct num_type {
    static constexpr size_t value = n;
};

template<typename _type>
struct operation {}; // prototype

template<size_t N>
struct operation<num_type<N>> {
    void operator()() {

    }
};

template<>
struct operation<num_type<HAS_FOO>> {
    void operator()() {
        std::cout << "HAS_FOO" << std::endl;
    }
};

template<>
struct operation<num_type<HAS_OPERATOR_PLUS>> {
    void operator()() {
        std::cout << "HAS_OPERATOR_PLUS" << std::endl;
    }
};


template<>
struct operation<num_type<HAS_OPERATOR_INC>> {
    void operator()() {
        std::cout << "HAS_OPERATOR_INC" << std::endl;
    }
};

template<>
struct operation<num_type<HAS_OPERATOR_SUBSCRIPT>> {
    void operator()() {
        std::cout << "HAS_OPERATOR_SUBSCRIPT" << std::endl;
    }
};

template<>
struct operation<num_type<HAS_OPERATOR_DEREF>> {
    void operator()() {
        std::cout << "HAS_OPERATOR_DEREF" << std::endl;
    }
};

template<typename T>
struct feature_trait {
    static constexpr size_t value = has_foo<T>::value | has_plus<T>::value | has_inc<T>::value | has_subscript<T>::value | has_dereference<T>::value;
    void operator()() {
        operation<num_type<value & HAS_FOO>>()();
        operation<num_type<value & HAS_OPERATOR_PLUS>>()();
        operation<num_type<value & HAS_OPERATOR_INC>>()();
        operation<num_type<value & HAS_OPERATOR_SUBSCRIPT>>()();
        operation<num_type<value & HAS_OPERATOR_DEREF>>()();
    }
};

struct TesterBase {
    virtual void run() = 0;
    virtual ~TesterBase() = default;
    using instance_type = std::unique_ptr<TesterBase>;
};

template<typename T>
class FeatureTester : TesterBase {
    feature_trait<T> method_;
    uint64_t line_;
public:
    FeatureTester() : method_(feature_trait<T>()), line_(0) {};
    explicit FeatureTester(uint64_t line) : method_(feature_trait<T>()), line_(line) {}
    static std::unique_ptr<TesterBase> create(uint64_t line = 0) {
        return std::unique_ptr<TesterBase>(new FeatureTester<T>(line));
    }
    void run() override {
        std::cout << "__LINE__ = " << line_ << std::endl;
        method_();
    }
};


struct HASFOO {
    void foo(int a) {}
    void operator++() {}
};

struct FOOTYPEMISMATCH {
    void foo() {}
};

struct NOFOO {
    int operator+(int) { return 1; }
};


TEST_CASE("NOT A REAL TEST") {
    std::cout << feature_trait<HASFOO>::value << ',' << feature_trait<NOFOO>::value << ',' << feature_trait<FOOTYPEMISMATCH>::value << std::endl;
    FeatureTester<HASFOO>().run();
    FeatureTester<NOFOO>().run();
    FeatureTester<FOOTYPEMISMATCH>().run();
    std::vector<TesterBase::instance_type> tmp;

    tmp.emplace_back(FeatureTester<HASFOO>::create(__LINE__));
    tmp.emplace_back(FeatureTester<NOFOO>::create(__LINE__));
    tmp.emplace_back(FeatureTester<FOOTYPEMISMATCH>::create(__LINE__));
    tmp.emplace_back(FeatureTester<FOOTYPEMISMATCH>::create(__LINE__));
    tmp.emplace_back(FeatureTester<FOOTYPEMISMATCH>::create(__LINE__));
    tmp.emplace_back(FeatureTester<HASFOO>::create(__LINE__));
    tmp.emplace_back(FeatureTester<NOFOO>::create(__LINE__));
    tmp.emplace_back(FeatureTester<HASFOO>::create(__LINE__));
    tmp.emplace_back(FeatureTester<NOFOO>::create(__LINE__));


    std::shuffle(tmp.begin(), tmp.end(), std::mt19937(std::random_device()()));// randomized vector

    std::cout << "---------------------------------" << std::endl;
    for (auto &t : tmp) {
        t->run();
    }

    FeatureTester<std::vector<int>::iterator>::create(__LINE__)->run();
    FeatureTester<std::list<int>::iterator>::create(__LINE__)->run();
    FeatureTester<std::deque<int>::iterator>::create(__LINE__)->run();

}
