# offlinebackup

## Description

This program is meant to backup smaller online storage to larger offline
storage. I use it to backup a 5TB drive to an offline 8TB disk. It uses
the remaining 3TB to store older files that have been deleted from the 5TB
drive.

Files are copied to the backup and optionally verified. Files that
exist on offline storage but don't exist online are preserved until the offline
disk becomes full. When the offline disk becomes full, the oldest files are
deleted to make room for newer files.

This allows me to store the most recent 5TB of data will storing the next older 3TB
offline without me having to manage it manually.

## Building

If you have a modern linux system, you should be able to just type make. It has no
special dependencies. It shouldn't be hard to build for other unix systems.

It doesn't use mmap (mainly so errors are easier to detect) so should be relatively
easy for non-unix systems as well.
```bash
make
```

## Quick start
To run this for the first time, try the following commands. This won't write anything to disk.
```bash
./offlinebackup --help
./offlinebackup --srcdest . /tmp --dryrun
```

## Usage


Usage: offlinebackup (OPTIONS)

### --srcdest A B
Copy files from directory A to B.

If you provide multiple --srcdest arguments, then PLEASE make sure no A's or
B's overlap in the filesystem. Also ensure that all B directories are on the
same partition so free space calculations make sense.

### --interactive
Prompt user before deleting or copying

### --not-interactive
disable --interactive (default)

### --verbose
Print progress to stdout

### --not-verbose
disable --verbose (default)

### --memstats
Print memory stats to stdout

### --not-memstats
disable --memstats (default)

### --verifycopy
Re-read files after writing them to verify contents

### --not-verifycopy
disable --verifycopy (default)

### --verifymatch
Compare existing destination files to source files

### --not-verifymatch
disable --verifymatch (default)

### --mismatchquit
Quit out if existing files don't verify, requires/enables --verifymatch (default)

### --not-mismatchquit
disable --mismatchquit

### --dryrun
Go through the motions but don't delete or copy

### --not-dryrun
disable --dryrun (default)

### --copyfiles
Copy files from source to destination (default)

### --not-copyfiles
disable --copyfiles

### --deletemismatch
Delete destination files that don't match source (default)

### --not-deletemismatch
disable --deletemistmatch

### --deletetofree
Delete oldest destination files to make space on device (default)

### --not-deletetofree
disable -deletetofree

### --allow0match
Allow 0-byte files on source to match destination files (default)

### --not-allow0match
disable --allow0match

### --max6months
Truncate source files older than 183 days to 0 bytes

All --maxXXX options also enable --allow0match --verifymatch

### --max1year
Truncate source files older than 366 days to 0 bytes

All --maxXXX options also enable --allow0match --verifymatch

### --max18months
Truncate source files older than 548 days to 0 bytes

All --maxXXX options also enable --allow0match --verifymatch

### --max2years
Truncate source files older than 731 days to 0 bytes 

All --maxXXX options also enable --allow0match --verifymatch

### --nomaxage
Do not truncate source files (default)

### --nomargin
Attempt copying even if it looks like we'll run out of space

### --verify\_mode
Shortcut for --not-copyfiles --verbose --verifymatch --not-deletemismatch --allow0match

### --fullbackup\_mode
Shortcut for --verbose --verifycopy --verifymatch --allow0match --max6months
