#ifndef _MAC_H_2015_05_22_LC_
#define _MAC_H_2015_05_22_LC_ 

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "logSystem.h"
#include <netinet/in.h>
#include <arpa/inet.h>




inline bool get_mac(std::string& smac,char* dev,ILogHandle* _log)
{

	  LogWrapper  log(_log);

	  char    mac[32] = {0};

    struct ifreq ifreq;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
    	  log.handle(std::string("mac::socket::error") + strerror(errno) + "\n");
        return false;
    }

    strcpy (ifreq.ifr_name, dev);    //Currently, only get eth0

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        log.handle(std::string("mac::ioctl::error") + strerror(errno) + "\n");
        close(sock);
        return false;
    }
    
    snprintf (mac, 32, "%02X:%02X:%02X:%02X:%02X:%02X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);


    smac =  mac;
    log.handle(smac);
    close(sock);

    return  true;
}


inline bool get_ip(std::string& sip,char* dev,ILogHandle* _log)
{

  LogWrapper  log(_log);

  struct   sockaddr_in  saddr;

  struct ifreq ifr;

  int sock;

  if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
  {
        log.handle(std::string("ip::socket::error") + strerror(errno) + "\n");

        return false;
  }

  strcpy(ifr.ifr_name,dev);

  if(ioctl(sock,SIOCGIFADDR,&ifr) < 0)
  {
        log.handle(std::string("ip::ioctl::error") + strerror(errno) + "\n");
        close(sock);
        return false;
  }

   memcpy(&saddr,   &ifr.ifr_addr,   sizeof(saddr));

   sip = inet_ntoa(saddr.sin_addr);

   log.handle(sip);

   close(sock);
   
   return true;
}

#endif
