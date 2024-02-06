<a href="https://lorebianco.github.io/Bartender_LYSO/">
  <img align="center" src="https://github.com/lorebianco/Bartender_LYSO/actions/workflows/docs.yml/badge.svg" />
</a>

# Bartender_LYSO
The *Bartender_LYSO* project is a simulation of the waveform output from the MPPCs (SiPMs) of the LYSO calorimeter prototype, entirely written in C++ and utilizing the ROOT libraries. It's directly connected to the Geant4-based Monte Carlo simulation of the detector (see [MC_LYSO](https://lorebianco.github.io/MC_LYSO/) for all details), as it simulates the digitizer and, consequently, the readout.

On these simulated data, it will be possible to test waveform analysis algorithms, searching for the best estimators for energy, time, and position, thus extracting a prediction of experimental resolutions.

This is only a very preliminary version, and several improvements are planned for the future; please refer to the [documentation](https://lorebianco.github.io/Bartender_LYSO/) for all info.


To compile and link and generate the executable, please ensure that the development environment for ROOT is configured. If it's not already set up, you can do so by executing the following command:
```
$ . /path/to/RootInstallation/bin/thisroot.sh
```
Afterwards, proceed as follows:
```
$ cd /path/to/Bartender_LYSO     # go to directory which contains Bartender_LYSO
$ mkdir build
$ cd build
$ cmake ..
$ make -j N     # "N" is the number of processes 
```
This sequence of commands will compile and link your code, generating the executable.

