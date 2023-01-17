#include "fonction.h"

int main(int argc, char *argv[]){
	char* host = argv[1];
	char* port = argv[2];
	char* fileName = argv[3];
	
	// Vérification du bon nombre d'argument
	if(argc != 4){
		printf("Il a %d arguments au lieu de 3 \n", argc);
		return(EXIT_FAILURE);
	}
	printf("Envoyer %s à l'host : %s@%s \n", fileName, host, port);
	
	struct addrinfo hints;
	struct addrinfo *res;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family=AF_INET;
	hints.ai_protocol=IPPROTO_UDP;
	int status;
	status = getaddrinfo(host, port, &hints, &res);
	if (status ==-1){
		printf("je ne peux pas trouver le host %s", argv[1]);
	}
	
	char buffer[MAXSIZE]={0};
	char buffer_receive[MAXSIZE]={0};
	buffer[1]=1;
	sprintf(buffer+2, "%s", fileName);
	sprintf( buffer + 3 + strlen(fileName), "NETASCII");
	
	int sock = socket(res->ai_family, res->ai_socktype,res->ai_protocol);
	int bufferSize = 12+strlen(fileName);
	
	int snd = sendto(sock, buffer, bufferSize, 0, res->ai_addr, res->ai_addrlen);
	if (snd == -1){
		printf("Erreur sur l'envoie de la demande.\n");
		return EXIT_FAILURE;
	}
	//la demande a été envoyé 
	
	struct sockaddr adresse;
	socklen_t sizeAdress = sizeof(adresse);
	int nbByte;
	
	
	FILE *file;
	struct stat info;
	if(stat(fileName,&info)!=0) {
		
		file = fopen(fileName,"w");
		
	}
	else {
		printf("Ce fichier existe déjà\n");
		return EXIT_SUCCESS;
	}
	int block=1;
	char buff_ok[MAXSIZE]={0};
	int snd2;
	
	do{
		nbByte=recvfrom(sock, buffer_receive, MAXSIZE, 0, &adresse, &sizeAdress);
		if (nbByte == -1){
			printf("Erreur sur la réception de la demande.\n");
			return EXIT_FAILURE;
		}
		printf("Just received %d bytes\n",nbByte); //la reception c'est bien faite	
		
		if (buffer_receive[0]==0 && buffer_receive[1]==5){
			printf("Erreur recu du serveur, lors du bloc #%d :\n%s\n",buffer_receive[3],buffer_receive+4);
			exit(EXIT_SUCCESS);
			}
		if (buffer_receive[0]==0 && buffer_receive[1]==3){
			buff_ok[1]=4;
			buff_ok[2]=0;
			buff_ok[3]=block;
			block++;
			snd2 = sendto(sock, buff_ok, 4, 0, &adresse, sizeAdress); //On dit au serveur que l'on a bien reçu des données
			if (snd2 == -1){
				printf("Erreur sur l'envoie de la confirmation de l'acquisition.\n");
				return EXIT_FAILURE;
			}
		}
		
		fwrite(buffer_receive+4, sizeof(char), nbByte-4,file);
		
	}while(nbByte ==512);
	
	fclose(file);

	return(0);
}
