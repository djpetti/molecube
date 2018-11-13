#!/bin/sh

set -e

# Create dummy container.
docker create --name src -v /src alpine:3.4 /bin/true
# Copy source code.
docker cp /molecube src:/src

# Run command in source tree.
docker run --net=host --volumes-from src molecube \
  /bin/sh -c "cd /src/molecube && $@"

# Clean up.
docker rm src
