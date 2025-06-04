#!/bin/bash

SRC_FILE="$1"
BASENAME=$(basename "$SRC_FILE" .c)
HEADER_FILE="components/include/${BASENAME}.h"

echo "#ifndef ${BASENAME^^}_H" > "$HEADER_FILE"
echo "#define ${BASENAME^^}_H" >> "$HEADER_FILE"

# extract functions using ctags
ctags --fields=+nKs --output-format=json --kinds-C=fp -o - "$SRC_FILE" |
jq -r '.[] | select(.kind=="function") | .pattern' |
sed -E 's/^\/\^//; s/\$\///; s/\{$/;/' >> "$HEADER_FILE"

echo "#endif" >> "$HEADER_FILE"

