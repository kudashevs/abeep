#!/bin/bash

TESTS_DIR=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")

pushd ${TESTS_DIR} >/dev/null || exit 1

if [ -z "$GITHUB_ACTIONS" ]; then
    echo "Running locally..."
    ( cd .. && make clean && make build )
fi

if [ $? -ne 0 ]; then
    echo "build failed"
    popd
    exit $?
fi

echo ""

stdbuf -oL -eL ../abeep -d null -f 440 -r 44100 -l 40 2>&1 | ./verify.sh -t storage/null-options

stdbuf -oL -eL ../abeep -d null -f 440 -r 44100 -l 40 -v 2>&1 | ./verify.sh -t storage/null-options-verbose

stdbuf -oL -eL ../abeep -d null -i 2>&1 | ./verify.sh -t storage/null-info

stdbuf -oL -eL ../abeep -d null -i -v 2>&1 | ./verify.sh -t storage/null-info-verbose

popd >/dev/null

