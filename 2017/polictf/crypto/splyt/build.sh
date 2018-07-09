#!/bin/bash

tmpdir=`mktemp -d`

./splyt.py split flag 20 13 > "$tmpdir/shares.json"
diff flag <(./splyt.py join "$tmpdir/shares.json")
if [[ $? -ne 0 ]]
then
    echo "Something's wrong, the reconstructed flag does not match the original one"
    exit -1
fi

mkdir -p challenge
jq ".[1,2,3]" "$tmpdir/shares.json" > challenge/challenge.json
cp -R splyt.py Splyt build.sh challenge

rm -rf "$tmpdir"