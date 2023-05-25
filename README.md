**An exercise in IPC (interprocess communication) with D-Bus using C, Python, Java and PHP.**

[**Walk through it step-by-step in Google Colab**](https://colab.research.google.com/drive/18zxaGt_iW2wkaimTAUD3fJq94TEAtCjA)[^1]
[^1]: Because why use Colab only for machine learning
when you can use it as a free interactive cloud-hosted Linux environment too.

Uses GDBus from [GLib](https://github.com/GNOME/glib) for C,
[dasbus](https://github.com/rhinstaller/dasbus) for Python,
[dbus-java](https://github.com/hypfvieh/dbus-java) for Java
and [pecl-dbus](https://github.com/derickr/pecl-dbus) for PHP.

The implemented functionality is the same for all 4 languages,
so you can choose only the one you want, or test everything and compare the results.

- [Description](#description)
- [Prerequisites](#prerequisites)
  - [C](#c)
  - [Python](#python)
  - [Java](#java)
  - [PHP](#php)
- [Usage](#usage)
  - [Compiling](#compiling)
  - [Base usage](#base-usage)
  - [Multiple clients](#multiple-clients)
  - [Docker and TCP](#docker-and-tcp)
  - [Backwards compatibility](#backwards-compatibility)

## Description
The server starts with reserving `com.exmpl.v0001` address on the session bus
(or system bus if launched with `--system` arg) and creating object `/factory`
with three methods: `Create`, `Destroy`, `StopService`.

Calling `Create` with one of the four supported strings (`Worker1`, `Worker2`, `Worker3`, `Worker4`)
dynamically creates another object called `/Worker<N>/Instance<C>`, where `<N>` is the worker type
specified and `<C>` is a global counter that starts with 1 and increments every time `Create`
is called again for this worker type.

The behavior of created objects based on their worker type is as follows:
- `Worker1` has 4 input floating-point properties `N1`, `N2`, `N3`, `N4`
and method `Calc` that populates output properties `Summ` and `Diff` with
their sum (`N1+N2+N3+N4`) and difference (`N1-N2-N3-N4`) respectively.
- `Worker2` does the same thing as `Worker1` except with an arbitrary number
of elements contained in array property `NS`.
- `Worker3` has 3 input string properties `S1`, `S2`, `S3` and method `JoinEw`
that joins them into a single string `Joined` elementwise (all 1st characters,
then all 2nd characters, etc), i.e. `ABC`, `abc`, `123` becomes `Aa1Bb2Cc3`
and `eme`, `xp1`, `al` becomes `example1`.
- `Worker4` does the same thing as `Worker4` except with an arbitrary number of
strings contained in array property `SS`.

Calling `Destroy` with an object name removes previously created object,
and calling `StopService` shuts the server down.

The client requests from the server 4 instances of every worker type
and populates their inputs as follows[^2]:
- Workers 1/2:
  - `1000.11`, `100.11`, `10.11`, `1.11` for the 1st instance
  - `2000.11`, `200.11`, `20.11`, `2.11` for the 2nd instance
  - `3000.11`, `300.11`, `30.11`, `3.11` for the 3rd instance
  - `4000.11`, `400.11`, `40.11`, `4.11` for the 4th instance
- Workers 3/4:
  - `eme`, `xp1`, `al` for the 1st instance
  - `eme`, `xp2`, `al` for the 2nd instance
  - `abcde`, `ABCDE`, `12345` for the 3rd instance
  - `pqrst`, `PQRST_tail`, `67890` for the 4th instance

then calls their respective methods and prints the results as `<object_name> <output>`.

Unless called with `--nostop` arg, the client also shuts down the server
with a call to `StopService` at the very end.

The output should look as follows:
```
START
/Worker1/Instance1 1111.44 888.78
/Worker1/Instance2 2222.44 1777.78
/Worker1/Instance3 3333.44 2666.78
/Worker1/Instance4 4444.44 3555.78
/Worker2/Instance1 1111.44 888.78
/Worker2/Instance2 2222.44 1777.78
/Worker2/Instance3 3333.44 2666.78
/Worker2/Instance4 4444.44 3555.78
/Worker3/Instance1 example1
/Worker3/Instance2 example2
/Worker3/Instance3 aA1bB2cC3dD4eE5
/Worker3/Instance4 pP6qQ7rR8sS9tT0_tail
/Worker4/Instance1 example1
/Worker4/Instance2 example2
/Worker4/Instance3 aA1bB2cC3dD4eE5
/Worker4/Instance4 pP6qQ7rR8sS9tT0_tail
STOP
```
where `START` and `STOP` lines are printed by the server, and the rest is printed by the client.

Overall, the described structure could be useful as a skeleton if you have a bunch of "calculators"
that transform some inputs into some outputs, and you need to serve them to multiple clients
simultaneously (so that every client can work with its own set of inputs). The code is written in a way
that should be easily expandable from 4 to 20 or 50 (or even, say, 200) worker types.

[^2]: These are, of course, purely test-oriented inputs; the exercise is more about creating working
examples for different environments than coming up with plausible real-world scenario.

## Prerequisites
If you're only interested in a specific language, feel free to skip the other parts.

### C
```
sudo apt install make gcc libglib2.0-dev  # Debian family
sudo dnf install make gcc glib2-devel     # RHEL family
sudo pacman -S make gcc pkgconf glib2     # Arch family
```

### Python
```
sudo apt install python3 python3-pip python3-gi       # Debian family
sudo dnf install python3 python3-pip python3-gobject  # RHEL family
sudo pacman -S python python-pip python-gobject       # Arch family
```
```
pip install -U pip
pip install dasbus
```
Alternatively, please refer to dasbus library
[installation instructions](https://dasbus.readthedocs.io/en/latest/#installation).

### Java
```
sudo apt install openjdk-11-jdk-headless maven  # Debian family
sudo dnf install java-11-openjdk-devel maven    # RHEL family
sudo pacman -S jdk11-openjdk maven              # Arch family
```
Any newer JDK version (as well as non-headless variants) should work too.

Alternatively, you can manually install the most recent version of Maven
as instructed on its [website](https://maven.apache.org/install.html).

### PHP
Installing PHP:
```
sudo apt install php-dev php-cli  # Debian family
sudo dnf install php php-cli      # RHEL family
sudo pacman -S php                # Arch family
```
Building pecl-dbus as per [instructions](https://github.com/derickr/pecl-dbus/blob/master/README.rst#building):
```
sudo apt install libdbus-1-dev  # Debian family
sudo dnf install dbus-libs      # RHEL family
sudo pacman -S dbus             # Arch family
```
```
cd <some_tmp_dir> && git clone https://github.com/derickr/pecl-dbus
cd <some_tmp_dir>/pecl-dbus && phpize && ./configure && make && make install
```
Alternatively, you can also use the included prebuilt version 
`php-ext/dbus.so` by copying it to where PHP expects extensions
(can be found using `php -i | grep extension_dir`), but this
would only work for the same version it was built with (PHP 7.4).

## Usage
Everything below assumes that you have cloned this repo (or downloaded its files manually)
and navigated to the top directory, e.g. like this:
```
git clone https://github.com/nsndp/dbus-example
cd dbus-example
```
You might also need to give executable permissions to every shell script inside,
either manually or like this:
```
find . -type f -iname "*.sh" -exec chmod -v +x {} \;
```

### Compiling
For C, you can use prebuilt executables `gserver` and `gclient` or overwrite them with
freshly compiled versions (with one caveat, see [below](#backwards-compatibility)) using:
```
make -C gdbus_server
make -C gdbus_client
```

For Java, the following commands will build `jar` files inside their respective directories
and then place their copies named `java-server.jar` and `java-client.jar` to the top
(so we can conveniently have everything at the same level):
```
cd java-server && mvn package && cd ..
cd java-client && mvn package && cd ..
cp java-server/target/java-server-1.0-SNAPSHOT.jar java-server.jar
cp java-client/target/java-client-1.0-SNAPSHOT.jar java-client.jar
```

Since Python and PHP are interpreted languages, no preparation is necessary for them,
and their scripts are ready to be run as-is.

Alternatively, you can use the provided `s_build.sh` script that packages the same commands like this:
```
./s_build.sh all               # build everything
./s_build.sh c                 # or build only C server + client
./s_build.sh java client-only  # or build only Java client
./s_build.sh clean             # remove all results and intermediate files
```

### Base usage
First, choose an appropriate command to launch an instance of the server:
```
./gserver                                         # C
python3 dasbus_server.py                          # Python
java -cp java-server.jar com.exmpl.server.Server  # Java
php -d extension=dbus.so -f pecldbus_server.php   # PHP
```
Since the server will need to keep running, you can either:
- run the command as-is, leave the current terminal window open,
and start another terminal window for the client (works for a GUI environment);
- run the command with `&` appended to send it to the background
(but still keep it tied to the current terminal and see its output printed);
- run the command with `nohup` prepended and `&` appended to detach it from the current terminal
(then use `ps` to keep track of it and `pkill` to manually stop it if necessary).

Second, choose a command to launch an instance of the client:
```
./gclient                                         # C
python3 dasbus_client.py                          # Python
java -cp java-client.jar com.exmpl.client.Client  # Java
php -d extension=dbus.so -f pecldbus_client.php   # PHP
```

By default, both work with D-Bus session bus. To use the system bus instead,
enable the necessary permissions by copying `com.exmpl.v0001.conf` to
`/usr/share/dbus-1/system.d`[^3], then use the `--system` flag at launch:
```
./gserver --system &
./gclient --system
```
[^3]: Might be `/etc/dbus-1/system.d` instead for older D-Bus versions.
For more on system permissions see [here](https://dbus.freedesktop.org/doc/dbus-daemon).

If you're running a container or some very minimal Linux setup, you might also need
to launch D-Bus service manually. For session bus, that would be:
```
export DBUS_SESSION_BUS_ADDRESS=$(dbus-daemon --session --fork --print-address)
```
Or if you have `dbus-x11` package installed:
```
export $(dbus-launch)
```
For system bus, the process is more distro-specific,
but often the following 2 lines should be enough[^4]:
```
mkdir -p /var/run/dbus
dbus-daemon --system
```
[^4]: See script `s_bus.sh` for more info on manually starting system bus.

Alternatively, you can use the provided `s_run.sh` script to automate all of the above like this:
```
./s_run.sh               # run C server and C client (default)
./s_run.sh c c           # same as the above
./s_run.sh py py         # run Python server and Python client
./s_run.sh java php      # run Java server and PHP client
./s_run.sh php java      # run PHP server and Java client
./s_run.sh c c --system  # run C server and C client on system bus
```
You can launch any combination of server/client implementations on either bus
and confirm that it prints the same results.

### Multiple clients
The script named `s_run_multi.sh` does the same thing as `s_run.sh`, except you can specify
multiple client instances (separated by `+` sign) that will be launched in parallel and
interact with the same server simultaneously:
```
./s_run_multi.sh c c+c+py+php
```
The output should look like:
```
START
[CLIENT 1] /Worker1/Instance1 1111.44 888.78
[CLIENT 4] /Worker1/Instance2 1111.44 888.78
[CLIENT 1] /Worker1/Instance3 2222.44 1777.78
[CLIENT 4] /Worker1/Instance4 2222.44 1777.78
[CLIENT 1] /Worker1/Instance5 3333.44 2666.78
[CLIENT 3] /Worker1/Instance7 1111.44 888.78
[CLIENT 4] /Worker1/Instance6 3333.44 2666.78
[CLIENT 3] /Worker1/Instance9 2222.44 1777.78
[CLIENT 1] /Worker1/Instance8 4444.44 3555.78
...
[CLIENT 2] /Worker4/Instance13 example1
[CLIENT 2] /Worker4/Instance14 example2
[CLIENT 2] /Worker4/Instance15 aA1bB2cC3dD4eE5
[CLIENT 2] /Worker4/Instance16 pP6qQ7rR8sS9tT0_tail
dbus-daemon killed (pid 44128)
```
where `[CLIENT <1-based index in the provided sequence>] `
is prepended to each instance's output, allowing to confirm that
- all clients receive correct results without interfering with each other;
- dynamical creation of D-Bus objects <WorkerN/InstanceI> works as expected;
- implementation in C is the fastest, usually followed by PHP, then Python, then Java.

### Docker and TCP
File `docker/docker-compose.yml` defines 6 containers (or services,
to use Docker Compose terminology), 2 for building and 4 for running:
| Service name | Container name | Image size | Main packages / libraries installed
| --- | --- | --- | ---
| dbus-c-dev | DBUS_C_DEV | 241 MB | gcc, make, libc-dev, glib-dev
| dbus-java-dev | DBUS_JAVA_DEV | 276 MB | openjdk11-jdk, maven
| dbus-c | DBUS_C | 18 MB | glib
| dbus-py | DBUS_PY | 89 MB | python3, py-pip, py3-gobject3, dasbus
| dbus-java | DBUS_JAVA | 183 MB | openjdk11-jre-headless
| dbus-php | DBUS_PHP | 114 MB | php7-dev, php-cli

Once you have Docker Engine + Docker Compose plugin
[installed](https://docs.docker.com/engine/install/)
and navigated to `<where you cloned this repo>/docker` dir, you can:

1. Build server+client in C (this should give the same result as running `./s_build.sh c` locally):
```
docker-compose up -d --build dbus-c-dev
docker exec DBUS_C_DEV ./s_build.sh c
docker cp DBUS_C_DEV:home/gserver ../
docker cp DBUS_C_DEV:home/gclient ../
docker-compose down
```
2. Build server+client in Java (this should give the same result as running `./s_build.sh java` locally):
```
docker-compose up -d --build dbus-java-dev
docker exec DBUS_JAVA_DEV ./s_build.sh java
docker cp DBUS_JAVA_DEV:home/java-server.jar ../
docker cp DBUS_JAVA_DEV:home/java-client.jar ../
docker-compose down
```
3. Run server/client pairs inside their respective containers
using the same `s_run.sh` / `s_run_multi.sh` scripts[^5]:
```
docker-compose up -d --build dbus-c dbus-py dbus-java dbus-php
docker exec DBUS_C ./s_run[_multi].sh c c[+c+c...] [--system]
docker exec DBUS_PY ./s_run[_multi].sh py py[+py+py...] [--system]
docker exec DBUS_JAVA ./s_run[_multi].sh java java[+java+java...] [--system]
docker exec DBUS_PHP ./s_run[_multi].sh php php[+php+php...] [--system]
docker-compose down
```
4. Make clients communicate with servers from different containers over TCP, e.g. like:
```
docker-compose up -d --build dbus-c dbus-py dbus-java dbus-php
docker exec DBUS_C    ./start.sh server                     # run session bus server in cont.1
docker exec DBUS_PY   ./start.sh client dbus-c              # make client from cont.2 talk to it
docker exec DBUS_JAVA ./start.sh server --system            # run system bus server in cont.3
docker exec DBUS_PHP  ./start.sh client dbus-java --system  # make client from cont.4 talk to it
...
docker-compose down
```
[^5]: You can also always start an interactive session with any running container using
`docker exec -it <container_name> /bin/bash` or `docker-compose <service_name> /bin/bash`
and then issue the same commands (or just walk around the container in general).

### Backwards compatibility
Note that building C/C++ binaries on Linux have this annoying thing where you immediately
lose any backward compatibility depending on the build machine's version of glibc (can be checked
with `ldd --version`). E.g. if you compile against glibc 2.35, then it won't work on anything below
that, even if the actual sources are entirely compatible with glibc that's 20 versions earlier.
The canonical Linux answer to that is "just tell them to build from the source", but if your software
is closed-source or if you want to distribute workable executables for some other reason, then the
next easiest solution is to set up an outdated virtual environment specifically for building.

The included files `gserver` and `gclient` were built with this in mind against glibc 2.13 inside a
Docker container running Debian 7 (`FROM --platform=amd64 debian/eol:wheezy RUN apt-get update &&
apt-get -y install make gcc libglib2.0-dev`), which should make them runnable on anything from
[2011](https://en.wikipedia.org/wiki/Glibc#Version_history) onwards. For other potential workarounds
see [here](https://stackoverflow.com/questions/2856438/how-can-i-link-to-a-specific-glibc-version).
