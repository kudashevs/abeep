#!/bin/bash

TESTS_DIR=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")

pushd ${TESTS_DIR} || exit 1

if [ -z "$GITHUB_ACTIONS" ]; then
    echo "Running locally..."
    ( cd .. && make clean && make build )
fi

if [ $? -ne 0 ]; then
    popd
    echo "build failed"
    exit $?
fi

echo ""

../abeep -h | ./verify.sh -t storage/run-help

../abeep -V | ./verify.sh -t storage/run-version

popd

# @note extract helper to test with buffer
