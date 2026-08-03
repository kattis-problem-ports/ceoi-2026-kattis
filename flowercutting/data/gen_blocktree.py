#!/usr/bin/python3
"""Gardens made of many maximal cliques glued together at shared flowers.

Why this shape is legal.  The statement guarantees the garden is saturated: no two
disconnected flowers may share two or more neighbours.  A *block graph* -- a graph in
which every biconnected component is a clique -- satisfies this automatically:

    Suppose non-adjacent u, v had two common neighbours w1 != w2.  Then u-w1-v-w2-u is a
    4-cycle.  Every cycle of a graph lies inside a single biconnected component, and here
    every such component is a clique, so u and v would be adjacent.  Contradiction.

So gluing cliques together in a tree -- each new clique sharing exactly one flower with
what already exists -- is saturated by construction, with no rejection sampling.  The
maximal cliques are then exactly the blocks, so the answer is

    sum over blocks of  [ C(k,2) - f(k) ],   f(k) = 3k/2 - 2 (k even), 3(k-1)/2 (k odd)

Why this is adversarial.  In every shipped test the maximal cliques are essentially
disjoint (5 to 115 of them even at m ~ 95000), so a solution that finds only one maximal
clique per flower is never caught.  Here hundreds of maximal cliques share cut flowers.

Modes:
  rand / star / chain  -- grow a block tree, attaching each new block to a uniformly
                          random / the oldest / the newest usable flower.  Block sizes are
                          drawn from [kmin, kmax].
  consume              -- deterministic worst case for "one clique per flower": build a
                          hub clique of `hub` fresh flowers, then hang a separate spoke
                          clique of `spoke` fresh flowers off *every* hub flower.  A solver
                          that charges each flower to a single clique loses the whole hub
                          whenever the spokes claim its flowers first.  Spokes are allowed
                          to be smaller than hubs, so this mode takes `hub`/`spoke` rather
                          than `kmin`/`kmax`.
"""

import sys
import random


def cmdlinearg(name, default=None):
    for arg in sys.argv:
        if arg.startswith(name + "="):
            return arg.split("=")[1]
    if default is None:
        print("missing parameter", name)
        sys.exit(1)
    return default


random.seed(int(cmdlinearg('seed', sys.argv[-1])))

n = int(cmdlinearg('n'))
maxdeg = int(cmdlinearg('maxdeg', 0))          # 0 = no degree cap
iso = int(cmdlinearg('iso', 0))                # flowers left with no connections
comps = int(cmdlinearg('comps', 1))            # number of connected gardens
mode = cmdlinearg('mode')

if mode not in ('rand', 'star', 'chain', 'consume'):
    print("unknown mode", mode)
    sys.exit(1)

if mode == 'consume':
    hub = int(cmdlinearg('hub'))
    spoke = int(cmdlinearg('spoke'))
    if hub < 4 or spoke < 3:
        # A hub smaller than 4 prunes to nothing, a spoke smaller than 3 never claims
        # its cut flower, so neither would test anything.
        print("consume needs hub >= 4 and spoke >= 3, got", hub, spoke)
        sys.exit(1)
    if maxdeg and hub - 1 > maxdeg:
        # The hub clique alone would already exceed the degree cap.
        print("hub", hub, "needs degree", hub - 1, "but maxdeg is", maxdeg)
        sys.exit(1)
    smallest = hub
else:
    kmin = int(cmdlinearg('kmin'))
    kmax = int(cmdlinearg('kmax', cmdlinearg('kmin')))
    if kmin < 2 or kmax < kmin:
        print("bad clique size range", kmin, kmax)
        sys.exit(1)
    smallest = kmin

if iso < 0:
    print("negative iso", iso)
    sys.exit(1)
structured = n - iso
if structured < smallest:
    print("not enough flowers to build anything:", structured)
    sys.exit(1)

deg = [0] * n
edges = []
next_fresh = 0


def add_block(block):
    for i in range(len(block)):
        for j in range(i + 1, len(block)):
            edges.append((block[i], block[j]))
            deg[block[i]] += 1
            deg[block[j]] += 1


def take(count):
    """Allocate `count` fresh flowers, or return None if the budget cannot cover them."""
    global next_fresh
    if next_fresh + count > structured:
        return None
    got = list(range(next_fresh, next_fresh + count))
    next_fresh += count
    return got


def fits(v, size):
    return maxdeg == 0 or deg[v] + size - 1 <= maxdeg


if mode == 'consume':
    # Hub cliques whose every flower is also the cut flower of its own spoke clique.
    while True:
        hub_vs = take(hub)
        if hub_vs is None:
            break
        add_block(hub_vs)
        for h in hub_vs:
            if not fits(h, spoke):
                continue
            legs = take(spoke - 1)
            if legs is None:
                break
            add_block([h] + legs)
    # Pad any leftover budget onto an existing flower so that n comes out exact.
    while next_fresh < structured:
        room = structured - next_fresh
        # A pad block of size want+1 adds `want` connections to its host, so the host must
        # have room for all of them -- shrink the block until some host can take it.
        want = min(room, spoke - 1)
        host = None
        while want >= 1:
            host = next((v for v in range(next_fresh) if fits(v, want + 1)), None)
            if host is not None:
                break
            want -= 1
        if host is None:
            print("nowhere to attach the last", room, "flowers under maxdeg =", maxdeg)
            sys.exit(1)
        add_block([host] + take(want))
else:
    placed = []
    for c in range(comps):
        seed_v = take(1)
        if seed_v is None:
            print("not enough flowers for", comps, "components")
            sys.exit(1)
        placed.append(seed_v)

    def attach_point(c, size):
        ok = [v for v in placed[c] if fits(v, size)]
        if not ok:
            return None
        if mode == 'star':
            return ok[0]
        if mode == 'chain':
            return ok[-1]
        return random.choice(ok)

    while next_fresh < structured:
        remaining = structured - next_fresh
        size_hi = min(kmax, remaining + 1)
        if size_hi < 2:
            break
        size = random.randint(min(kmin, size_hi), size_hi)
        order = list(range(comps))
        random.shuffle(order)
        chosen = None
        for c in order:
            v = attach_point(c, size)
            if v is not None:
                chosen = (c, v)
                break
        if chosen is None:
            print("every component is saturated under maxdeg =", maxdeg,
                  "with", remaining, "flowers unplaced")
            sys.exit(1)
        c, v = chosen
        fresh = take(size - 1)
        add_block([v] + fresh)
        placed[c].extend(fresh)

if next_fresh != structured:
    print("placed", next_fresh, "of", structured, "flowers")
    sys.exit(1)
if not edges:
    print("generated no connections")
    sys.exit(1)

# Random relabelling, so nothing depends on the order the blocks were created in.
perm = list(range(n))
random.shuffle(perm)
edges = [(perm[a], perm[b]) for a, b in edges]
edges = [(a, b) if random.randrange(2) else (b, a) for a, b in edges]
random.shuffle(edges)

out = [f"{n} {len(edges)}"]
out += [f"{a + 1} {b + 1}" for a, b in edges]
sys.stdout.write("\n".join(out) + "\n")
