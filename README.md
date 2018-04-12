# agl-service-low-collector

An AGL binding used to collect system/board data. Data collection is based on
`collectd` tool/plugin.

**UNDER DEVELOPMENT - this binding is not fully functional, it's a proof of concept for now**

## Setup

```bash
git clone --recursive https://github.com/iotbzh/agl-server-low-collector
cd agl-server-low-collector
```

## Build  for AGL

```bash
#setup your build environement
. /xdt/sdk/environment-setup-aarch64-agl-linux
#build your application
./conf.d/autobuild/agl/autobuild package
```

## Build for 'native' Linux distros (Fedora, openSUSE, Debian, Ubuntu, ...)

```bash
./conf.d/autobuild/linux/autobuild package
```

You can also use binary package from OBS: [opensuse.org/LinuxAutomotive][opensuse.org/LinuxAutomotive]

## Test

### Native setup

Here are commands used to setup some bindings in order to test on `low-collector`
natively on a Linux host:

```bash
cd $ROOT_DIR/agl-service-low-collector
./conf.d/autobuild/linux/autobuild build
afb-daemon --port=5678 --workdir=./build/package --ldpaths=lib --roothttp=htdocs -vv

```

## Deploy

### AGL

TBD
