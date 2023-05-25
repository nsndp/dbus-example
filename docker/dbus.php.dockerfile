FROM alpine:3.18
RUN apk add --no-cache dbus bash coreutils grep procps && \
    apk add --no-cache php7-dev --repository=https://dl-cdn.alpinelinux.org/alpine/v3.13/community && \
	apk add --no-cache php-cli
WORKDIR /home
COPY pecldbus_server.php pecldbus_client.php php-ext/dbus.so ./
RUN cp dbus.so $(php -i | grep extension_dir | cut -d " " -f 5)

COPY s_bus.sh s_run.sh s_run_multi.sh ./
COPY com.exmpl.v0001.conf /usr/share/dbus-1/system.d/
COPY docker/start.sh ./
COPY docker/session-local.conf docker/system-local.conf /etc/dbus-1/
CMD ["/bin/bash"]
