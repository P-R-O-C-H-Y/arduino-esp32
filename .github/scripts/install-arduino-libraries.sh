#!/bin/bash

export ARDUINO_LIB_PATH="$ARDUINO_USR_PATH/libraries"

echo "List of libraries:"
cd "$ARDUINO_LIB_PATH"
for entry in "$ARDUINO_LIB_PATH"/*
    do
    echo "$entry"
    done
echo ""

if [ ! -d "$ARDUINO_LIB_PATH" ]; then
    echo "Creating Arduino Library folder ..."
    sudo mkdir -p "$ARDUINO_LIB_PATH"
    cd "$ARDUINO_LIB_PATH"

    echo "Cloning Arduino Libraries ..."
        git clone https://github.com/FastLED/FastLED.git FastLED

    echo "Arduino libraries has been installed in '$ARDUINO_LIB_PATH'"
    echo "List of libraries:"
    for entry in "$ARDUINO_LIB_PATH"/*
        do
        echo "$entry"
        done
    echo ""
fi
