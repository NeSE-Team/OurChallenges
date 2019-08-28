# Flag Teller

This challenge is an FPGA bitstream reversing challenge. `challenge.bin` is provided initially and `challenge.v` is provided later in competition as hint; the other files are secret, mainly `memory.bin`.

This program expect 16 chars and if they are correct, spit out the flag; otherwise it sends "try again" message. I/O uses UART protocol. We did not test on the FPGA chip though we used icestorm to find the verilog code and saw that the code acted exactly as we expect in wrong / correct input.

This challenge is a parody of [WCTF2019 TPM2137](https://github.com/q3k/TPM2137). The original problem is purely warm-up level, aiming for familiarize [icestorm](http://www.clifford.at/icestorm/). However, the challenge in XNUCA qualifier progresses further. The code is complexer and not trivially z3-solvable (though in retrospection the algorithm is still weak - it is linear to input finally). This challenge asks the problem - for a general FPGA binary, what is the effective method of reversing it into human-readable data.

Original problem is only the bitstream file. However my teammates on the problem agree that this challenge is not trivially solvable as of average reversing level of 2019, so the source code is provided as hint (ridiculous to see any reversing challenge provide UNOBFUSCATED SOURCE CODE as hint and remain unsolved) and only `memory.bin` is kepy secret. One of the obvious expected solution, with source code, should be as follows:

1. find out the wires that indexes the memory (the memory is readonly so we expect it to be hard wire; however it will be easier if memory is really array of flipflops)
2. read out the memory and find the content of `memory.bin`
3. with `memory.bin` & `challenge.v`, solve the challenge.

This challenge is one of our collected "nontrivial unsolved challenges" and will be kept for a long time until any of us can reverse it directly (without the `challenge.v`!) - hopefully soon.