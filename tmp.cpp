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
                candidate_list.push_back(t);
            }
        }
    }
    std::sort(candidate_list.begin(), candidate_list.end(), [](const std::string &s1, const std::string &s2) { return s1.length() > s2.length(); });
    return candidate_list;
}
std::vector<std::string> find_palindrome(const std::string &text) {
    return find_palindrome_simple(text);


    size_t scan_start_pos = 0; // where scanning string starts
    size_t cur_palindrome_start_pos = 0; //where current palindrome string starts

    std::vector<std::string> candidate_list;// storage all possible sequences
    for (int i; i < text.length(); ++i) {
        if (i <= scan_start_pos) {
            continue; // skip initial
        }
        if ((i > 0 && text[i] == text[i - 1]) || (i > 1 && text[i] == text[i - 2])// scanning pos starting palindrome
            || text[i] == text[cur_palindrome_start_pos] || text[i] == text[cur_palindrome_start_pos - 1]) {  // aready in a sequence
            if (cur_palindrome_start_pos == scan_start_pos) { // sequence starting
                cur_palindrome_start_pos = (text[i] == text[i - 2]) ? (i - 1) : i; // will be decreased immediately
            }
            if (cur_palindrome_start_pos > scan_start_pos) {
                --cur_palindrome_start_pos;
            } else { // reaches sequence head, currently longest and push_back directly
                goto ADD_SEQ;
            }
        } else {
            if (cur_palindrome_start_pos == scan_start_pos) { // palindrome never found
                continue; // just go ahead
            }
ADD_SEQ:
            // break scanning and start a new sequence
            candidate_list.push_back(text.substr(cur_palindrome_start_pos, i - cur_palindrome_start_pos));
            scan_start_pos = i;
            cur_palindrome_start_pos = i;
        }
    }
    // maybe a last hasn't be added
    if (text[text.length() - 1] == text[cur_palindrome_start_pos]) {
        candidate_list.push_back(text.substr(cur_palindrome_start_pos, text.length()));
    }
    std::sort(candidate_list.begin(), candidate_list.end(), [](const std::string &s1, const std::string &s2) { return s1.length() > s2.length(); });
    for (const auto &s : candidate_list) {
        std::cout << s << std::endl;
    }

    return std::vector<std::string>{};

}

std::string get_random_str() {
    static const char chars[] = {"abcd1234ABCD"};
    static std::mt19937 rd_ng(std::random_device{}());
    static std::uniform_int_distribution<size_t> len_generator(2, 1000);//len in (2,1000)
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
    REQUIRE(is_palindrome(std::string("aba")));
    REQUIRE(is_palindrome(std::string("abba")));
    REQUIRE(is_palindrome(std::string("fff")));
    REQUIRE(is_palindrome(std::string("ffff")));
    REQUIRE_FALSE(is_palindrome("ab"));
    REQUIRE_FALSE(is_palindrome("abc"));
    REQUIRE_FALSE(is_palindrome("abcb"));
    REQUIRE(dump(std::vector<std::string>{"aaa", "bbb"}) == "[\"aaa\",\"bbb\"]");
    REQUIRE(dump(find_palindrome("abcdcba")) == "[\"abcdcba\",\"bcdcb\",\"cdc\"]"); // full text
    REQUIRE(dump(find_palindrome("faba")) == "[\"aba\"]");// partial text
    REQUIRE(dump(find_palindrome("ffabba")) == "[\"abba\",\"ff\",\"bb\"]");// multi, short front
    REQUIRE(dump(find_palindrome("abaff")) == "[\"aba\",\"ff\"]");// multi, short back
    REQUIRE(dump(find_palindrome("abbaCANACaQa")) == "[\"aCANACa\",\"CANAC\",\"abba\",\"ANA\",\"aQa\",\"bb\"]");

    SECTION("random cases") {
        int count = 100;
        while (count--) {
            auto t = get_random_str();
            // REQUIRE(dump(find_palindrome(t)) == dump(find_palindrome_simple(t)));
            // fprintf(stderr,"%s\n",t.c_str());
        }
    }
}