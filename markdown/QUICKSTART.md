# Quickstart

## Install from source

The Fusion interpreter and repl are written in C so you'll need to setup a development environment for C if you don't
already have one.

### Prerequisites

- A C99 supported compiler.
- CMake >= 3.22.1
- `git` command line

### 1. Clone the repository

```shell
$ git clone https://github.com/GymateSoftware/Fusion
$ cd ./Fusion
```

### 2. Configure and run CMake

```shell
$ mkdir build
$ cd build
$ cmake ..
```

### 3. Build

Still in the build folder, run the following:

```shell
$ make
```

## Install from releases

> NOTE: This method is **not** functional at the moment, please install from source [above](#install-from-source).

You can install the Fusion toolchain directly by downloading and executing the following script:

```shell
$ wget https://github.com/GymateSoftware/Fusion/releases/latest/install.sh | sh install.sh
```

## Run the Fusion REPL (Fuse)

Run the Fusion REPL by executing the `FUSIONr` command.

```shell
$ FUSION
┏─────────────────────┓
│     ░L░u░n░a░R░     │
│    The Fusion Repl    │
┗─────────────────────┛

Version  : 0.0.1 (dev)
Platform : linux_unknown

[!] Type 'info' for configuration info or 'exit' to quit.

›››
```

You can also pass a script file to the `FUSIONr` command to execute that script file.

```shell
$ FUSIONr <script>
```

## Next steps

Head on over to [Examples](EXAMPLES.md) to see Fusion in action or
our [Official Documentation](https://opensource.gymate.io/Fusion) to dive deep in to the inner
workings of Fusion.