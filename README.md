# NetVanguard

NetVanguard is an educational, low-level firewall designed for Linux-based systems. It is perfect for developers and enthusiasts who want to learn how to filter specific network packets at the kernel level and how to utilize Netlink sockets.

---

## Table of Contents

- [Overview](#overview)
- [Download](#download)
- [Build and Install](#build-and-install)
- [License](#license)
- [Contributing](#contributing)
- [Copyright](#copyright)

---

## Overview

Building this project generates two main executable files and the core `netvanguard.ko` kernel module. Once the module is loaded into the kernel (detailed in the **Build and Install** section), you can monitor its debug and informational messages using the kernel ring buffer (`dmesg`).

The module leverages the Linux NetFilter subsystem by attaching two hooks: one for pre-routing and one for post-routing. At each hook, NetVanguard extracts packet information, evaluates it against your rules, and applies the corresponding action.

Users can configure rules to **accept**, **block**, or **reject** specific packets. All methods evaluate the source IP, destination IP, and port numbers.

**Current limitations to note:**

* The **reject** method currently only supports ICMP packets.
* The **accept** and **block** methods currently only support the TCP/IP protocol.

The module also includes packet dumping capabilities, allowing you to inspect the source/destination IPs and ports for each rule. If you need to drop specific outbound packets originating from the host, you can easily do so using the post-routing hook.

---

## Download

You can clone the repository directly or download a compressed archive from the **Releases** section. If you choose to clone the project, the `main` branch always contains the latest stable updates corresponding to the most recent release.

---

## Build and Install

To get started, clone or download the project repository.

**Prerequisites:**
Ensure you have the necessary build tools and kernel headers installed. On Debian/Ubuntu-based systems, run:

```bash
sudo apt update 
sudo apt install autoconf automake libtool gcc make build-essential linux-headers-$(uname -r) libnl-3-dev libnl-genl-3-dev

```

### Build Instructions

**1. Verify your branch:**
Navigate to the root directory of the project and check your current branch:

```bash
git branch

```

*(Ensure you are on the `main` branch or a specific release branch like `v0.1.0`)*

**2. Generate the build environment:**
You can use the provided `autogen.sh` script for an automatic build, which will store all executable files in a `build` folder. Alternatively, if you prefer to build it manually, run the following command to generate the configuration files:

```bash
autoreconf -fi

```

**3. Compile the project:**
It is highly recommended to create a dedicated build directory, though you can build directly in the root directory if you prefer.

```bash
# Optional: Create and move into a build directory
mkdir build && cd build

# Configure the build (use ../configure if you are inside the build directory)
./configure 

# Compile the project
make

# Run tests (optional but recommended)
make check 

```

---

## License

This project is licensed under the **GNU General Public License, Version 3** (29 June 2007).

Copyright (C) 2007 Free Software Foundation, Inc. [https://fsf.org/](https://fsf.org/)

Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed.

---

## Contributing

Contributions are highly encouraged! If you would like to contribute or if you find a bug, please check the **Issues** tab first.

**To contribute code:**

1. Fork the project.
2. Create a new branch using the format `<type>/<name>` (e.g., `feature/add-udp-support` or `bugfix/fix-memory-leak`).
3. Commit your changes and push them to your fork.
4. Create a Pull Request.

I will review your PR, and if everything looks good, I will merge it. Good luck!

---

## Copyright

This project utilizes the third-party library `libnl` for Generic Netlink socket communication.

All rights reserved.
