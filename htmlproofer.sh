#!/bin/bash
set -e

# Define some config vars
export NOKOGIRI_USE_SYSTEM_LIBRARIES=true
export REPOSITORY_NAME=${PWD##*/}
export MOVEIT_BRANCH=main
echo "Testing branch $MOVEIT_BRANCH of $REPOSITORY_NAME"

# Install htmlpoofer
gem install --user-install html-proofer -v 3.19.4 # newer 4.x requires different cmdline options
PATH="$(ruby -r rubygems -e 'puts Gem.user_dir')/bin:$PATH"

# Install python dependencies
pip3 install --user --upgrade -r requirements.txt

# Clear out any previous builds
rm -rf build

# Build API docs
mkdir -p build/html
pushd build/html
git clone https://github.com/ros-planning/moveit2 -b $MOVEIT_BRANCH --depth 1
pushd moveit2
sed -i "s/HTML_EXTRA_STYLESHEET  =.*/HTML_EXTRA_STYLESHEET  = ..\/..\/..\/theme.css/g" Doxyfile
DOXYGEN_OUTPUT_DIRECTORY="../api" doxygen
popd
rm -rf moveit2
popd

# Test build with non-ROS wrapped Sphinx command to allow warnings and errors to be caught
sphinx-build -W -b html . build/html

# Replace Edit on Github links with local file paths
grep -rl 'https:\/\/github.com\/ros-planning\/moveit2_tutorials\/blob\/main\/' ./build/ | \
 xargs sed -i "s|https://github.com/ros-planning/moveit2_tutorials/blob/main/|file://$PWD|g"

# Replace internal links with local file paths
grep -rl 'https:\/\/moveit.picknik.ai\/rolling\/' ./build/ | \
 xargs sed -i "s|https://moveit.picknik.ai/rolling/|file://$PWD|g"

# Run HTML tests on generated build output to check for 404 errors, etc
# 429 or 403 - happens when GitHub rate-limits requests
htmlproofer ./build \
  --only-4xx --check-html --http-status-ignore "429" --http-status-ignore "403" \
  --file-ignore ./build/html/genindex.html,./build/html/search.html,/html/api/ \
  --alt-ignore '/.*/' --url-ignore '#'

# Tell GitHub Pages (on deploy) to bypass Jekyll processing
touch build/.nojekyll
