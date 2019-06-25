# MagiskManager CLI

This tool for control Magisk modules, useful for recovering system.

## Build tools

You need have installed compiler for architecture of your device

## Building:

### for arm64:

```bash
CXX=aarch64-linux-gnu-g++ cmake -Bbuild
make -Cbuild
```

### for arm:

```bash
CXX=arm-none-eabi-g++ cmake -Bbuild
make -Cbuild
```

Copy `build/bin/mm` to Android, then set perrmisions to execute `chmod +x mm` on Android.

## Using

simple start `mm`, and see header:

```
MagiskManager CLI v0.0.1.2
Copyright (C) 2019, DOGINFOG <ftdabcde@gmail.com>
This program comes with ABSOLUTELY NO WARRANTY;
This is free software, and you are welcome to
redistribute it under certain conditions;

?):
```

then enter `?` and press enter, for get help message:

```
?): ?

Commands:
    l                 list modules

    d [id or name]    toggle disable module
    r [id or name]    toggle remove module
    m [id or name]    toggle skip mount module

    h                 this help message
    q                 quit

?):
```

example `l` command:

```
?): l

list modules:
[0] [*] archlinux
[1] [M] AOSPVolumeStepsIncrease100
[2] [*] adb-ndk
[3] [*] busybox-ndk
[4] [*] InitdInjector
[5] [*] riru-core
[6] [*] riru_edxposed_sandhook
[7] [*] safetypatcher
[8] [M] ssh
[9] [*] iYTBP-Vanced-Magisk-Repo

?):
```

first column is ID of module.

second column shows the status of module:

- `[*]` module is enabled
- `[M]` module is enabled with skip_mount
- `[ ]` module is disabled
- `[X]` module will delete at next boot

third column its name of module
