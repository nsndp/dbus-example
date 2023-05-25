FROM alpine:3.18
RUN apk add --no-cache dbus bash coreutils grep procps && \
    apk add --no-cache openjdk11-jre-headless
WORKDIR /home
COPY java-server.jar java-client.jar java-tcp/dbus-java-transport-tcp-4.3.0.jar ./

COPY s_bus.sh s_run.sh s_run_multi.sh ./
COPY com.exmpl.v0001.conf /usr/share/dbus-1/system.d/
COPY docker/start.sh ./
COPY docker/session-local.conf docker/system-local.conf /etc/dbus-1/
CMD ["/bin/bash"]
