README for Windows Users
========================

The following contains some specific comments for people who have
installed ASCEND on Windows using the binary 'setup' installer. If you have
built you own copy of ASCEND from the source code, comments here may not 
apply to you.

For more information about and support for ASCEND, consult our home page:
http://ascend.cheme.cmu.edu/

ASCEND documentation (such as it is) is available at the above link, or
if you selected to install the Documentation, you should have a link
called "User's Manual" in your Start Menu under ASCEND. At present the
documentation is available only as a PDF file.


Running ASCEND
--------------

There are two different GUIs for ASCEND. Depending on the software that was
available on your machine at the time when you installed ASCEND, you may
have either the Tcl/Tk GUI and the PyGTK GUI, or both of these, or neither.

The newer GUI is the PyGTK GUI. It looks nice and is under active development.
An alternative GUI is the Tcl/Tk GUI which is older but still somewhat more
stable and complete than the new GUI. It is not really under active development
any more, however. You can choose for yourself.

If you've had any problem with getting either of the two GUIs up and running
it may be worth reviewing the prerequisites list on our Wiki at

http://ascendwiki.cheme.cmu.edu/Prerequisites_for_Windows

In some cases, download/install of components may have failed during
installation. One know case where this occurs is for people who are using
their computer from behind an HTTP proxy. Our current installer is not
set up for that use-case, and you should manually install all prerequisites
before attempting to install ASCEND.


Running the PyGTK GUI
---------------------

ASCEND can be launched from the Start menu. If you didn't choose to install
the Start menu shortcuts, you can run ASCEND using

c:\Python25\pythonw.exe c:\Program Files\ASCEND\ascend

The location of 'pythonw.exe' and 'ascend' may be different depending on 
where you installed Python and where you installed ASCEND.

You can also launch the PyGTK GUI for ASCEND by double-clicking on files
with the .a4c or .a4l extension. The installer will associate ASCEND with
those file extensions.

There is not yet a manual for the PyGTK GUI. The 'Help' menu also provides
access to the online manual.


Running the Tcl/Tk GUI
----------------------

The Tcl/Tk GUI can be run from the Start menu.

If you didn't select to install start menu shortcuts, you can launch it
via

c:\Program Files\ASCEND\ascend4.exe

The file location will depend on the folder where you chose to install ASCEND.

There is no file association for .a4c and .a4l files with the Tcl/Tk GUI.

Note that .a4s files are 'scripts' for the Tcl/Tk GUI.

A PDF-file manual has been written for the Tcl/Tk GUI and may be downloaded 
from the ASCEND wiki.


If ASCEND Stops Working
-----------------------

An easy way to break ASCEND is to remove some of the software it depends
on in order to work. This includes uninstalling Python, GTK+, Tcl/Tk,
PyCairo, PyGObject, PyGTK, IPython, Numpy, Matplotlib and possibly others.

The best thing to do in this case is to uninstall ASCEND using the
Add/Remove Programs control panel, then attempting to re-install it. At this
point, the installer should tell you what components you are missing.


Configuration Settings
----------------------

There are configuration settings in the Windows Registry that the ASCEND 
PyGTK GUI uses to determine where its files have been installed.

There are also per-user settings installed in

c:\Documents and Settings\yourusername\Application Data\.ascend.ini

This last file can be edited if you want to add new folders to your
ASCENDLIBRARY path, for example if you are building up a collection of your
own ASCEND models in a directory of their own.

For example:

[Directories]
librarypath = C:\Program Files\ASCEND\models;C:\Program Files\freesteam\ascend

Other settings are also stored in this file. You can delete this file if you
to revert to the default settings that ASCEND used when you first installed
it.

Neither .ascend.ini file nor the Windows Registry are used by the Tcl/Tk
version of ASCEND. Instead, a number of files are stored in the 'ascdata'
folder within  your My Documents folder.


Removing ASCEND from your System
--------------------------------

The uninstaller for ASCEND can be found in the Start menu and also in the
Add/Remove Programs list. In any case, the uninstaller will remove all 
ASCEND components from your system.

NOTE however that when the uninstaller runs, it does NOT remove the
dependencies such as Python, PyGTK, GTK+ etc that may have been downloaded
and installed by the ASCEND installer when you first installed ASCEND.
These additional components will need to be removed by you manually. The
reason that they are not automatically removed is that it is not
possible to determine whether or not you have other programs installed
that also depend on them.

Syntax Highlighting for TextPad
-------------------------------

If you would like to be able to edit ASCEND files using a text edit that 
provides syntax highlighting support, you can use the editor 'TextPad'
available from http://www.textpad.com. If you use TextPad, you can use
the syntax file 'ascend.syn' that is installed in c:\Program Files\ASCEND
by first copying it to your c:\Program Files\TextPad 5\system folder, 
and then creating a new 'Document Class' for .a4c and .a4l files, and
selecting the 'ascend.syn' syntax file.

-- 
John Pye
4 Aug 2007.
