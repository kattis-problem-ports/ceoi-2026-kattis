#!/usr/bin/env bash
# The problem is interactive: there is no answer to compute. gen.sh still wants a
# reference solution to run over every .in file, so this one consumes the input and
# writes nothing; generator.sh then fills the .ans files. See generator.sh.
cat > /dev/null
