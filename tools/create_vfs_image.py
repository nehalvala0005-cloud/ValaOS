#!/usr/bin/env python3
"""Create or verify the disposable ValaOS VFS data image; never overwrite it."""

import os
import sys

SECTOR_SIZE = 512
SECTOR_COUNT = 2048
IMAGE_SIZE = SECTOR_SIZE * SECTOR_COUNT
MARKER = b"VALOSD1\0"


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: create_vfs_image.py PATH")
    path = sys.argv[1]
    try:
        descriptor = os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
    except FileExistsError:
        with open(path, "rb") as image:
            if os.fstat(image.fileno()).st_size != IMAGE_SIZE:
                raise SystemExit("existing image has unexpected size; left unchanged")
            if image.read(len(MARKER)) != MARKER:
                raise SystemExit("existing image lacks ValaOS data marker; left unchanged")
        print(f"Verified existing ValaOS data image: {path} (not modified)")
        return

    try:
        with os.fdopen(descriptor, "wb") as image:
            image.truncate(IMAGE_SIZE)
            image.seek(0)
            image.write(MARKER)
    except Exception:
        # The path was created exclusively by this invocation. Leave a partial
        # file visible for manual inspection instead of deleting user data.
        raise
    print(f"Created blank 1 MiB ValaOS data image: {path}")
    print("Filesystem metadata remains unformatted until the guest 'format' command.")


if __name__ == "__main__":
    main()
