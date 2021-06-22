# DIAS Firewall

The current project stores the source code of the Statefull Firewall/Intrusion Detection System developed by [Nislab](https://nislab.umfst.ro/) from University of Medicine, Pharmacy, Science and Technology of Targu Mures, as part of the Horizon2020 [DIAS Project](https://dias-project.com/).

## Dependencies
The following dependencies must be installed in order to build the _dias-firewall_:
```
sudo apt install g++ libexpat-dev libconfig-dev
```

## Building the project
The build script _build.sh_ is used to compile the current project. Use:
```
./build.sh core
```
to compile the solution

```
./build.sh tests
```
to compile the test file, and
```
./build.sh clean
```
to remove the _Build_ directory.

Once the core is compiled, the following directory structure will be generated inside the build directory:
```
├── Bin 
│   ├──               // _Binary files_
├── DiasFirewall      // Executable                                                 
└── Lib 
    └── libfwcore.so  // Shared library
```

## Awknowledgement 

This work was funded by the European Union’s Horizon 2020 Re-search and Innovation Programme through [DIAS project](https://dias-project.com/) under Grant Agreement No. 814951. This documentreflects only the author’s view and the Agency is not responsiblefor any use that may be made of the information it contains
