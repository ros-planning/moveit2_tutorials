#!/bin/bash -eu

export MOVEIT_BRANCH=main

have_loop() {
  for arg in "$@"; do
    if [[ "${arg}" == "loop" ]]; then
      return 0
    fi
  done
  return 1
}

have_noinstall() {
  for arg in "$@"; do
    if [[ "${arg}" == "noinstall" ]]; then
      return 0
    fi
  done
  return 1
}

if [[ "$0" != "${BASH_SOURCE}" ]]; then
  {
    echo "This file is meant to be executed, not 'source'd:"
    echo
    echo "    ./${BASH_SOURCE}"
  } >&2
  return 1
fi

################################################################################
# Begin Main Script
################################################################################

# Install dependencies, unless argument says to skip
if ! have_noinstall "$@"; then
  sudo apt-get install -y doxygen graphviz
  pip3 install --user --upgrade -r requirements.txt
fi

# A fresh build is required because changes to some components such as css files does not rebuilt currently
# See issue https://github.com/sphinx-doc/sphinx/issues/2090
# rm -rf build

# Build
make local-with-api

# Run
xdg-open ./build/html/index.html &

# Optional build loop
if have_loop "$@"; then
  while inotifywait -re modify,move,create,delete .; do
    rm -rf build
    make html
  done
fi
