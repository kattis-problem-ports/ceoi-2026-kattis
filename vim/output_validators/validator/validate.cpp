#include "validate.h"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

typedef long long ll;

// What counts as whitespace here MUST match what split_tokens() below skips, because
// next_line() decides whether a line exists and split_tokens() decides what is on it.
// istringstream's >> skips every character isspace() accepts -- which includes '\v' and
// '\f' -- so a stricter next_line() would hand back a "non-blank" line that then yields
// zero tokens. (The cast is required: isspace() on a negative char is undefined.)
static bool is_space(char c) {
    return isspace(static_cast<unsigned char>(c)) != 0;
}

// Reads the next line that is not entirely whitespace, with surrounding whitespace
// (including CR) removed.
static bool next_line(istream& in, string& line) {
    while (getline(in, line)) {
        while (!line.empty() && is_space(line.back()))
            line.pop_back();
        size_t s = 0;
        while (s < line.size() && is_space(line[s])) s++;
        if (s == line.size()) continue; // blank line, skip
        line.erase(0, s);
        return true;
    }
    return false;
}

// Splits into at most three whitespace-separated tokens (a third one is an error).
static void split_tokens(const string& line, vector<string>& out) {
    istringstream iss(line);
    string tok;
    while (out.size() < 3 && iss >> tok) out.push_back(tok);
}

static bool parse_ll(const string& s, ll& val) {
    if (s.empty() || s.size() > 18) return false;
    for (char c : s)
        if (c < '0' || c > '9') return false;
    val = 0;
    for (char c : s) val = val * 10 + (c - '0');
    return true;
}

// The text consists entirely of '-' characters, so only lengths matter:
// L = length of the text, p = 1-based cursor position, C = clipboard length.
// Returns true iff `seq` is a valid sequence of exactly `count` commands that
// ends with exactly `n` characters.
static bool valid_sequence(const string& seq, ll n, ll count) {
    if ((ll)seq.size() != count) return false;
    ll L = 1, p = 1, C = 0;
    for (char c : seq) {
        if (c == 'h') {
            if (p > 1) p--;
        } else if (c == 'l') {
            if (p < L) p++;
        } else if (c == 'Y') {
            C = L - p + 1;
        } else if (c == 'P') {
            if (C == 0) continue; // empty clipboard: nothing happens
            // The text only ever grows, so overshooting n can never be undone.
            if (L + C > n) return false;
            L += C;
            p = p + C - 1;
        } else {
            return false; // not one of h, l, Y, P
        }
    }
    return L == n;
}

int main(int argc, char** argv) {
    init_io(argc, argv);

    ll t;
    if (!(judge_in >> t)) judge_error("could not read t from the input file");

    vector<ll> n(t);
    for (ll i = 0; i < t; i++)
        if (!(judge_in >> n[i]))
            judge_error("could not read n for test case %lld", i + 1);

    // Skip to the start of the judge answer's first line.
    string line;
    double worst = 1.0;

    for (ll i = 0; i < t; i++) {
        // ---- judge's answer: authoritative minimum command count ----
        if (!next_line(judge_ans, line))
            judge_error("judge answer ran out at test case %lld", i + 1);
        vector<string> jt;
        split_tokens(line, jt);
        ll jcount;
        if (jt.empty() || !parse_ll(jt[0], jcount))
            judge_error("judge answer for test case %lld is malformed", i + 1);
        if (jcount > 0 && (jt.size() < 2 || !valid_sequence(jt[1], n[i], jcount)))
            judge_error("judge answer for test case %lld is not a valid sequence", i + 1);

        // ---- contestant's answer ----
        if (!next_line(author_out, line)) {
            author_message("Output ended after %lld of %lld test cases", i, t);
            wrong_answer("Too few lines of output");
        }
        vector<string> at;
        split_tokens(line, at);
        if (at.empty()) {
            // Unreachable while next_line() and split_tokens() agree on whitespace, but
            // contestant output is arbitrary: never index at[0] without checking.
            author_message("Line %lld contains no token", i + 1);
            wrong_answer("Malformed line");
        }
        if (at.size() > 2) {
            author_message("Line %lld has more than two tokens; expected the "
                           "command count and optionally one command sequence", i + 1);
            wrong_answer("Malformed line");
        }
        ll acount;
        if (!parse_ll(at[0], acount)) {
            author_message("Line %lld does not start with a non-negative integer", i + 1);
            wrong_answer("Malformed command count");
        }
        if (acount != jcount) {
            author_message("Line %lld: wrong minimum number of commands", i + 1);
            wrong_answer("Wrong command count");
        }

        // The count is right. Full credit additionally needs a valid example.
        bool has_seq = (at.size() == 2);
        bool ok;
        if (jcount == 0) {
            // n = 1: the empty sequence is the correct example, and it cannot
            // be printed as a token, so a bare count is fully correct here.
            ok = !has_seq || valid_sequence(at[1], n[i], 0);
        } else {
            ok = has_seq && valid_sequence(at[1], n[i], acount);
        }
        if (!ok) worst = min(worst, 0.5);
    }

    if (worst < 1.0)
        author_message("The command counts are all correct, but at least one "
                       "example sequence was missing or invalid: half credit.");

    accept_with_score(worst);
}
