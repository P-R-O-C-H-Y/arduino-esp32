#!/bin/bash

export ARDUINO_LIB_PATH="$HOME/Arduino/libraries"
echo "$HOME/Arduino/libraries"
echo "$ARDUINO_LIB_PATH"
cd "$HOME/Arduino/libraries"
echo "Cloning Arduino Libraries ..."
    git clone https://github.com/FastLED/FastLED.git FastLED

echo "Arduino libraries has been installed in '$ARDUINO_LIB_PATH'"
echo "List of libraries:"
cd ..
for dir in libraries/*; do
        echo $dir;
        done
echo ""

