#!/usr/bin/env bash
c=$(realpath $1)
x=/home/froleyks/24_voiraig/build/voiraig
cd $(dirname $0)
echo "testing $c"
printf "\nnew\n"
# time ./aigdd -v -v -v $c new.aag $x > new.log 2>&1
time ./aigdd -r $c new.aag $x
n=$(wc -l new.aag | awk '{print $1}')
echo "new: $n"
# exit 0
printf "\nold\n"
# time ../olddd -v -v -v $c old.aag $x > old.log 2>&1
time ../olddd -r $c old.aag $x
o=$(wc -l old.aag | awk '{print $1}')
echo "old: $o new: $n"
exit $(($n > $o))
