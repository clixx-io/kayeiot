# KayeIoT (pronounced Coyote) 

![Screenshot](https://github.com/clixx-io/kayeiot/raw/master/doc/images/2018-11-13-windows.PNG)

## KayeIoT is a CAD tool for Component and Cable-Level IoT and Electronics.

* It's like a Cable-Level version of Fritzing that can Autowires connections between parts.

* It uses Fritzing Format Board files so there are thousands of parts that will work instantly.

* Can produce a System diagram of an IoT Project by reading the Arduino Code.

* Has updateable Parts Libraries.

* Will produce a Printable Diagram of a Project within a minute.

* Produces an animation of a project working.

* Platform independent Qt and runs on Windows, Mac and Linux.

* Registered in the Hackaday 2018 Challenge: https://hackaday.io/project/159288-kayeiot-cad-for-hacking-and-iot

Kayeiot is fast and written in C++. It has been written as an Open-Source
tool to make designing and documenting IoT systems easier.

We release KayeIoT under the open-source license GPL2/GPL3.

## How to build it

KayeIoT relies on the Qt Framework, and should work with versions 4 and 5.
**To build KayeIoT, you first have to install the [Qt SDK](http://www.qt.io/download-open-source/)**, and then
easiest path for building it is to use **Qt Creator** (an IDE that comes with the SDK) to open brd2svg.pro. We have built and used brd2svg on both Mac and Windows (though it has been some time since anyone has used the Mac build). It should also work under Linux.

If you prefer the command line, you can also build it like this:

**Mac**

    brew install qt
    qmake -spec macx-g++ kayeiot.pro
    make

**GNU/Linux**

    qmake kayeiot.pro
    make


## Contributing

To report a bug: <https://github.com/clixx-io/kayiot/issues>

We welcome pull requests from the community. Please take a look at the [TODO](https://github.com/trentm/python-markdown2/blob/master/TODO.txt) for opportunities to help this project. For those wishing to submit a pull request to `python-markdown2` please ensure it fulfills the following requirements:

* It must include relevant test coverage.
* Bug fixes must include a regression test that exercises the bug.
* The entire test suite must pass.
* The README and/or docs are updated accordingly.

# Test Suite

This markdown implementation passes a fairly extensive test suite. To run it:

    make test

