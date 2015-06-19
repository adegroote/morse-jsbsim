Morse-JSBSim
============

This repository contains a work-in-progress component to make Morse interacts
with the JSBSim flight dynamics model, using High-level architecture (HLA).

Dependencies
------------

- JSBSim
- cmake
- CERTI
- py-hla (from CERTI applications)
- morse-hla


Building
--------

```
cd cpp
mkdir build
cd build
cmake ..
make
```

Running a simulation
--------------------

In one console, start ``rtig``
In another console:

```
morse run jsbsim
```

In last console, 
```
export JSBSIM_MORSE_HOME="${MORSE_JSBSIM_REPO}/jsbsim/"
cd ${MORSE_JSBSIM_REPO}/cpp/build
./jsbsim_node
```



Press enter in the second console when all simulators are ready.

Status
------

Not really working yet, see TODO file
