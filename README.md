# Msgpack for C/C++

It's like JSON but small and fast.


## Overview

MessagePack is an efficient binary serialization format. It lets you exchange data among multiple languages like JSON. But it's faster and smaller. Small integers are encoded into a single byte, and typical short strings require only one extra byte in addition to the strings themselves.


## License

Msgpack is Copyright (C) 2008-2014 FURUHASHI Sadayuki and licensed under the Apache License, Version 2.0 (the "License"). For details see the `COPYING` file in this directory.


## Contributing

The source for msgpack-c is held at [msgpack-c](https://github.com/msgpack/msgpack-c) github.com site.

To report an issue, use the [msgpack-c issue tracker](https://github.com/msgpack/msgpack-c/issues) at github.com.


## Using Msgpack

### Building and Installing

#### Install from git repository

You will need gcc (4.1.0 or higher), autotools.

You can use msgpack-c as a header only library on C++11 and C++03.

You don't need to prepare ruby when you choose C++11 configuration.

For C++11:
```
$ git clone https://github.com/redboltz/msgpack-c/tree/cxx_separate
$ cd msgpack-c
$ ./bootstrap
$ ./configure CXXFLAGS="-std=c++11"

Add msgpack-c/src to your include path.
```

For C++03:
```
$ git clone https://github.com/redboltz/msgpack-c/tree/cxx_separate
$ cd msgpack-c
$ ./bootstrap
$ ./configure

Add msgpack-c/src to your include path.
```

For C:
```
$ git clone https://github.com/redboltz/msgpack-c/tree/cxx_separate
$ cd msgpack-c
$ ./bootstrap
$ ./configure
$ make
$ sudo make install
```
