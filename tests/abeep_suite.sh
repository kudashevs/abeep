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

../abeep -i | ./verify.sh -t storage/run-info

../abeep -V | ./verify.sh -t storage/run-version

popd

