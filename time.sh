#!/bin/bash

# Check if a command is provided
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 [--verbose] command [args...]"
    exit 1
fi

# Check for --verbose flag
verbose=0
if [ "$1" == "--verbose" ]; then
    verbose=1
    shift
fi

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 [--verbose] command [args...]"
    exit 1
fi

# Record start time in ms
start_time=$(date +%s%3N)
start_readable=$(date)
if [ "$verbose" -eq 1 ]; then
    echo -e "\033[0;35m-Started at: $start_readable\033[0m"
fi

# Run the command and capture its exit status
"$@"
exit_status=$?

# Record end time in ms
end_time=$(date +%s%3N)
end_readable=$(date)
elapsed=$(( end_time - start_time ))

if [ "$verbose" -eq 1 ]; then
    echo -e "\033[0;35m-Finished at: $end_readable\033[0m"
fi
echo -e "\033[0;35m-Elapsed time: ${elapsed} milliseconds for the command: $*\033[0m"

exit $exit_status