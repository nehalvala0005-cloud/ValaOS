# ValaOS persistent storage

The kernel uses primary-master ATA PIO with 28-bit LBA commands. `make run`
attaches the boot ISO as the CD-ROM and `build/ValaOS-data.img` as IDE primary
master. It deliberately does not attach the existing `build/ValaOS.img`.

`make data-disk` creates a 1 MiB (2048 sector) blank data image with an eight
byte `VALOSD1\0` marker in sector zero. The creation script uses exclusive file
creation and refuses to overwrite an existing path. The kernel accepts writes
only to a marked image, rejects sector zero, and bounds I/O to the first 2048
sectors. This is a guard against accidentally writing to the old boot disk; it
is not a general ATA device discovery or hardware identity mechanism.

The filesystem is ValaOS VFS version 2. It uses 512 byte sectors, a header at
LBA 1, 32 fixed 300 byte directory records serialized across LBAs 21 through
39, and a legacy v1 read region at LBAs 2 through 11. Other sectors are unused.
There are at most 31 non-root entries; names are at most 31 bytes and file
contents at most 255 bytes. Files occupy space inside their fixed record, so
the entry limit, rather than a general block allocator, is the capacity limit.
LBA 40 is reserved as the ATA read/write diagnostic scratch sector; the VFS
never stores files there.

Boot validates the marker, header, all used records, strings, parent links and
duplicate names. A blank filesystem stays unmounted until `format` is entered
in the shell. Formatting is allowed only when the VFS header sector is all
zeroes; it refuses mounted filesystems, recognized filesystems, and unknown
nonblank headers. It initializes the default root files. No disk is formatted
automatically. `fsinfo`, `ls`, `cat`, `touch`, and `write` operate on a mounted
filesystem.

Metadata/data writes are ordered with the header last, but VFS v2 has no
journal or redundant metadata copy. Power loss during a multi-sector update
can leave a filesystem that fails validation; the kernel will refuse to mount
it and will not reformat it. The format command does not repair it. The driver
has bounded polling and reports ATA status errors, but its geometry is the
fixed QEMU test-image limit, not IDENTIFY-derived physical-device capacity.

To launch QEMU from WSL, run `cd /mnt/d/ValaOS && make run`. On first use,
`make run` creates the blank marked data image; type `format` once in the guest.
Subsequent launches reuse the same image and preserve files across reboot.
