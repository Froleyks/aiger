#!/usr/bin/env bash
c=$(realpath $1)
x=/home/froleyks/24_voiraig/fuzz/voiraig
cd $(dirname $0)
echo "testing $c"
printf "\nnew\n"
time ./aigdd -r $c new.aag $x
n=$(wc -l new.aag | awk '{print $1}')
echo "new: $n"
# exit 0
printf "\nold\n"
time ../olddd -r $c old.aag $x
o=$(wc -l old.aag | awk '{print $1}')
echo "old: $o new: $n"
exit $(($n != $o))
