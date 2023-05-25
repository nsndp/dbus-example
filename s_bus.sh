#!/bin/bash

# a helper script used by s_run.sh/s_run_multi.sh that manually starts and stops dbus
# in a distro-independent way (should be useful only for containers, full systems
# usually have both system and session buses autostarted)

if [[ ! $1 = "start" ]] && [[ ! $1 = "stop" ]] || [[ ! $2 =~ ^(|--system)$ ]]; then
  echo "Usage: ./s_bus.sh [start|stop] [--system]"
  exit 1
fi

if [[ $1 = "start" ]]; then

    if [[ $2 = "--system" ]]; then
    
        # copying permissions file to appropriate folder for our service to work
        conf=com.exmpl.v0001.conf
        pdir=/usr/share/dbus-1/system.d
        if [[ ! -d $pdir ]]; then
            # older systems tend to save here instead of /usr/share
            pdir=/etc/dbus-1/system.d
        fi
        if [[ ! -f $pdir/$conf ]]; then
            cp $conf $pdir/
            echo "copied .conf file with permissions to $pdir"
        fi
        
        # extracting path to file descriptor with pid (if there is one) from config
        # so that we can properly clean up after
        sysconf=/usr/share/dbus-1/system.conf
        if [[ ! -f $sysconf ]]; then
            # older systems tend to save here (might even mismatch with
            # system.d dir above - one can be in /etc, but another in /var/share)
            sysconf=/etc/dbus-1/system.conf
        fi
        if [[ -f $sysconf ]]; then
            # getting line with <pidfile>, removing tags, removing leading whitespaces
            pidpath=$(grep "<pidfile>" $sysconf | sed 's/<[^>]*>//g' | sed 's/^[ ]*//g')
        fi
    fi

    # we decide if the session bus is running by checking whether env var is set or not
    # we decide if the system bus is running by checking names of all running processes

    ptrn1="[d]bus-daemon --system"
    # a different launcher that may be used in some distros instead of dbus-daemon
    ptrn2="[d]bus-brocker-launch --scope=system"
    if [[ ! $2 = "--system" ]] && [[ -z $(printenv DBUS_SESSION_BUS_ADDRESS) ]] ||
         [[ $2 = "--system" ]] && [[ -z $(ps -ef | grep -e "${ptrn1}" -e "${ptrn2}") ]]; then
        manual=1
        if [[ ! $2 = "--system" ]]; then
            info=$(dbus-daemon --session --fork --print-pid --print-address)
            # the output should be "address pid" with ' ' or '\n' as separator depending
            # on dbus version, so we account for both and then save lines separately
            addr=$(echo $info | tr ' ' '\n' | sed '1!d')
            dpid=$(echo $info | tr ' ' '\n' | sed '2!d')
            export DBUS_SESSION_BUS_ADDRESS=$addr
            echo "manually started dbus-daemon --session (pid $dpid)"
        else
            # this is needed if pid file is being created
            if [[ ! -z $pidpath ]]; then mkdir -p $(dirname $pidpath); fi
            # this might never be needed but also shouldn't do any harm as an extra check
            if [[ -x /usr/bin/dbus-uuidgen ]]; then dbus-uuidgen --ensure; fi
            dpid=$(dbus-daemon --system --fork --print-pid)
            echo "manually started dbus-daemon --system (pid $dpid)"
            # there are usually ways to do the things above automatically,
            # either with "/etc/init.d/dbus [start|stop]" or "service dbus [start|stop]",
            # but those are distro-specific and may not be available inside containers
        fi
    fi
fi

if [[ $1 = "stop" ]] && [[ $manual = 1 ]]; then
    kill $dpid
    echo "stopped dbus-daemon (pid $dpid)"
    if [[ $2 = "--system" ]] && [[ ! -z $pidpath ]]; then
        rm -f $pidpath
        echo "removed pid file from $pidpath"
    fi
fi
