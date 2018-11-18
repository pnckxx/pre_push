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
#include <map>


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
                fprintf(stderr, "==> [%zu,%zu]:%s\n", i, len - j, t.c_str());
                candidate_list.push_back(t);
            }
        }
    }
    std::sort(candidate_list.begin(), candidate_list.end(), [](const std::string &s1, const std::string &s2) { return s1.length() > s2.length(); });
    fprintf(stderr, "========> %s\n", candidate_list[0].c_str());
    return candidate_list;
}


std::string find_palindrome_longest(const std::string &text) {
    size_t cur_palindrome_start_pos = 0; //where current palindrome string starts

    size_t longest_start = 0;
    size_t longest_end = 0;

    auto update_longest = [&](size_t l, size_t r) {
        fprintf(stderr, "trying [%zu,%zu]:%s\n", l, r, text.substr(l, r - l + 1).c_str());
        if (r - l > longest_end - longest_start) {
            longest_start = l;
            longest_end = r;
        }
    };

    enum flags_e {
        FLAG_IN_PALINDROME_SEQ = 1,
        FLAG_IN_REPEATING_SEQ = 1 << 1,
    };
    uint32_t flags = 0;
    for (size_t i = 1; i < text.length(); ++i) {
        if (!(flags & FLAG_IN_PALINDROME_SEQ) &&
            ((i > 0 && text[i] == text[i - 1]) || (i > 1 && text[i] == text[i - 2]))) {// scanning pos starting palindrome sequence
            flags |= FLAG_IN_PALINDROME_SEQ;

            cur_palindrome_start_pos = i - 1;
            if (text[i] == text[i - 1]) {
                flags |= FLAG_IN_REPEATING_SEQ;
            } else {
                --cur_palindrome_start_pos;
            }
            update_longest(cur_palindrome_start_pos, i);
        } else if (flags & FLAG_IN_PALINDROME_SEQ) {
            if ((flags & FLAG_IN_REPEATING_SEQ)) {
                if (text[i] == text[cur_palindrome_start_pos]) {// repeating sequence growing
                    update_longest(cur_palindrome_start_pos, i);
                    continue;
                } else { // repeating sequence broken
                    flags &= ~FLAG_IN_REPEATING_SEQ;
                }
            }
            if (cur_palindrome_start_pos > 0) { // no repeating sequence
                if (text[i] == text[cur_palindrome_start_pos - 1]) { // but palindrome sequence growing
                    --cur_palindrome_start_pos;
                    update_longest(cur_palindrome_start_pos, i);
                } else {
                    // palindrome sequence broken assume longer sequence
                    flags &= ~FLAG_IN_PALINDROME_SEQ;
                }
            } else { // assume sequence longer
                flags &= ~FLAG_IN_PALINDROME_SEQ;
            }

        } else { // not in palindrome sequence, not found starting, assume sequence longer and go ahead
            continue;
        }
    }
    fprintf(stderr, "[%zu,%zu]:%s\n", longest_start, longest_end, text.substr(longest_start, longest_end - longest_start + 1).c_str());
    return text.substr(longest_start, longest_end - longest_start + 1);
}

std::string get_random_str() {
    static const char chars[] = {"qp123"};//{"qpxyQPXY12345"};
    static std::mt19937 rd_ng(std::random_device{}());
    static std::uniform_int_distribution<size_t> len_generator(2, 100);//len in (2,1000)
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

TEST_CASE("MAIN") {

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
    }
#if 1
    SECTION("random cases") {
        int count = 100;
        while (count--) {
            auto ts = get_random_str();
            auto results = find_palindrome_simple(ts);
            fprintf(stderr, "case: %s\n", ts.c_str());
            auto longest_found = find_palindrome_longest(ts);
            REQUIRE((is_palindrome(longest_found) && longest_found.length() == results[0].length()));
        }
    }
#endif
}