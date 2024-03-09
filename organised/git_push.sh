#!/bin/bash

# Function to sync changes in all files
function sync_all_files() {
    # Execute git commands for all files
    git add .
    git commit -m "$commit_message"
    git push origin main

    # Check if the push was successful
    if [ $? -ne 0 ]; then
        echo "Error: Push failed"
    fi
}

# Ask the user if they want to sync changes in all files
read -p "Do you want to sync changes in all files? (y/n): " sync_all

# Check the user's response
if [ "$sync_all" == "y" ]; then
    # Ask for confirmation
    read -p "Press 'y' to confirm and any other key to cancel: " confirm
    if [ "$confirm" != "y" ]; then
        echo "Operation canceled by the user"
        exit 0
    fi

    # Ask the user to enter the commit message
    read -p "Enter the commit message: " commit_message

    # Call function to sync changes in all files
    sync_all_files
else
    # Ask the user to enter the filename
    read -p "Enter the filename for which you want to push changes: " filename

    # Check if the file exists and has been modified
    if git status --porcelain "$filename" | grep -q "^ M"; then
        # Ask the user to enter the commit message
        read -p "Enter the commit message: " commit_message

        # Execute git commands for the specified file
        git add "$filename"
        git commit -m "$commit_message"
        git push origin main

        # Check if the push was successful
        if [ $? -ne 0 ]; then
            echo "Error: Push failed"
        fi
    elif [ -f "$filename" ]; then
        echo "Error: The file '$filename' exists but has not been modified."
    else
        echo "Error: The file '$filename' does not exist."
    fi
fi
echo "-------------------------"
