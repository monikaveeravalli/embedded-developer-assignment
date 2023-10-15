#!/bin/bash

trap "kill 0" EXIT

NUM_JUNIORS=5  # Specify the number of Junior instances you want to create

echo "Creating virtual Rx/Tx byte streams for 1 Senior and $NUM_JUNIORS Juniors.."

# Start one Senior instance
socat -d -d pty,raw,echo=0 pty,raw,echo=0 2>&1 | grep --line-buffered "N PTY is" | grep --line-buffered -o '[^ ]*$' > socat_output_senior &

# Start multiple Junior instances
for ((i = 1; i <= $NUM_JUNIORS; i++))
do
    socat -d -d pty,raw,echo=0 pty,raw,echo=0 2>&1 | grep --line-buffered "N PTY is" |  grep --line-buffered -o '[^ ]*$' > "socat_output_junior$i" &
done

sleep 2

cat socat_output_senior
TX_SENIOR=$(head -n 1 socat_output_senior)
RX_SENIOR=$(tail -n 1 socat_output_senior)
echo "Senior should send to $TX_SENIOR, listen on $RX_SENIOR"

# Store Tx and Rx pseudoterminals for Junior instances
for ((i = 1; i <= NUM_JUNIORS; i++)); do
    cat "socat_output_junior$i"
    TX_JUNIOR[$i]=$(head -n 1 "socat_output_junior$i")
    RX_JUNIOR[$i]=$(tail -n 1 "socat_output_junior$i")
    echo "Junior $i should send to ${TX_JUNIOR[$i]}, listen on ${RX_JUNIOR[$i]}"
done

wait
