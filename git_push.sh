#!/bin/bash

# Ask the user to enter the commit message
read -p "Enter the commit message: " commit_message

# Execute git commands
git add .
git commit -m "$commit_message"
git push origin main

# Check if the push was successful
if [ $? -eq 0 ]; then
    echo "Push successful"
else
    echo "Error: Push failed"
fi
