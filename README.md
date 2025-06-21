# Maho Retrofit – LinuxCNC Config (Mesa 7i94T + 7i77 + 7i84, QtDragon GUI)

This repository contains the LinuxCNC 2.9 configuration for a retrofitted Maho CNC milling machine.

## Key features

- **Mesa 7i94T (Ethernet)**, **7i77 (servo control)**, and **7i84 (digital I/O via Smart Serial)**
- **QtDragon GUI** for touchscreen-friendly operation
- **Trivial kinematics (trivkins)** for a 3-axis Cartesian machine
- **Incremental encoders** on X, Y, and Z axes with axis-specific scaling
- **Servo thread period**: 1 ms

> Detailed wiring and I/O assignments (e.g. relay mappings, E-stop chain, CNC ready logic)  
> are documented inline in the HAL files where they are implemented.

## Getting started

### Prerequisites

- [Nox](https://nox.thea.codes/en/stable/) for builds and tests
- [Ceedling](https://www.throwtheswitch.org/ceedling#get-ceedling-section) for the test framework
- [gitlint](https://jorisroovers.com/gitlint/latest/) for linting git commit messages
- (Optional) **CLion** for development with CMake integration


### Git commit guidelines

This project uses **gitlint** to enforce clean and consistent commit messages.  
Make sure gitlint is installed and used before committing changes:

```shell
pipx install gitlint
gitlint install-hook
```

### Build for CLion

1. Ensure Ceedling is installed.
2. Build the test artifacts:

```shell
nox -s build_test
```

3. Open the project in CLion (CMake configuration is provided).

### Run tests

Run the tests using Nox:

```shell
$ nox -s test
```
