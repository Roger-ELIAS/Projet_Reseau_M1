#include "extremite.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

char tun[256];
char inip[256];
char inport[256];
char inopt[256];
char outip[256];
char outport[256];

//Tunnel Allocation
int tun_alloc(char *dev) {
	struct ifreq ifr;
	int fd, err;

	if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		perror("alloc tun");
		exit(-1);
	}

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *        IFF_TAP   - TAP device
	 *
	 *        IFF_NO_PI - Do not provide packet information
	 */
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
	if (*dev)
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		close(fd);
		return err;
	}
	strcpy(dev, ifr.ifr_name);

	return fd;
}
//Creation du tunnel avec tun_alloc et Configuration avec configure-tun.sh
int create_tun(char* tunname) {
	int tunfd;
	printf("Creation de %s\n", tunname);
	tunfd = tun_alloc(tunname);
	
	printf("Lancement de ./configure-tun.sh");
	system("bash ./configure-tun.sh");
	system("ip addr");

	if (strcmp(outip, "fc00:1234:2::36") == 0) {
		system("sudo ip route add 172.16.2.176/28 via 172.16.2.10 dev tun0");
		printf("Route ajoutée ! j'espère c OK\n");
	}
	else if (strcmp(outip, "fc00:1234:1::16") == 0) {
		system("sudo ip route add 172.16.2.144/28 via 172.16.2.10 dev tun0");
		printf("Route ajoutée ! j'espère c OK2\n");
	}
	else printf("Cas bizarre ! A vérifier les gars c'est chelou\n");
	return tunfd;
}
//Read du configuration.txt
void read_config(char *filename) {
	FILE *config_file = fopen(filename, "r");
	if (config_file == NULL) {
		perror("Erreur fichier");
		exit(0);
	}
	fscanf(config_file, "tun=%s\n", tun);
	fscanf(config_file, "inip=%s\n", inip);
	fscanf(config_file, "inport=%s\n", inport);
	fscanf(config_file, "outip=%s\n", outip);
	fscanf(config_file, "outport=%s\n", outport);
	printf("---CONFIGURATION---\ntun=%s\ninip=%s\ninport=%s\noptions=%s\noutip=%s\noutport=%s\n",
		tun, inip, inport, inopt, outip, outport);
}
//Main
int main(int argc, char** argv) {


	read_config(argv[1]);
	int tunnel = create_tun(tun);

	int f = fork();
	if (f != 0) {
		//Serveur 
		printf("-----SERVEUR-----");
		ext_out(inport, tunnel);
		kill(f, SIGKILL);
	}
	else {
		//Client
		printf("CLIENT : %s (port : %s)\n-----Entrer pour continuer-----\n", outip, outport);
		getchar();
		ext_in(outip, outport, tunnel);
		kill(getppid(), SIGKILL);
	}
	return 0;
}
