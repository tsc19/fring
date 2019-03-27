# FRing - A Geograph-based P2P Overlay Network for Fast and Robust Broadcast in Blockchain Systems

FRing is a novel p2p overlay network designed for blockchain systems, especially those with high transaction rates. Compared to current network solutions for blockchain systems, FRing improves both message complexity and convergence time for broadcast. Also, FRing provides sufficient robustness for all blockchain systems.

## Dependencies (in APT Names)

- Boost
	- libboost-log-dev
	- libboost-random-dev
- Protobuf
	- libprotobuf-dev
	- libprotobuf10
	- protobuf-compiler

## Design of Bootstrap Packet Format

See `evaluation/proto/bootstrat_message.proto`
