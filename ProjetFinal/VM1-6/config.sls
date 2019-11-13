# Configuration eth1
# RAPPEL: eth0 est à vagrant, ne pas y toucher

## Désactivation de network-manager
NetworkManager:
  service:
    - dead
    - enable: False
    
## Suppression de la passerelle par défaut
ip route del default:
  cmd:
    - run

##Configuration de VM1-6
eth1:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - ipaddr: 172.16.2.156
    - netmask: 28

eth2:
  network.managed:
    - enabled: True
    - type: eth
    - proto: none
    - enable_ipv4: false
    - ipv6proto: static
    - enable_ipv6: true
    - ipv6_autoconf: no
    - ipv6ipaddr: fc00:1234:1::16
    - ipv6netmask: 64

## Configuration de la route vers LAN2-6 via VM2-6
routes:
  network.routes:
    - name: eth2
    - routes:
      - name: LAN2-6
        ipv6ipaddr: fc00:1234:1::/64
        gateway: fc00:1234:1::26

net.ipv4.ip_forward:
  sysctl:
    - present
    - value: 1