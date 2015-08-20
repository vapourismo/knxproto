# knxclient
A library which provides the means to communicate with several KNX-related devices or services.

## Installation
First off, you need a compatible C standard library and the `libev` library. These might
come with your distribution of choice. So far, the library has been tested against `glibc` only.

Obtain the source and navigate to it's location. Run

    $ make

to build the library, followed by

    # make PREFIX=/path/to/prefix install

to install it. You can omit the `PREFIX` variable, it defaults to `/usr/local`.

## Contributing
Since I have a very limited amount of KNX hardware to test against, I highly encourage you to test
this library against the devices you own. Furthermore am I interested in how the software performs
on different operating systems and also different C standard libraries.

## Documentation
A Doxygen documentation can be generated.

    $ make docs

## Development
This library is still in pre-release state, you should expect the interface to change without
notice.
