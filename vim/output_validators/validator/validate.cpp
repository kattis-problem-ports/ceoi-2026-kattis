#include "validate.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

typedef long long ll;

// What counts as whitespace here MUST match what split_head_and_rest() below skips,
// because next_line() decides whether a line exists and split_head_and_rest() decides
// what is on it: a stricter next_line() would hand back a "non-blank" line that then
// yields no tokens at all. (The cast is required: isspace() on a negative char is
// undefined.)
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

// Splits `line` into its first whitespace-separated token (`head`, the command count) and
// the concatenation of every remaining token (`rest`, the example sequence).
//
// Concatenating rather than requiring one token is deliberate: the statement asks for "the
// required minimum number of commands and an example of such a sequence of commands", and
// says nothing about the sequence being free of whitespace. A contestant who writes
// `10 Y P Y P h P Y P P P` has produced a perfectly good example, and separators cannot
// carry meaning when the alphabet is {h, l, Y, P}. Joining can never let a wrong answer
// through: the result still has to be exactly `count` commands ending at exactly n.
//
// Tokenising in place, rather than through an istringstream, keeps peak memory at about
// one extra copy of the line instead of three. Contestant output is arbitrary, and a
// single line can be enormous.
static void split_head_and_rest(const string& line, string& head, string& rest) {
    size_t i = 0, n = line.size();
    while (i < n && is_space(line[i])) i++;
    size_t s = i;
    while (i < n && !is_space(line[i])) i++;
    head.assign(line, s, i - s);
    rest.clear();
    while (i < n) {
        while (i < n && is_space(line[i])) i++;
        size_t t = i;
        while (i < n && !is_space(line[i])) i++;
        rest.append(line, t, i - t);
    }
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

    string line, head, seq;
    double worst = 1.0;

    for (ll i = 0; i < t; i++) {
        // ---- judge's answer: authoritative minimum command count ----
        if (!next_line(judge_ans, line))
            judge_error("judge answer ran out at test case %lld", i + 1);
        split_head_and_rest(line, head, seq);
        ll jcount;
        if (!parse_ll(head, jcount))
            judge_error("judge answer for test case %lld is malformed", i + 1);
        // Unconditional, including jcount == 0: valid_sequence("", n, 0) is true exactly
        // when n == 1, which is the only length reachable without commands. So this also
        // catches a judge answer of a bare "0" against some n > 1.
        if (!valid_sequence(seq, n[i], jcount))
            judge_error("judge answer for test case %lld is not a valid sequence", i + 1);

        // ---- contestant's answer ----
        if (!next_line(author_out, line)) {
            author_message("Output ended after %lld of %lld test cases", i, t);
            wrong_answer("Too few lines of output");
        }
        split_head_and_rest(line, head, seq);

        // Only the command count can earn a wrong answer. Anything wrong with the example
        // costs half the points, never all of them -- that is what the statement's
        // "no example or an incorrect example" clause promises, and it must include an
        // example we could not make sense of, not just one that simulates incorrectly.
        ll acount;
        if (!parse_ll(head, acount)) {
            author_message("Line %lld does not start with a non-negative integer", i + 1);
            wrong_answer("Malformed command count");
        }
        if (acount != jcount) {
            author_message("Line %lld: wrong minimum number of commands", i + 1);
            wrong_answer("Wrong command count");
        }

        // The count is right. Full credit additionally needs a valid example. An empty
        // `seq` means none was printed, which is valid only for n = 1 (jcount == 0), where
        // the empty command sequence is the right answer and cannot be printed as a token.
        if (!valid_sequence(seq, n[i], acount)) {
            author_message("Line %lld: command count is right but the example sequence is "
                           "missing or invalid", i + 1);
            worst = min(worst, 0.5);
        }
    }

    if (worst < 1.0)
        author_message("The command counts are all correct, but at least one "
                       "example sequence was missing or invalid: half credit.");

    accept_with_score(worst);
}
