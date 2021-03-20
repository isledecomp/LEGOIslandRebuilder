# LEGO Island Rebuilder

A launcher and mod/patch tool for the 1997 video game *LEGO Island*.

![Screenshot](https://i.imgur.com/vAXy68m.png)

[Homepage](https://www.legoisland.org/rebuilder) | [Download](https://github.com/itsmattkc/LEGOIslandRebuilder/releases/download/continuous/Rebuilder.exe) | [Patreon](https://www.patreon.com/mattkc)

**Supports Windows 98+** (requires [.NET Framework 2.0+](http://web.archive.org/web/20051201020101/http://download.microsoft.com/download/5/6/7/567758a3-759e-473e-bf8f-52154438565a/dotnetfx.exe)).

Rebuilder makes no permanent changes to your *LEGO Island* installation and none of its patches require administrator privileges (it can also patch the parts of *LEGO Island* that require administrator privileges).

Tested with the following versions of LEGO Island ([identify your version](https://www.legoisland.org/wiki/index.php/LEGO_Island_Versions)):
* English v1.0 / August Build
* English v1.1 / September Build
* German v1.1
* Danish v1.1
* Spanish 1.1

Research done towards developing this tool is documented at [the LEGO Island Modding Wiki](http://www.legoisland.org/).

# Usage

Running Rebuilder will open a GUI you can use to select which patches you'd like to run the game with.

To use the music insertion functions, open the Music tab to display a list of the music files in the game. You can then pick and choose which ones you'd like to replace.

The Run button will launch the game with the current settings. The Kill button will close all currently running instances of the game.

Rebuilder also supports command line args:
* `-r`/`--run`: Runs LEGO Island with the currently saved configuration, bypassing Rebuilder's GUI. 
* `-h`/`--help`: Shows a help page.
