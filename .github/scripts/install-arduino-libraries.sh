#!/bin/bash

export ARDUINO_LIB_PATH="$ARDUINO_USR_PATH/libraries"
if [ ! -d "$ARDUINO_LIB_PATH" ]; then
    echo "$ARDUINO_USR_PATH/libraries"
    echo "$ARDUINO_LIB_PATH"
    cd "$ARDUINO_USR_PATH/libraries"
    echo "Cloning Arduino Libraries ..."
        git clone https://github.com/FastLED/FastLED.git FastLED

    echo "Arduino libraries has been installed in '$ARDUINO_USR_PATH'/libraries"
    echo "List of libraries:"
    for entry in "./"
        do
        echo "$entry"
        done
    echo ""
fi
