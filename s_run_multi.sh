#!/bin/bash

# an extended variation of s_run.sh that can launch multiple clients

langs="c|py|java|php"
regex_serv="^(|$langs)$"
regex_clients="^(|($langs)(\+($langs))*)$"

if [[ ! $1 =~ $regex_serv ]] || [[ ! $2 =~ $regex_clients ]] || [[ ! $3 =~ ^(|--system)$ ]]; then
  echo "Usage: ./s_run_mult.sh [c|py|java|php] [c|py|java|php]+ [--system]"
  echo "arg1 - server implementation (leaving empty = 'c')"
  echo "arg2 - client implementation (leaving empty = 'c')*"
  echo "arg3 - use system bus (leaving empty = use session bus)"
  echo "*can be repeated multiple times (joined by +)"
  echo "e.g. 'c+c+py+py' will run 2 c and 2 python clients in parallel"
  exit 1
fi

source s_bus.sh start $3

if [[ -z $1 ]] || [[ $1 = "c" ]]; then
    ./gserver $3 &
elif [[ $1 = "py" ]]; then
    python3 dasbus_server.py $3 &
elif [[ $1 = "php" ]]; then
    php -d extension=dbus.so -f pecldbus_server.php -- $3 &
elif [[ $1 = "java" ]]; then
    java -cp java-server.jar com.exmpl.server.Server $3 &
    sleep 4
fi

pidserv=$!     # "$!"" return PID of the most recently launched background process
mkdir -p tmp   # temporary folder to store clients' outputs (will delete at the end of this script)
sleep 1
if [[ -z $3 ]]; then args="--session --nostop"; else args="--system --nostop"; fi
if [[ -z $2 ]]; then clients="c"; else clients=$2; fi
let i=0

for cl in ${clients//+/ }; do
    # we will launch every client in background mode, then wait for them all to finish
    # we will also prepend "[CLIENT <index>] " to each client's print to differentiate between them
    # - first by redirecting its output to a temp file, then by using "tail" to monitor changes in
    # said file (this is needed so that lines are still printed the moment they appear)
    k=$((i+1))
    log="tmp/client${k}.log"
    echo -n "" > $log
    if [[ $cl = "c" ]]; then
        stdbuf -oL ./gclient $args >> $log &
    elif [[ $cl = "py" ]]; then
        python3 -u dasbus_client.py $args >> $log &
    elif [[ $cl = "php" ]]; then
        php -d extension=dbus.so -f pecldbus_client.php -- $args >> $log &
    elif [[ $cl = "java" ]]; then
        stdbuf -oL java -cp java-client.jar com.exmpl.client.Client $args >> $log &
    fi
    pids[$i]=$!
    let i++
    tail -n0 -f -s0 $log | sed "s/^/[CLIENT ${i}] /" &
done

# waiting for every client to finish
for pid in ${pids[*]}; do wait $pid; done

# since --nostop args meant no client made a call to StopService,
# the server is still running, so we stop it manually
kill $pidserv
source s_bus.sh stop $3
pkill tail
rm -rf tmp