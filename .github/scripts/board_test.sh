#!/bin/bash

Patch=$(curl "https://api.github.com/repos/P-R-O-C-H-Y/arduino-esp32/pulls/${{github.event.number}}/files" | jq -r '.[] | select(.filename == "boards.txt") | .patch ')

substring_patch=$(echo "$Patch" | grep -o '@@[^@]*@@')

echo "Step 1:"

params_array=()
boards_array=()

IFS=$'\n' read -d '' -ra params <<< $(echo "$substring_patch" | grep -oE '[-+][0-9]+,[0-9]+')

echo "Step 1.1:"

for param in "${params[@]}"
do
echo "The parameter is $param"
params_array+=("$param")
done

echo "Step 2:"
previous_board=""
file="boards.txt"

for (( c=0; c<${#params_array[@]}; c+=2 ))
do
    deletion_line=$( echo "${params_array[c]}" | cut -d'-' -f2 | cut -d',' -f1 )
    deletion_count=$( echo "${params_array[c]}" | cut -d',' -f2 | cut -d' ' -f1 )
    addition_line=$( echo "${params_array[c+1]}" | cut -d'+' -f2 | cut -d',' -f1 )
    addition_count=$( echo "${params_array[c+1]}" | cut -d'+' -f2 | cut -d',' -f2 | cut -d' ' -f1 )
    
    addition_end=$(($addition_line+$addition_count-$deletion_count))
    
    echo $deletion_count
    
    echo $addition_line
    
    echo $addition_count

    i=0
    
    echo "Step 3:"

    while read -r line
    do
    i=$((i+1))
    if [ $i -lt $addition_line ]
    then
    continue
    elif [ $i -gt $addition_end ]
    then
    break
    fi
    board_name=$(echo "$line" | cut -d '.' -f1 | cut -d '#' -f1)
    if [ "$board_name" != "" ]
    then
        if [ "$board_name" != "$previous_board" ]
        then
            boards_array+=("$board_name")
            previous_board="$board_name"
            echo "Added '$board_name' to array"
        fi
    fi
    done < "$file"
done