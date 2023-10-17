# OBC Software Repository

In this repository is where all of the OBC Software is housed. This is the final version that will be used on the satellite.
The software runs on the Microchip ATSAMV71Q21B Microcontroller Unit, which is a 32-bit ARM Cortex-M7 core.
We use FreeRTOS to handle the tasking of the MCU.
More information regarding OBC can be found [here](https://gitlab.com/groups/acubesat/obc/-/wikis/home).

There is the `env-tests` branch that contains the test code, to be used in the Environmental Tests campaign.

## Build

After cloning the repo, run the command `conan source .` to clone the needed repositories, which currently are:
- [cross-platform-software](https://gitlab.com/acubesat/obc/cross-platform-software)
- [atsam-component-drivers](https://gitlab.com/acubesat/obc/atsam-component-drivers)
  If cloning `COBS` throws a permission/access error, setup an SSH key in GitHub to fix it.

If you're using CLion, you need to add in CMake options (File -> Settings -> Build, Execution, Deployment -> CMake ->
CMake Options) this `-DCMAKE_TOOLCHAIN_FILE=cmake-build-debug/build/Debug/generators/conan_toolchain.cmake -DCMAKE_CXX_COMPILER="/usr/bin/arm-none-eabi-g++" -DCMAKE_C_COMPILER="/usr/bin/arm-none-eabi-gcc"`.

If you just cmake from cli, just add the same flags in your command.

To be able to build, however, you need to install the required `conan` packages. See the `Conan` section for more info.

### Conan
This repository uses [conan 2.0](https://conan.io/) to manage dependencies.

#### AcubeSAT Conan Packages
Some of the Conan packages ([logger](https://gitlab.com/acubesat/obc/logger) and [ecss-services](https://gitlab.com/acubesat/obc/ecss-services)) are hosted on a private repository, so you
need to either:
- have access to the [repository](https://artifactory.spacedot.gr) (if you're already on GitLab, it's the same
  credentials, and you should login at least once) and add the
  remote to your conan remotes. To do that run the following two commands
  `conan remote add conan https://artifactory.spacedot.gr/artifactory/api/conan/conan` and
  `conan remote login conan $YOUR_USERNAME`, which will prompt you to add your password.
- or, clone the repo on your own, and package it locally use `conan create . --build=missing` in the root of the repo. This way, you don't need to add the remote repository, as conan will add it in local cache.
- or, clone the repo on your own and add it as a submodule in the `lib` folder, and make the necessary CMakeLists.
  txt changes to include it in the build.

To install the necessary packages, you need to follow these steps:
- Make sure you performed one of the `AcubeSAT Conan Packages` sections teps
- Run `conan profile detect --force`: Generates default profile detecting GCC. However, for this project, you need to set up
  the correct architecture. Find where `conan` sets up profiles (probably `~/.conan2/profiles`) and run `cp conan-arm-profile ~/.conan2/profiles` (or another directory if conan2 stores the profiles elsewhere) in this project's folder.
- Then run `conan install . --output-folder=cmake-build-debug --build="*" -u -pr conan-arm-profile`. If you're using CLion and don't see `cmake-build-debug`, you have to `Reload CMake project` to have it generated.
  After you've run `conan install...` you can `Reload CMake project` and build as per usual.
- Make sure you followed the steps under the `Build` section
- If the *Imported target "common" included non-existent path* appears, just delete the `cmake-build-debug` folder and redo the `conan install...` command

<details>
<summary>Getting conan</summary>

You can install [conan](https://conan.io/) following the instructions from
[here](https://docs.conan.io/2/installation.html).:
</details>


## Implemented Software

Prototypes of ECSS Services

- ST[01]
- ST[03]
- ST[17] (Not a task, since there isn't a need of periodic update. However, the MCU responds to TC[17,1] and TC[17,3])
- ST[20]

Peripherals:

- Internal MCU Temperature Sensor
- External (MCP9808) Temperature Sensor
- UART with DMA
- Parameter updating of ST[20]

## Note

The internal watchdog has been disabled for debugging reasons.

To re-enable remove the line

```cpp
WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk;
```

in `initialization.c`. It's on line 148 at the time of writing. You should also uncomment `WDT_Initialize()` below that.
