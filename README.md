# NetVanguard

# NetVanguard Project TODO List

## Phase 1: Environment Setup & Build System (Autotools & Git)
- [ ] Initialize a new Git repository (`git init`).
- [ ] Set up the standard directory structure (`src/kernel/`, `src/user/`, `src/cli/`, `include/`).
- [ ] Write the `configure.ac` file to define the Autotools project.
- [ ] Write the `Makefile.am` files for the root, user-space, and CLI directories.
- [ ] Create a `Kbuild` or `Makefile` specific to the Linux Kernel Module (LKM) compilation.
- [ ] Successfully run `autoreconf -i`, `./configure`, and `make` to produce dummy binaries.

## Phase 2: Kernel-Space Development (Kernel Module & Netdevices)
- [ ] Write a basic LKM skeleton (`init_module`, `cleanup_module`) and test loading/unloading with `insmod`/`rmmod`.
- [ ] Implement Netfilter hooks (`NF_INET_PRE_ROUTING` or `NF_INET_LOCAL_IN`) to intercept incoming IPv4 traffic.
- [ ] Write a packet parser function inside the module to extract Ethernet, IP, and TCP/UDP headers from the `sk_buff` struct.
- [ ] Implement a basic filtering or marking logic (e.g., identify specific traffic signatures or drop specific ports).
- [ ] Ensure kernel logs (`printk`) correctly output intercepted packet statistics (use `dmesg` to verify).

## Phase 3: User-Space to Kernel-Space IPC (Netlink Sockets)
- [ ] Define a custom Netlink protocol family header file shared between `src/kernel/` and `src/user/`.
- [ ] In the Kernel Module: Implement Netlink socket creation to receive rules from user-space (e.g., "block port 80" or "mark packets with payload X").
- [ ] In the User-Space Daemon: Write a C function using standard socket programming (`AF_NETLINK`) to send configuration structures to the kernel module.
- [ ] Verify two-way communication: User-space sends a rule, kernel acknowledges, kernel sends packet statistics back to user-space.

## Phase 4: User-Space Daemon (Multi-Threading & IPC)
- [ ] Implement the main daemon loop in C.
- [ ] Create a dedicated worker thread using `pthread_create` to continuously listen for asynchronous statistics/events from the kernel via Netlink.
- [ ] Implement a Thread-Safe data structure (using `pthread_mutex_t`) to store the current active rules and traffic statistics.
- [ ] Implement a UNIX Domain Socket server (`AF_UNIX`) within the daemon to listen for local commands.

## Phase 5: The CLI Tool
- [ ] Write a lightweight CLI application in C (`src/cli/`).
- [ ] Implement IPC: The CLI connects to the daemon's UNIX Domain Socket.
- [ ] Add commands to the CLI:
  - `netvanguard add-rule <ip/port>`
  - `netvanguard stats` (fetches packet counts from the daemon, which got them from the kernel).
- [ ] Ensure proper input validation and memory management (check for leaks).

## Phase 6: Testing, Benchmarking & DevOps
- [ ] Write a shell script to automate the loading of the kernel module and starting the daemon.
- [ ] Set up a virtual network namespace (`ip netns`) or use two VMs for isolated testing.
- [ ] Use `iperf3` to generate heavy TCP/UDP traffic and measure the latency/throughput impact of your kernel module.
- [ ] Use `tcpreplay` to send specific pcap files through your interface to test the DPI/filtering logic.
- [ ] **Bonus (Jenkins):** Create a `Jenkinsfile` that checks out the Git repository, runs Autotools (`./configure && make`), and runs a basic unit test.
- [ ] **Bonus (Yocto/Embedded):** Write a simple BitBake recipe (`.bb`) to cross-compile your user-space daemon for an ARM architecture (like a Raspberry Pi or generic ARM Cortex).

## Phase 7: Documentation
- [ ] Write a comprehensive `README.md` in English.
- [ ] Detail the architecture (User-Space vs. Kernel-Space).
- [ ] Provide clear build instructions using Autotools.
- [ ] Provide examples of how to run the CLI and the traffic generator benchmarks.
