FROM alpine:3.18
RUN apk add --no-cache dbus bash coreutils grep procps && \
    apk add --no-cache python3 py-pip py3-gobject3 && \
    pip install -U pip && pip install dasbus
WORKDIR /home
COPY dasbus_common.py dasbus_server.py dasbus_client.py ./

COPY s_bus.sh s_run.sh s_run_multi.sh ./
COPY com.exmpl.v0001.conf /usr/share/dbus-1/system.d/
COPY docker/start.sh ./
COPY docker/session-local.conf docker/system-local.conf /etc/dbus-1/
CMD ["/bin/bash"]
