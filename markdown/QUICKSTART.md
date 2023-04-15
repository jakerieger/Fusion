# Quickstart

## Install from source

The Luna interpreter and repl are written in C so you'll need to setup a development environment for C if you don't
already have one.

### Prerequisites

- A C99 supported compiler.
- CMake >= 3.22.1
- `git` command line

### 1. Clone the repository

```shell
$ git clone https://github.com/GymateSoftware/Luna
$ cd ./Luna
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

You can install the Luna toolchain directly by downloading and executing the following script:

```shell
$ wget https://github.com/GymateSoftware/Luna/releases/latest/install.sh | sh install.sh
```

## Run the Luna REPL (LunaR)

Run the Luna REPL by executing the `lunar` command.

```shell
$ luna
┏─────────────────────┓
│     ░L░u░n░a░R░     │
│    The Luna Repl    │
┗─────────────────────┛

Version  : 0.0.1 (dev)
Platform : linux_unknown

[!] Type 'info' for configuration info or 'exit' to quit.

›››
```

You can also pass a script file to the `lunar` command to execute that script file.

```shell
$ lunar <script>
```

## Next steps

Head on over to [Examples](EXAMPLES.md) to see Luna in action or
our [Official Documentation](https://opensource.gymate.io/Luna) to dive deep in to the inner
workings of Luna.