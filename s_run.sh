#!/bin/bash
langs="c|py|java|php"
if [[ ! $1 =~ ^(|$langs)$ ]] || [[ ! $2 =~ ^(|$langs)$ ]] || [[ ! $3 =~ ^(|--system)$ ]]; then
  echo "Usage: ./s_run.sh [c|py|java|php] [c|py|java|php] [--system]"
  echo "arg1 - server implementation (leaving empty = 'c')"
  echo "arg2 - client implementation (leaving empty = 'c')"
  echo "arg3 - use system bus (leaving empty = use session bus)"
  exit 1
fi

manual=0
source s_bus.sh start $3

if [[ -z $1 ]] || [[ $1 = "c" ]]; then (./gserver $3 &);
elif [[ $1 = "py" ]]; then (python3 dasbus_server.py $3 &);
elif [[ $1 = "php" ]]; then (php -d extension=dbus.so -f pecldbus_server.php -- $3 &);
elif [[ $1 = "java" ]]; then
    java -cp java-server.jar com.exmpl.server.Server $3 &
    sleep 4 # for some reason dbus-java needs a lot of time for inits, so extra delay here
fi

# giving the server some time to initialize
sleep 1

if [[ -z $2 ]] || [[ $2 = "c" ]]; then ./gclient $3;
elif [[ $2 = "py" ]]; then python3 dasbus_client.py $3;
elif [[ $2 = "php" ]]; then php -d extension=dbus.so -f pecldbus_client.php -- $3;
elif [[ $2 = "java" ]]; then java -cp java-client.jar com.exmpl.client.Client $3;
fi

# giving the server some time to shut down (only needed for Java)
if [[ $1 = "java" ]]; then sleep 1; fi

source s_bus.sh stop $3
