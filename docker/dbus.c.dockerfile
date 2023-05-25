FROM alpine:3.18
RUN apk add --no-cache dbus bash coreutils grep procps && \
    apk add --no-cache gcompat glib
WORKDIR /home
COPY gserver gclient ./

COPY s_bus.sh s_run.sh s_run_multi.sh ./
COPY com.exmpl.v0001.conf /usr/share/dbus-1/system.d/
COPY docker/start.sh ./
COPY docker/session-local.conf docker/system-local.conf /etc/dbus-1/
CMD ["/bin/bash"]

# coreutils is for stdbuf command    (used in s_run_multi.sh script)
# grep is for grep with -oP option   (used in s_run.sh script)
# procps is for pkill with -e option (used in s_run.sh script)

# Line 8 is for system bus to work
# Line 10 is for TCP connections to work
