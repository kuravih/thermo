#!/bin/bash
# examples:
# ./push.sh -dep spl vision.uml.edu
# ./push.sh -dev kuravih cyclops.uml.edu"

# Function to show usage information
usage() {
    echo "Usage: $0 [-dev | --dev] [-dep | --dep] <username> <hostname>"
    echo "examples:
        ./push.sh -dep kuravih havoc
        ./push.sh -dev kuravih polaris"
    exit 1
}

# Function to push a folder to the remote server
push_folder() {
    local folder_path=$1
    local remote_path=$2
    echo "Pushing $folder_path to $remote_path", using excludes from "$EXCLUDES_FILE"
    
    # Execute rsync command with error handling
    # rsync -avL -e 'ssh -p 2222' --progress --exclude-from="$EXCLUDES_FILE" "$folder_path" "$remote_path"
    rsync -avL --progress --exclude-from="$EXCLUDES_FILE" "$folder_path" "$remote_path"

    if [ $? -eq 0 ]; then
        echo "Push complete for $folder_path"
    else
        echo "An error occurred while pushing $folder_path"
        exit 1
    fi
}

# Default excludes file
EXCLUDES_FILE="../excludes.txt"

# Argument Parsing
while [[ "$1" == -* ]]; do
    case "$1" in
        -dev|--dev|--d)
            EXCLUDES_FILE="../dev_excludes.txt"
            shift
            ;;
        -dep|--dep)
            EXCLUDES_FILE="../dep_excludes.txt"
            shift
            ;;
        *)
            usage
            ;;
    esac
done

# Now there should be exactly 2 arguments left
if [ "$#" -ne 2 ]; then
    usage
fi

# Assigning remaining arguments to variables
USER=$1
HOST=$2

REPO_BASE_PATH="$USER@$HOST:/home/$USER/thermo/"
# ---- camlive --------------------------------------------------------------------------------------------------------
push_folder "." "$REPO_BASE_PATH"
# ---- camlive --------------------------------------------------------------------------------------------------------