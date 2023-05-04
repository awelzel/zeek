#!/bin/bash
#
# Script to batch-delete all untagged images from ECR public repositories.
# First scans for manifest lists (referencing images) and deletes them, then
# deletes all untagged images.
#
# There's max limit of 100 images via batch-delete-image and currently no
# logic to deal with this other than capping at 100 to avoid an error. The
# script can be invoked multiple times and/or regularly in CI, or fixed...
set -eu

if ! command -v aws; then
    echo "missing aws command" >&2
    exit 1
fi

REGISTRY_ID=${REGISTRY_ID:-103243056077}
REPOSITORY_NAME=${REPOSITORY_NAME:-zeek-dev}

if [[ ! "${REGISTRY_ID}" =~ ^[0-9]+$ ]]; then
    echo "non numeric REGISTRY_ID" >&2
    exit 1
fi

# Find all untagged manifest lists with the following media types:
#
#   application/vnd.docker.distribution.manifest.list.v2+json
#   application/vnd.oci.image.index.v1+json

IMAGE_DIGESTS=$(aws ecr-public describe-images \
    --registry-id "${REGISTRY_ID}" \
    --repository-name "${REPOSITORY_NAME}" \
    --query 'imageDetails[?!imageTags && (contains(imageManifestMediaType, `manifest.list.v2`) || contains(imageManifestMediaType, `image.index.v1`))].{imageDigest: imageDigest}[0:100]' \
    --output json)

if echo "$IMAGE_DIGESTS" | grep 'imageDigest'; then
    aws ecr-public batch-delete-image \
        --registry-id "${REGISTRY_ID}" \
        --repository-name "${REPOSITORY_NAME}" \
        --image-ids "${IMAGE_DIGESTS}"
fi

# Now find all untagged manifests that are left and unreferenced.
IMAGE_DIGESTS=$(aws ecr-public describe-images \
    --registry-id "${REGISTRY_ID}" \
    --repository-name "${REPOSITORY_NAME}" \
    --query 'imageDetails[?!imageTags].{imageDigest: imageDigest}[0:100]' \
    --output json)

if echo "$IMAGE_DIGESTS" | grep 'imageDigest'; then
    aws ecr-public batch-delete-image \
        --registry-id "${REGISTRY_ID}" \
        --repository-name "${REPOSITORY_NAME}" \
        --image-ids "${IMAGE_DIGESTS}"
fi
