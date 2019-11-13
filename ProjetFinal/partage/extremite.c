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


//Affichage de ce qui passe par le serveur dans la sortie standard
void copieStandard(int fd, char *hote, char *port, int tunnel) {
	int nread;
	char buffer[1024];
	int pid = getpid();

	while (1) {
		nread = recv(fd, buffer, 1024, 0);
		if (nread <= 0) {
			printf("nread = %d\n", nread);
			break;
		}
		printf("%d LECTURE SERVEUR :\n", nread);
		for (int i = 0; i < nread; i++) {
			printf("%c", buffer[i]);
		}
		printf("\n");
		write(tunnel, buffer, nread);
	}

	close(fd);
	fprintf(stderr, "[%s:%s](%i): Termine.\n", hote, port, pid);
}

//Serveur
void ext_out(char *port, int tunnel) {
	printf("-----SERVEUR LANCE-----\n");
	int s; /* descripteur de socket */
	struct addrinfo *resol; /* résolution */

	struct addrinfo indic = { AI_PASSIVE, /* Toute interface */
						   PF_INET6,   SOCK_STREAM, 0, /* IP mode connecté */
						   0,          NULL,        NULL, NULL };
	struct sockaddr_in6 client; /* adresse de socket du client */
	client.sin6_addr = in6addr_any;

	fprintf(stderr, "En écoute sur le port %s\n", port);
	int err = getaddrinfo(NULL, port, &indic, &resol);
	if (err < 0) {
		fprintf(stderr, "Resolution: %s\n", gai_strerror(err));
		exit(2);
	}
	
	/* Creation de la socket, de type TCP / IP */
	if ((s = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
		perror("Socket allouée");
		exit(3);
	}
	fprintf(stderr, "Socket numéro : %i\n", s);

	/* On rend le port reutilisable rapidement /!\ */
	int on = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("option socket");
		exit(4);
	}
	fprintf(stderr, "Option(s) OK!\n");

	/* Association de la socket s a l'adresse obtenue par resolution */
	if (bind(s, resol->ai_addr, sizeof(struct sockaddr_in6)) < 0) {
		perror("bind");
		exit(5);
	}
	freeaddrinfo(resol); /* /!\ Liberation mémoire */
	fprintf(stderr, "bind!\n");

	/* la socket est prete a recevoir */
	if (listen(s, SOMAXCONN) < 0) {
		perror("listen");
		exit(6);
	}
	fprintf(stderr, "listen!\n");

	int len = sizeof(struct sockaddr_in6);

	int resolaccept;
	if ((resolaccept = accept(s, (struct sockaddr *)&client, (socklen_t *)&len)) < 0) {
		perror("accept");
		exit(7);
	}

	/* Nom reseau du client */
	char hoteclient[NI_MAXHOST];
	char portclient[NI_MAXSERV];
	err = getnameinfo((struct sockaddr *)&client, len, hoteclient, NI_MAXHOST, portclient, NI_MAXSERV, 0);
	if (err < 0) {
		fprintf(stderr, "résolution client (%i): %s\n", resolaccept, gai_strerror(err));
	}
	else {
		fprintf(stderr, "accept! (%i) ip=%s port=%s\n", resolaccept, hoteclient, portclient);
	}
	/* traitement */
	copieStandard(resolaccept, hoteclient, portclient, tunnel);

}
//Client
void ext_in(char* hote, char* port, int tunnel) {
	printf("-----CLIENT LANCE-----\n");
	char ip[NI_MAXHOST];    /* adresse IPv4 en notation pointee */
	struct addrinfo *resol; /* struct pour la resolution de nom */
	int s;                  /* descripteur de socket */

	if (port == NULL || hote == NULL) {
		printf("Usage: client + hote + port\n");
		exit(1);
	}

	/* Resolution de l'hote */

	if (getaddrinfo(hote, port, NULL, &resol) < 0) {
		perror("résolution adresse");
		exit(2);
	}

	/* On extrait l'addresse IP */
	// sprintf(ip, "%s",
		 // inet_ntoa(((struct sockaddr_in *)resol->ai_addr)->sin_addr));
	sprintf(ip, "%s",
		inet_ntop(resol->ai_family, resol->ai_addr, ip, NI_MAXHOST));

	printf("------->Adresse IP Client = %s\n", ip);

	/* Creation de la socket, de type TCP / IP */
	/* On ne considere que la premiere adresse renvoyee par getaddrinfo */
	if ((s = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
		perror("allocation de socket");
		exit(3);
	}
	fprintf(stderr, "Socket numéro : %i\n", s);
	/* Connexion */
	fprintf(stderr, "Tentative de connexion a l hote %s (%s) au port %s\n\n", hote,ip, port);
	if (connect(s, resol->ai_addr, sizeof(struct sockaddr_in6)) < 0) {
		perror("connexion");
		exit(4);
	}
	freeaddrinfo(resol); /* /!\ Liberation memoire */

	/* Session */

	char buffer[1024];
	int nread;

	while (1) {
		nread = read(tunnel, buffer, 1024);
		if (nread <= 0) {
			printf("nread = %d\n", nread);
			break;
		}

		printf("%d Lecture par le  client : \n", nread);
		for (int i = 0; i < nread; i++) {
			printf("%c", buffer[i]);
		}
		printf("\n");

		send(s, buffer, nread, 0);
	}

	close(s);
	fprintf(stderr, "Fin de la session.\n");
}
