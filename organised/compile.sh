#!/bin/bash

ORIGINAL_FILE="test.asm"

if [ ! -f "$ORIGINAL_FILE" ]; then
    echo "Error: $ORIGINAL_FILE does not exist"
    exit 1
fi

cp "$ORIGINAL_FILE" temporary.txt || { echo "Error: Unable to create temporary.txt"; exit 1; }
sleep 1

gcc -w labels.c error.c data.c || { echo "Error: Compilation failed"; exit 1; }

./a.out || { echo "Error: Execution of a.out failed"; exit 1; }

gcc -w -o compile main.c text.c data.c || { echo "Error: Compilation failed"; exit 1; }

./compile temporary.txt || { echo "Error: Execution of compile failed"; exit 1; }

cp temporary.txt "$ORIGINAL_FILE" || { echo "Error: Unable to restore original contents"; exit 1; }

rm temporary.txt
rm a.out compile

echo "Compilation and restoration complete. Unnecessary files removed."
#!/bin/bash

ORIGINAL_FILE="output.mc"
TEMP_FILE="temp.mc"

if [ ! -f "$ORIGINAL_FILE" ]; then
    echo "Error: $ORIGINAL_FILE does not exist"
    exit 1
fi

# Find the line number where <end of data segment...> occurs
data_line=$(grep -n "<end of data segment...>" "$ORIGINAL_FILE" | cut -d: -f1)

# If the data segment marker is not found, exit
if [ -z "$data_line" ]; then
    echo "Error: <end of data segment...> not found"
    exit 1
fi

# Extract the lines after the data segment marker
tail -n +$((data_line + 1)) "$ORIGINAL_FILE" > temp_data.mc

# Remove the lines after the data segment marker from the original file
head -n $data_line "$ORIGINAL_FILE" > temp_text.mc

# Append the extracted data segment, separator line, and the original text segment
cat temp_data.mc >> "$TEMP_FILE"
echo "<END OF TEXT SEGMENT>" >> "$TEMP_FILE"
cat temp_text.mc >> "$TEMP_FILE"

# Replace the original file with the combined content
mv "$TEMP_FILE" "$ORIGINAL_FILE"

# Clean up temporary files
rm temp_data.mc temp_text.mc

echo "Appending complete."
