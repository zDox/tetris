#!/bin/bash

# Check if the number of clients argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_clients>"
    exit 1
fi

# Number of clients to run
num_clients=$1

# Start TetrisServer
xfce4-terminal -e 'bash -c "./build/TetrisServer; /bin/bash"' &

# Start multiple TetrisClient instances
for ((i=1; i<=$num_clients; i++)); do
    xfce4-terminal -e 'bash -c "./build/TetrisClient; /bin/bash"' &
done

# Wait for all background processes to finish
read -p "Press Enter to close the script..."
