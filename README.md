
<h1 align="center">
  <br>
  <img src="https://raw.githubusercontent.com/itsmattkc/LEGOIslandRebuilder/mfc/res/mama.ico" alt="LEGO Island Rebuilder" width="35" align="top">
  <b>LEGO Island Rebuilder</b>
  <br>
</h1>

<h4 align="center">A launcher and modification/patching tool for the 1997 video game <a href="https://en.wikipedia.org/wiki/Lego_Island" target="_blank"><i>LEGO Island</i></a></h4>

<p align="center">
  <a href="https://patreon.com/mattkc">
    <img src="https://img.shields.io/badge/patreon-donate-yellow.svg">
  </a>
    <img src="https://img.shields.io/github/stars/itsmattkc/legoislandrebuilder.svg">
</p>

<p align="center">
  <a href="https://www.legoisland.org/">Homepage</a> •
  <a href="#info">Info</a> •
  <a href="#download">Download</a> •
  <a href="#building">Building</a> •
  <a href="#usage">Usage</a> •
  <a href="#contributing">Contributing</a>
</p>

<p align="center">
  <img src='./pkg/fade.gif'>
</p>

<h4 align="center"><i>Supports Windows 95 - Windows 11 (Linux and macOS through <a href="https://www.winehq.org/" target="_blank">Wine</a>)</i></h4>

## Info

LEGO Island Rebuilder is a launcher and modification tool for Mindscape's 1997 video game LEGO Island. It is the product of in-depth research done on the game; the bulk of which is documented on the [LEGO Island Wiki](https://www.legoisland.org/).

It includes fixes to numerous bugs in the original game and multiple quality of life patches while making no permanent changes to the game installation. Additionally, Rebuilder acts as a full replacement for the original configuration tool. 

Rebuilder currently supports [all known versions of LEGO Island](https://www.legoisland.org/wiki/index.php/LEGO_Island_Versions).

## Download

The latest binaries are available on the [releases page](https://github.com/itsmattkc/LEGOIslandRebuilder/releases/tag/continuous).

## Building

Rebuilder utilizes the [CMake](https://cmake.org/download/) build system. Ensure that it is installed before proceeding with the compilation.

Compilation is currently broken with build systems other than [Ninja](https://ninja-build.org/), so it must be installed until this is addressed.

In order to retain compatibility with Windows 95, Rebuilder is compiled using MSVC 6.0.* For convenience and compatibility, a portable version of MSVC 6.0 is available [here](https://github.com/itsmattkc/MSVC600). Its command line can be accessed by running `VC98/bin/VCVARS32.BAT` from any normal command prompt. 

Using the MSVC command line:

```bash
# Clone this repository and enter it
$ git clone https://github.com/itsmattkc/LEGOIslandRebuilder

$ cd LEGOIslandRebuilder

# Create build directory and enter it
$ mkdir build

$ cd build

# Generate the build files
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# Build the program
$ ninja
```

<sub>*While using a modern compiler for building is possible, there are no guarantees that Rebuilder will continue to function on older operating systems.</sub>

## Usage

By default, Rebuilder opens a graphical interface which can be used to set the patch configuration and to launch the game. However, command line arguments are also supported:

* `-r/--run`: Runs LEGO Island with the currently saved configuration, bypassing Rebuilder's interface entirely. 
* `-h/--help`: Shows a help screen with usage details.

### Linux and macOS

Rebuilder works on non-Windows platforms using Wine.

It is important to note that mfc42 must be installed before using Wine with Rebuilder. This can be done using `winetricks mfc42`.

After the installation is complete, simply run Rebuilder with Wine using `wine Rebuilder.exe`.

## Contributing

Contributions like code changes, documentation, or findings and research about the game are welcome. The best way of contributing code to Rebuilder is to open a [pull request](https://github.com/itsmattkc/LEGOIslandRebuilder/pulls).
