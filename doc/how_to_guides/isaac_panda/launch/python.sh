#!/bin/bash

# Copyright 2023 PickNik Inc.
# All rights reserved.
#
# Unauthorized copying of this code base via any medium is strictly prohibited.
# Proprietary and confidential.

# Script to call the isaac launch system (see python.sh in the ISAAC_SCRIPT_DIR)

OV_PKG_DIR=$HOME/.local/share/ov/pkg
ISAAC_SCRIPT_DIRS=()
for ISAAC_SCRIPT_DIR in $(ls -d -- $OV_PKG_DIR/isaac_sim-*);
do
    ISAAC_VER=${ISAAC_SCRIPT_DIR//$OV_PKG_DIR\/isaac_sim-/};
    if [[ "$ISAAC_VER" =~ ^(2022.2.0|2022.2.1)$ ]]; then
        ISAAC_SCRIPT_DIRS+=($ISAAC_SCRIPT_DIR)
    fi
done

# Prepend the path to all arguments passed in
CUR_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
NEW_ARGS=""
for arg in "$@"
do
    NEW_ARGS="${NEW_ARGS} ${CUR_SCRIPT_DIR}/${arg}"
done

pushd ${ISAAC_SCRIPT_DIRS[${#ISAAC_SCRIPT_DIRS[@]}-1]}
./python.sh $NEW_ARGS
popd
