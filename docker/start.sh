#!/bin/bash

# a helper script for containers that does things similar to s_run.sh except starting
# servers and clients separately and adding TCP support based on this: https://stackoverflow.com/a/13275973

services="dbus-c|dbus-py|dbus-java|dbus-php"
if [[ ! $1 = "server" ]] && [[ ! $1 = "client" ]] || [[ ! $3 =~ ^(|--system)$ ]]; then
  echo "Usage: ./start.sh [server|client] [local|<service_name>] [--system]"
  exit 1
fi

dsn=$(printenv DOCKER_SERVICE_NAME)

if [[ $1 = "server" ]]; then
    if [[ ! $2 = "--system" ]]; then
        export DBUS_SESSION_BUS_ADDRESS=$(dbus-daemon --session --fork --print-address)
		printenv DBUS_SESSION_BUS_ADDRESS > cur_local_bus_addr.txt
    else
		dbus-daemon --system --fork
    fi
	if [[ $dsn = "dbus-c" ]]; then (nohup ./gserver $2 &);
	elif [[ $dsn = "dbus-py" ]]; then (nohup python3 dasbus_server.py $2 &);
	elif [[ $dsn = "dbus-php" ]]; then (nohup php -d extension=dbus.so -f pecldbus_server.php -- $2 &);
	elif [[ $dsn = "dbus-java" ]]; then (nohup java -cp java-server.jar com.exmpl.server.Server $2 &); fi
fi

if [[ $1 = "client" ]]; then
	if [[ ! $3 = "--system" ]]; then
		args="--session --nostop"
		if [[ ! $2 = "local" ]]; then export DBUS_SESSION_BUS_ADDRESS="tcp:host=${2},bind=*,port=55556,family=ipv4";
		else export DBUS_SESSION_BUS_ADDRESS=$(cat cur_local_bus_addr.txt); fi
	else
		args="--system --nostop"
		if [[ ! $2 = "local" ]]; then export DBUS_SYSTEM_BUS_ADDRESS="tcp:host=${2},bind=*,port=55557,family=ipv4"; fi
	fi
	if [[ $dsn = "dbus-c" ]]; then ./gclient $args;
	elif [[ $dsn = "dbus-py" ]]; then python3 dasbus_client.py $args;
	elif [[ $dsn = "dbus-php" ]]; then php -d extension=dbus.so -f pecldbus_client.php -- $args;
	elif [[ $dsn = "dbus-java" ]]; then
		# for dbus-java to do TCP, we also need to have transport .jar placed somewhere and added to classpath
		java -cp "java-client.jar:dbus-java-transport-tcp-4.3.0.jar" com.exmpl.client.Client $args
	fi
fi
