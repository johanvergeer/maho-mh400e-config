Maho Retrofit - LinuxCNC Config (Mesa 7i94T + 7i77 + 7i84, QtDragon GUI)

This configuration is for a retrofitted Maho CNC milling machine using LinuxCNC 2.9.

Key features:
- Mesa 7i94T (Ethernet), 7i77 (servo control), and 7i84 (digital I/O via Smart Serial)
- QtDragon GUI for touchscreen-friendly operation
- Trivial kinematics (trivkins) for a 3-axis Cartesian setup
- Incremental encoders on X, Y, and Z axes, with axis-specific scaling
- Servo thread period: 1 ms

Note:
- Detailed wiring and I/O assignments (e.g. relay mappings, E-stop chain, CNC ready logic)
  are documented inline in the HAL files where they are implemented.

# To get this running

## Nox

This project uses [Nox](https://nox.thea.codes/en/stable/) to run builds and tests.

Follow the steps on their website to install Nox.

## CLion

- Step 1: Install [Ceedling](https://www.throwtheswitch.org/ceedling#get-ceedling-section)
- Step 2: Run the `test_build` nox command to build the artifacts required for CLion.

```shell
$ nox -s build_test
```

- Step 3: Open CLion using the CMake configuration

## To run the tests

Run the tests using Nox directory once Ceedling is installed.

```shell
$ nox -s test
```
