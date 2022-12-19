
<h1 align="center">
  <br>
  <img src="./res/mama.ico" alt="LEGO Island Rebuilder" width="35" align="top">
  <b>LEGO Island Rebuilder</b>
  <br>
</h1>

<h4 align="center">A launcher and modification/patching tool for the 1997 video game <a href="https://en.wikipedia.org/wiki/Lego_Island" target="_blank"><i>LEGO Island</i></a></h4>

<p align="center">
  <a href="https://patreon.com/mattkc">
    <img src="https://img.shields.io/badge/patreon-donate-yellow.svg">
  </a>
    <img src="https://img.shields.io/github/stars/itsmattkc/legoislandrebuilder.svg">
  </a>
    <img src="https://img.shields.io/github/actions/workflow/status/itsmattkc/LEGOIslandRebuilder/ci.yml?branch=master">
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

LEGO Island Rebuilder specifically targets Microsoft Visual C++ 6.0 in order to retain compatibility with Windows 95. It is highly recommended to use this compiler at all times, as code written for newer compilers may not be compatible with this version.

Rebuilder can be built with the standard Visual C++ 6.0 IDE. Since the MSVC 6.0 installer is known to have issues on newer versions of Windows, a portable version of MSVC 6.0 is available [here](https://github.com/itsmattkc/MSVC600).

Since the IDE is old and somewhat archaic, you may wish to use a more modern IDE/code editor. This can be done by using the following commands as your build step in the IDE of your choice:

```
# Enter Visual C++ 6.0 build environment
<MSVC600 directory>/VC98/bin/VCVARS32.BAT

# Build the project
msdev Rebuilder.dsw /make
```

(Replace `<MSVC600 directory>` with the directory you installed/cloned MSVC 6.0 to)

Rebuilder can only be built in "Release" mode. This is because it utilizes DLL injection to modify LEGO Island in memory, and with MSVC, "Debug" code is incompatible with "Release" code (which is what LEGO Island was naturally compiled as).

## Usage

By default, Rebuilder opens a graphical interface which can be used to set the patch configuration and to launch the game. However, command line arguments are also supported:

* `-r/--run`: Runs LEGO Island with the currently saved configuration, bypassing Rebuilder's interface entirely. 
* `-h/--help`: Shows a help screen with usage details.

### Linux and macOS

Rebuilder works on non-Windows platforms using Wine. Ensure LEGO Island is installed into the same prefix you're running Rebuilder in, and it should work identically to Windows.

## Contributing

Contributions like code changes, documentation, or findings and research about the game are welcome. The best way of contributing code to Rebuilder is to open a [pull request](https://github.com/itsmattkc/LEGOIslandRebuilder/pulls).
