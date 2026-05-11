#!/bin/bash

TESTS_DIR=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")

pushd ${TESTS_DIR} || exit 1

( cd .. && make clean && make build )

if [ $? -ne 0 ]; then
    popd
    echo "build failed"
    exit $?
fi

echo ""

stdbuf -oL -eL ../abeep -i 2>&1 | ./verify.sh -t storage/run-info

stdbuf -oL -eL ../abeep -i -v 2>&1 | ./verify.sh -t storage/run-info-verbose

../abeep -V | ./verify.sh -t storage/run-version

popd

