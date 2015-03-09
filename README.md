## Installation
First off, you need a compatible C standard library, the `libevent` and `pthread` library. These might come
with your distribution of choice. So far, the library has been tested against `glibc` only.
It is known to work under Arch Linux and Debian Linux on both `amd64` and `armv6l` architectures.

Obtain the source and navigate to it's location. Run

```
$ make
```

to build the library, followed by

```
# make PREFIX=/path/to/prefix install
```

to install it. You can omit the `PREFIX` variable, it defaults to `/usr/local`.


## Contributing
Since I have a very limited amount of KNX hardware to test against, I highly encourage you to test this
library against the devices you own. Furthermore am I interested in how the software performs on
different operating systems and also different C standard libraries.


## Documentation
So far there is no useful documentation other than the docstrings contained within the header files.
Maybe start with [src/routerclient.h](https://github.com/vapourismo/knxclient/blob/master/src/routerclient.h) or
[src/tunnelclient.h](https://github.com/vapourismo/knxclient/blob/master/src/tunnelclient.h).


## Development
This library is still in pre-release state, you should expect the interface to change without notice.
