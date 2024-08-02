#include "utils.hpp"
using namespace std;
#include <unordered_set>

unordered_set<vector<bool>> seen;
unsigned tests = 0;

string options, bin, path;
vector<int> v;

void write(const vector<bool> &exclude, string_view path) {
  ofstream f(path.data());
  for (auto [e, i] : views::zip(exclude, v))
    if (!e) f << i << "\n";
}

int run(const vector<bool> &exclude) {
  LV(6, exclude);
  write(exclude, "/tmp/dd");
  assert(!seen.contains(exclude));
  seen.insert(exclude);
  tests++;
  return cmd(bin + " " + options + " /tmp/dd >/dev/null 2>&1");
}

[[maybe_unused]] void binExtend_split() {
  const size_t n = v.size();
  vector<bool> exclude(n, false);
  const int expected = run(exclude);
  L2 << "expected exit code" << expected;
  size_t r = 0;
  for (size_t w = n; w > 0; w >>= 1) {
    for (bool shorter : {false, true})
      for (size_t i = 0, j = 1; i < n; j++) {
        const bool extend = j & -j & r;
        const size_t beg = i, end = i + w + extend;
        const bool delay_shorter = extend == shorter;
        const bool successful_sibling =
            (!(j % 2) && exclude[beg - 1]) || ((j % 2) && exclude[end]);
        if (exclude[i] || delay_shorter || successful_sibling)
          i = end; // skip
        else {
          for (; i < end; ++i)
            exclude[i] = true;
          if (run(exclude) == expected)
            write(exclude, path);
          else
            for (i = beg; i < end; ++i)
              exclude[i] = false;
        }
      }
    if (w == 1) break;
    r = (r << 1) | (w & 1u);
  }
  for (size_t i = 0, j = 1; i < n; j++) {
    const bool extend = j & -j & r;
    if (!extend)
      i++;
    else {
      const size_t end = i + 2;
      for (; i < end; ++i) {
        if (exclude[i] || ((i == end - 1) && exclude[i - 1])) continue;
        exclude[i] = true;
        if (run(exclude) == expected)
          write(exclude, path);
        else
          exclude[i] = false;
      }
    }
  }
}

void binExtend() {
  const size_t n = v.size();
  vector<bool> exclude(n, false);
  const int expected = run(exclude);
  L2 << "expected exit code" << expected;
  size_t r = 0;
  for (size_t w = n;; w >>= 1) {
    LV(5, w, r);
    bool skip = false;
    for (int shorter = 0; shorter < 2 - !w; shorter++)
      for (size_t i = 0, j = 1; i < n; j++) {
        const bool extend = (j & -j & r);
        const size_t beg = i, end = i + max(1ul, w + extend);
        const bool delay_shorter = extend == shorter;
        const bool successful_sibling = (w && ((!(j % 2) && exclude[beg - 1]) ||
                                               ((j % 2) && exclude[end]))) ||
                                        (skip && exclude[beg - 1]);
        if (exclude[i] || delay_shorter || successful_sibling)
          i = end; // skip
        else {
          L5 << "exclude" << beg << "to" << (end - 1);
          for (; i < end; ++i)
            exclude[i] = true;
          if (run(exclude) == expected) {
            L5 << "test successful, writing" << path;
            write(exclude, path);
          } else
            for (i = beg; i < end; ++i)
              exclude[i] = false;
        }
        skip = !w && extend && !skip;
        j -= skip;
      }
    if (w > 1) r = (r << 1) | (w & 1u);
    if (!w) break;
  }
}

int main(int argc, char *argv[]) {
  Logging::setLevel(3);
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " <input> <reduced> <bin> [<options>]\n";
    return 0;
  }
  v = readInts(argv[1]);
  path = argv[2];
  bin = argv[3];
  for (int i = 4; i < argc; ++i) {
    options += " ";
    options += argv[i];
  }
  const unsigned original = v.size();
  binExtend();
  const unsigned reduced = readInts(argv[2]).size();
  L1 << "redudced from" << original << "to" << reduced << "lines"
     << "in" << tests << "tests";
  assert(tests <= 2 * original);
}
