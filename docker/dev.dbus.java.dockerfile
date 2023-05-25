FROM alpine:3.18
RUN apk add --no-cache bash openjdk11-jdk maven
WORKDIR /home
ADD java-client java-client
ADD java-server java-server
COPY s_build.sh .
CMD ["/bin/bash"]
