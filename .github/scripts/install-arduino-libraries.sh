#!/bin/bash

export ARDUINO_LIB_PATH="$HOME/Arduino/libraries"
if [ ! -d "$ARDUINO_LIB_PATH" ]; then
    echo "$HOME/Arduino/libraries"
    echo "$ARDUINO_LIB_PATH"
    cd "$HOME/Arduino/libraries"
    echo "Cloning Arduino Libraries ..."
        git clone https://github.com/FastLED/FastLED.git FastLED

    echo "Arduino libraries has been installed in '$ARDUINO_LIB_PATH'"
    echo "List of libraries:"
    for entry in "./*"
        do
        echo "$entry"
        done
    echo ""
fi
