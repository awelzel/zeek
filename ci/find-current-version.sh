#!/bin/bash
#
# Helper script to return the current lts or feature version based on
# the branch and tags of Zeek's repository.
#
# * lts is the highest sorting release/x.0 branch which contains a tag
#   of the form form '^v{x}.0.0$'.
#
# * feature is the highest sorting release/x.[^0]+ branch that does
#   that contains a 'v{x}.{y}.0' tag
#
set -euo pipefail -x

REMOTE=${REMOTE:-origin}
MAIN_BRANCH=${MAIN_BRANCH:-refs/remotes/${REMOTE}/master}

function usage() {
    echo "Usage $0 <lts|feature>" >&2
    exit 1
}

if [ $# -ne 1 ]; then
    usage
fi

if [ "${1}" = "lts" ]; then
    PATTERN=".* refs/remotes/${REMOTE}/release/[0-9]+\.0\$"
elif [ "${1}" = "feature" ]; then
    PATTERN=".* refs/remotes/${REMOTE}/release/[0-9]+\.[1-9][0-9]*\$"
else
    usage
fi

# Iterate through all candidate branches, determine if a corresponding
# v{x}.{y}.0 tag exists for that branch. If so, that'll be the most recent
# (highest sorting) branch where we had a release.
for ref in $(git show-ref | grep -E "${PATTERN}" | awk '{ print $2 }' | sort -rn); do
    version=$(echo $ref | sed -E 's,^.*/(.+)$,\1,g')
    tag_ref="refs/tags/v${version}.0"

    # Find the tag using *objname as Zeek is using annotated tags.
    tag_obj=$(git for-each-ref --format='%(*objectname)' "${tag_ref}")

    if [ -z ${tag_obj} ]; then
        continue
    fi

    # We're probably safe, but do verify that the found tag_obj is
    # somewhere between the merge base and the tip of the branch.
    merge_base=$(git merge-base $MAIN_BRANCH $ref)
    if git rev-list ${merge_base}..${ref} | grep -q "^${tag_obj}$"; then
        echo "${version}"
        exit 0
    fi
done

exit 1
