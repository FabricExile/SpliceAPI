Fabric Splice API
======================
A C/C++ API for creating cross-DCC portable runtimes.

Fabric Splice API allows you to make use of the Fabric Core inside of DCCs, and persist custom runtime to be able to use them across multiple DCC applications.

Repository Status
=================

This repository will be maintained and kept up to date by Fabric Technologies to match the latest Fabric Core.

Supported platforms
===================

To date all three major platforms (windows, linux, osx) are supported, if you build the thirdparty dependencies for the corresponding platform.

Building
========

A scons (http://www.scons.org/) build script is provided. Fabric Splice API depends on
* A static build of boost (http://www.boost.org/), version 1.55 or higher.
* A dynamic build of Fabric Core (matching the latest version).

To inform scons where to find the Fabric Core includes as well as the thirdparty libraries, you need to set the following environment variables:

* FABRIC_CAPI_DIR: Should point to Fabric Engine's Core folder.
* BOOST_DIR: Should point to the boost root folder (containing boost/ (includes) and lib/ for the static libraries).

The temporary files will be built into the *.build* folder, while the structured output files will be placed in the *stage* folder.

License
==========

The license used for this API can be found in the root folder of this repository.
