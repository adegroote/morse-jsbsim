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
./jsbsim_node --config /tmp/jsbsim_config.json --robot robot
```

Press enter in the Morse console when all simulators are ready.

Status
------

It is mostly working, through some details still need some polishing
