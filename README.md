[![Travis CI](https://travis-ci.org/vapourismo/knxproto.svg)]
(https://travis-ci.org/vapourismo/knxproto)

## Usage
The simplest way to get started is to clone the repository, generate the documentation and start
reading.

## Installation
Obtain the source and navigate to its location. Run

    $ make

to build the library, followed by

    # make PREFIX=/path/to/prefix install

to install it. You can omit the `PREFIX` variable, it defaults to `/usr/local`.

## Contributing
Since I have a very limited amount of KNX hardware to test against, I highly encourage you to test
this library against the devices you own. Furthermore am I interested in how the software performs
on different operating systems and also different C standard libraries.

## Documentation
A copy based on current state of the `master` branch can be found
[here](http://knxproto.vprsm.de/).

You can also generate it yourself locally.

    $ make docs

## Development
This library is still in pre-release state, you should expect the interface to change without
notice.
