//
// Created by pnck on 2018/11/16.
//

#include <catch.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <type_traits>
#include <random>
#include <list>


#ifdef NODEBUG
#define DEBUG_MESSAGE(...)
#define RANDOM_STR_MAXLEN 2000 // find_simple is too slow to handle long string
#define RANDOM_CASES 500
#define RANDOM_STR_CHARSET "123ABCxyz~!@_"
#else
#define DEBUG_MESSAGE(FMT, ...) fprintf(stderr, "@[%d] " FMT "\n" ,__LINE__,__VA_ARGS__);
#define RANDOM_STR_MAXLEN 100
#define RANDOM_CASES 100
#define RANDOM_STR_CHARSET "123qp"
#endif
bool is_palindrome(const std::string &s) {
    if (s.empty() || s.length() == 1) {
        return false;
    }
    auto il = s.cbegin();
    auto ir = s.cend();
    --ir;
    while (il < ir) {
        if (*il != *ir) {
            return false;
        }
        ++il;
        --ir;
    }
    return true;
}

std::string dump(std::vector<std::string> v) {
    std::stringstream ss;
    ss << '[';

    for (const auto &s : v) {
        ss << '\"' << s << '\"' << ',';
    }
    if (!v.empty()) {
        auto pos = ss.tellp();
        pos -= 1;
        ss.seekp(pos);
    }
    ss << "]";
    return ss.str();
}

std::vector<std::string> find_palindrome_simple(const std::string &text) {
    size_t len = text.length();
    std::vector<std::string> candidate_list;// storage all possible sequences
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = i; j < len; ++j) {
            auto t = text.substr(i, len - j);
            if (is_palindrome(t)) {
                DEBUG_MESSAGE("==> [%zu,%zu]:%s", i, len - j, t.c_str());
                candidate_list.push_back(t);
            }
        }
    }
#ifndef  NODEBUG
    std::sort(candidate_list.begin(), candidate_list.end(), [](const std::string &s1, const std::string &s2) { return s1.length() > s2.length(); });
#else
    if (candidate_list.size() < 100) { // to satisfy defined cases
        std::sort(candidate_list.begin(), candidate_list.end(),
                  [](const std::string &s1, const std::string &s2) { return s1.length() > s2.length(); });
    } else {// random test is too long to sort, just select the longest
        auto longest_it = candidate_list.begin();
        std::iter_swap(longest_it,
                       std::max_element(
                               candidate_list.begin(),
                               candidate_list.end(),
                               [](const auto &s1, const auto &s2) { return s1.length() > s2.length(); }));
    }
#endif
    DEBUG_MESSAGE("should be ========> %s", candidate_list.empty() ? "(none)" : candidate_list[0].c_str());
    return candidate_list;
}

std::string find_palindrome_longest(const std::string &text) {
    enum flags_e {
        FLAG_IN_PALINDROME_SEQ = 1,
        FLAG_IN_REPEATING_SEQ = 1 << 1,
    };

    struct InclusiveRange {
        uint32_t flags;
        size_t start_pos;
        size_t end_pos;
        InclusiveRange(size_t l, size_t r) : flags(0), start_pos(l), end_pos(r) {}
    };

    std::list<InclusiveRange> ranges;

    size_t longest_start = 0;
    size_t longest_end = 0;

    auto update_longest = [&](size_t l, size_t r) {
        DEBUG_MESSAGE("trying [%zu,%zu]:%s", l, r, text.substr(l, r - l + 1).c_str());
        if (r - l > longest_end - longest_start) {
            longest_start = l;
            longest_end = r;
        }
    };


    uint32_t flags = 0;
    for (size_t i = 1; i < text.length(); ++i) {
        // process palindrome ranges
        if (!ranges.empty()) {
            for (auto it = ranges.rbegin(); it != ranges.rend();) {
                if ((it->flags & FLAG_IN_REPEATING_SEQ)) {
                    if (text[i] == text[it->start_pos]) {// repeating sequence growing
                        it->end_pos = i;
                        ++it;
                        continue;
                    } else { // repeating sequence broken
                        it->flags &= ~FLAG_IN_REPEATING_SEQ;
                    }
                }
                if (it->start_pos > 0 && text[i] == text[it->start_pos - 1]) {
                    --it->start_pos;
                    it->end_pos = i;
                    ++it;
                } else {
                    update_longest(it->start_pos, it->end_pos);
                    it = decltype(it)(ranges.erase((++it).base()));
                }
            }
        }
        // new center point found
        if (((i > 0 && text[i] == text[i - 1]) || (i > 1 && text[i] == text[i - 2]))) {// scanning pos starting palindrome sequence
            ranges.emplace_back(i - 1, i);
            if (text[i] == text[i - 1]) {
                ranges.back().flags |= FLAG_IN_REPEATING_SEQ;
            } else {
                --ranges.back().start_pos;
            }
        }
    }
    while (!ranges.empty()) {
        update_longest(ranges.back().start_pos, ranges.back().end_pos);
        ranges.pop_back();
    }
    if (longest_start == longest_end) {
        return "";
    } else {
        DEBUG_MESSAGE("[%zu,%zu]:%s", longest_start, longest_end, text.substr(longest_start, longest_end - longest_start + 1).c_str());
        return text.substr(longest_start, longest_end - longest_start + 1);
    }
}

std::string get_random_str() {

    static const char chars[] = {RANDOM_STR_CHARSET};
    static std::mt19937 rd_ng(std::random_device{}());
    static std::uniform_int_distribution<size_t> len_generator(2, RANDOM_STR_MAXLEN);
    static std::uniform_int_distribution<size_t> idx_generator(0, sizeof(chars) - 2);
    size_t len = len_generator(rd_ng);
    std::string s;
    s.reserve(len + 1);
    for (size_t i = 0; i < len; ++i) {
        size_t idx = idx_generator(rd_ng);
        s.replace(i, 1, 1, chars[idx]);
    }
    return s;
}

TEST_CASE("palindrome") {

    SECTION("defined cases") {
        REQUIRE(is_palindrome(std::string("aba")));
        REQUIRE(is_palindrome(std::string("abba")));
        REQUIRE(is_palindrome(std::string("fff")));
        REQUIRE(is_palindrome(std::string("ffff")));
        REQUIRE_FALSE(is_palindrome("ab"));
        REQUIRE_FALSE(is_palindrome("abc"));
        REQUIRE_FALSE(is_palindrome("abcb"));
        REQUIRE(dump(std::vector<std::string>{"aaa", "bbb"}) == R"_(["aaa","bbb"])_");
        REQUIRE(dump(find_palindrome_simple("abcdcba")) == R"_(["abcdcba","bcdcb","cdc"])_"); // full text
        REQUIRE(dump(find_palindrome_simple("faba")) == R"_(["aba"])_");// partial text
        REQUIRE(dump(find_palindrome_simple("ffabba")) == R"_(["abba","ff","bb"])_");// multi, short front
        REQUIRE(dump(find_palindrome_simple("abaff")) == R"_(["aba","ff"])_");// multi, short back
        REQUIRE(dump(find_palindrome_simple("abbaCANACaQa")) == R"_(["aCANACa","CANAC","abba","ANA","aQa","bb"])_");
        REQUIRE(find_palindrome_longest("1q1p1qq2q22q131pp221133p22pq121qp3qq1132q3p233p3113q21pq2q33pq1q") == "pq121qp");
        REQUIRE(find_palindrome_longest("3p31qq1ppqpqpp31232q1pp1pp") == "ppqpqpp");
        dump(find_palindrome_simple("2232pppqpqpqp1qqppq1qpppq1q3111qp3q12p23p2"));
        REQUIRE(find_palindrome_longest("qpqpqp1qqppq1qpppq1q3111qp3q12p23p2") == "q1qpppq1q");
        REQUIRE(find_palindrome_longest("abcdefg33") == "33");
        REQUIRE(find_palindrome_longest("123456p1p") == "p1p");
        REQUIRE(find_palindrome_longest("123456pppp") == "pppp");
        REQUIRE(find_palindrome_longest("1p1p1p12p") == "1p1p1p1");
        REQUIRE(find_palindrome_longest("12345").empty());
    }
#if 1
    SECTION("random cases") {
        int count = RANDOM_CASES;
        while (count--) {
            auto ts = get_random_str();
            DEBUG_MESSAGE("case: %s", ts.c_str());

            DYNAMIC_SECTION("case " << RANDOM_CASES - count) {
                std::vector<std::string> results;
                SECTION("simple") {
                    results = find_palindrome_simple(ts);
                }

                std::string longest_found;
                SECTION("longest") {
                    longest_found = find_palindrome_longest(ts);
                }

                SECTION("compare") {
                    if (!results.empty()) {
                        REQUIRE(is_palindrome(longest_found));
                        REQUIRE(longest_found.length() == results[0].length());
                    }
                }
            }
        }
    }
#endif
}