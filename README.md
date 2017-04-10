# HAT
A C++ platform for home automation.

The original purpose of the project was to provide a platform useful to students and teachers for developing student labs in the area of IoT (Internet of Things) and Home Automation, using low cost hardware such as Beaglebone Back or Raspberry PI. The idea was to provide a library, easy to install on this kind of hosts, supporting the quick development of simple applications in order to introduce students into the IoT domain. The choice was to adopt C++ as basic programming language that is still very performant on low power hosts while providing a powerful object oriented paradigm.

In order to provide an environment highly modular easy to modify and to encourage the student to explore network programming HAT supports an agent based architecture, which stay at the origin of the project name. In fact, HAT is the acronym of “Home Agent Team”.

HAT agents runs as Linux processes distributed on hosts interconnected by a TCP/IP local network and cooperate by exchanging messages encoded according to a restful protocol. Moreover, they provide a self-configuration protocol inspired to zeroconf.

A comprehensive description of HAT architecture ca be found in the project documentation.

Beyond the initial goal, HAT is now grown up and provides not only a development environment but also a set of stable agents, which provide most functionalities, usually required in the home automation domain, which can be used to implment self-made smart utilities, such as anti-intrusion systems, energy monitoring and management, and so on.

Moreover, the agent architecture can be easy extended in order to integrate agent implemented in programming language different from C++, such as Java or Pyton. The interested user may find examples of this kind of solution in several student lab, also made publicly available.
