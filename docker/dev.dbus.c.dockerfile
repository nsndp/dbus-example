FROM alpine:3.18
RUN apk add --no-cache bash gcc make libc-dev glib-dev
WORKDIR /home
ADD gdbus_client gdbus_client
ADD gdbus_server gdbus_server
COPY s_build.sh .
CMD ["/bin/bash"]
