#include "Ethernet.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
//uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int EthernetClass::begin(uint8_t *mac_address, unsigned long timeout, unsigned long responseTimeout){
  //start with DNS
  return 1;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip){
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server){
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway){
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet){
  //beggiiinn
}

int EthernetClass::maintain(){
  return 0;
}

IPAddress EthernetClass::localIP(){
  IPAddress ret;
  return ret;
}

IPAddress EthernetClass::subnetMask(){
  IPAddress ret;
  return ret;
}

IPAddress EthernetClass::gatewayIP(){
  IPAddress ret;
  return ret;
}

IPAddress EthernetClass::dnsServerIP(){
  IPAddress ret;
  return ret;
}

EthernetClass Ethernet;
