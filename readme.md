# Wiretap

Wiretap is a simple packet analysis tool (think `tcpdump` but with more limited features) that I developed to interact with the Unix socket interface. It works by binding a packet socket (`AF_PACKET` and `SOCK_RAW`) which accepts all Ethernet/IEEE 802.3 packets (`ETH_P_ALL`), and monitoring all incoming traffic on the socket to generate logs. However, it should be noted that Wiretap also works with wireless network interfaces such as those implementing WiFi/IEEE 802.11, which probably is a consequence of `ETH_P_ALL` including them too. (I find it quite amusing that IEEE 802.11 support is included in the `net/ethernet.h` header of all places.)

## Usage
Use `wiretap <interface>` to run Wiretap with a packet socket bound to `<interface>`. Note that since Wiretap uses raw sockets, elevated privileges such as those granted by `sudo` are required to run the program.

## Build
Wiretap may be built by running `make wiretap`. Please note, however, that the program depends on `linux/if_packet.h` meaning that it uses Linux-specific functionality. It might be possible to port Wiretap to Unix-like systems such as BSDs by removing all parts that depend on the aforementioned header. As for MacOS and Windows, the user is on their own since I have no idea about how sockets work on those platforms.

## What data do Wiretap logs contain?
Wiretap in its current form only reads the data contained in packet headers, such as source and destination MAC/IPv4/IPv6 addresses, packet identifiers, and the IP protocol used by an IPv4/IPv6 packet (as long as it is ICMP, ICMPv6, TCP or UDP), so the logs only give information about the type of packets received and when they were received. (In the form of an ISO 8601 timestamp.) It should, however, be rather simple to extend Wiretap to also read the payloads of packets.