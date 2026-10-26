#!/usr/bin/env bash
# Sets up the Fast Downward build used by labs 3 and 4:
#   1. clones Fast Downward 24.06.1 into ../fast-downward (or the given folder),
#   2. links this folder's planopt_heuristics plugin into its source tree,
#   3. applies course-hooks.patch (registers the plugin and the course's
#      --test-and-or-graphs and --test-projections flags),
#   4. builds the release configuration.
#
# Usage: fast-downward-plugin/setup.sh [target_dir]
# Requirements: git, CMake 3.16+, a C++20 compiler, Python 3 (see Fast Downward's BUILD.md).

set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
target="${1:-$here/../fast-downward}"

if [ ! -d "$target" ]; then
    git clone --depth 1 --branch release-24.06.1 https://github.com/aibasel/downward.git "$target"
    git -C "$target" apply "$here/course-hooks.patch"
    # A symlink keeps the plugin in this repository: edits here are picked up
    # by the next build.
    ln -s "$here/planopt_heuristics" "$target/src/search/planopt_heuristics"
fi

(cd "$target" && ./build.py)
echo "Fast Downward is ready: $target/fast-downward.py"
