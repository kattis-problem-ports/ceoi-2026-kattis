#include "validator.h"

const long long MAX_N = 200000; // 2 * 10^5

// The input format is "d/v" per line, with no spaces, so Int() cannot be used
// directly: Word() would swallow the slash. Parse a line strictly by hand.
// Grammar for each field: 0 | [1-9][0-9]*  (no sign, no leading zeros).
static long long parse_field(const string& line, size_t& i, const string& what,
                             long long lineno) {
    size_t start = i;
    while (i < line.size() && line[i] >= '0' && line[i] <= '9') i++;
    if (i == start)
        die("line " + to_string(lineno) + ": expected a digit for " + what);
    if (line[start] == '0' && i - start != 1)
        die("line " + to_string(lineno) + ": " + what + " has a leading zero");
    if (i - start > 18)
        die("line " + to_string(lineno) + ": " + what + " is far too long");
    long long val = 0;
    for (size_t k = start; k < i; k++) val = val * 10 + (line[k] - '0');
    return val;
}

static void parse_line(const string& line, long long lineno, long long& d, long long& v) {
    size_t i = 0;
    d = parse_field(line, i, "the depth", lineno);
    if (i >= line.size() || line[i] != '/')
        die("line " + to_string(lineno) + ": expected '/' after the depth");
    i++;
    v = parse_field(line, i, "the vertex", lineno);
    if (i != line.size())
        die("line " + to_string(lineno) + ": trailing characters after the vertex");
}

void run() {
    long long maxn = Arg("maxn", MAX_N);
    // Subtask flags. chain=1 is subtask 4 ("the i-th line is i-1/i-2"),
    // incdepth=1 is subtask 5 ("the i-th line is i-1/v").
    bool chain = bool(Arg("chain", 0));
    bool incdepth = bool(Arg("incdepth", 0));

    // The first line is always "0/(n-1)", so it tells us n.
    long long d, v;
    parse_line(Line(), 1, d, v);
    if (d != 0) die("line 1: the first line must have depth 0");
    if (v > maxn - 1)
        die("line 1: vertex " + to_string(v) + " implies n > " + to_string(maxn));
    long long n = v + 1;

    vector<char> seen(n, 0);
    seen[n - 1] = 1;
    long long prev_depth = 0;

    for (long long i = 2; i <= n; i++) {
        parse_line(Line(), i, d, v);

        // In a DFS printout the depth grows by at most one per step, and only
        // the very first line may have depth 0.
        if (d < 1 || d > prev_depth + 1)
            die("line " + to_string(i) + ": depth " + to_string(d) +
                " must be in [1, " + to_string(prev_depth + 1) + "]");
        if (v > n - 1)
            die("line " + to_string(i) + ": vertex " + to_string(v) +
                " is out of range [0, " + to_string(n - 1) + "]");
        if (seen[v]) die("vertex " + to_string(v) + " is printed more than once");
        seen[v] = 1;

        if (incdepth && d != i - 1)
            die("incdepth: line " + to_string(i) + " has depth " + to_string(d) +
                ", expected " + to_string(i - 1));
        if (chain && (d != i - 1 || v != i - 2))
            die("chain: line " + to_string(i) + " is " + to_string(d) + "/" +
                to_string(v) + ", expected " + to_string(i - 1) + "/" + to_string(i - 2));

        prev_depth = d;
    }

    for (long long w = 0; w < n; w++)
        if (!seen[w]) die("vertex " + to_string(w) + " is never printed");
}
