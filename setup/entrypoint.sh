#!/bin/bash
set -euo pipefail

# Run the requested command (default to interactive bash)
if [ "$#" -gt 0 ]; then
    "$@"
else
    /bin/bash
fi

# Keep container alive even after interactive sessions end
exec tail -f /dev/null
