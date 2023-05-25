#!/bin/bash
if [[ ! $1 =~ ^(c|java|all|clean)$ ]]; then
    echo "usage: ./s_build.sh [c|java|all|clean] [server-only|client-only]"
    exit 1
fi

if [ $1 = "all" ] || [ $1 = "c" ]; then
    if [[ ! $2 = "client-only" ]]; then make -C gdbus_server; fi
    if [[ ! $2 = "server-only" ]]; then make -C gdbus_client; fi
fi

if [ $1 = "all" ] || [ $1 = "java" ]; then
    if [[ ! $2 = "client-only" ]]; then cd java-server && mvn package && cd ..; fi
    if [[ ! $2 = "server-only" ]]; then cd java-client && mvn package && cd ..; fi
    if [[ ! $2 = "client-only" ]]; then cp java-server/target/java-server-1.0-SNAPSHOT.jar java-server.jar; fi
    if [[ ! $2 = "server-only" ]]; then cp java-client/target/java-client-1.0-SNAPSHOT.jar java-client.jar; fi
fi

if [ $1 = "clean" ]; then
    make clean -C gdbus_server
    make clean -C gdbus_client
    rm -rf __pycache__
    rm -f java-server.jar
    rm -f java-client.jar
    cd java-server && mvn clean && rm -f dependency-reduced-pom.xml && cd ..
    cd java-client && mvn clean && rm -f dependency-reduced-pom.xml && cd ..
fi
