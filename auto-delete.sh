#!/bin/bash

# This script creates a snapshot of the current directory structure and files
TREETMPFILE=".tree.tmp"
DELETED_DIR="delete"

# Change to script directory (or specify the target directory)
cd "$(dirname "$0")"

# If TREETMPFILE doesn't exist, create it with the plain list of files (and directories)
if [ ! -f $TREETMPFILE ]; then
    # Save a flat, absolute file list snapshot (excluding TREETMPFILE)
    tree -afi --noreport . | grep -v "^\./tree\.tmp$" > $TREETMPFILE
    echo -e "\033[0;32m$TREETMPFILE created.\033[0m"
else
    # Ensure the delete directory exists
    mkdir -p "$DELETED_DIR"
    
    # For each file and directory (excluding files under .git folders) in the current directory recursively
    find . -mindepth 1 \( -type f -o -type d \) ! -name "$TREETMPFILE" ! -path "*/.git*" | while read -r item; do
        echo "checking: $item"
        if ! grep -Fxq "$item" $TREETMPFILE; then
            echo -e "\033[0;31m  Moving '$item' to '$DELETED_DIR/'\033[0m"
            mv "$item" "$DELETED_DIR"/
        fi
    done
    # Remove TREETMPFILE after processing
    rm "$TREETMPFILE"
    echo -e "\033[0;31m  $TREETMPFILE removed.\033[0m"
fi