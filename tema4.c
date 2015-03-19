// Viziru Luciana Elena - 312CA

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// declaratiile functiilor folosite
void initialize(int n, unsigned char *c);
void finalize(unsigned char *c);
void dump(int n,unsigned char *c);
void alloc(int n, int size, unsigned char *c);
void my_free(int index, unsigned char *c);
void fill(int index, int size, int val, unsigned char *c);
void show_free(int n, unsigned char *c);
void show_usage(int n, unsigned char *c);
void show_allocations(int n, unsigned char *c);
int main(){
// pentru recunoasterea comenzilor, am folosit siruri de caractere
	unsigned char *c;
	char *com, *aux;
	int n, index, size, val;
// comanda este salvata in sirul com si 
// este prelucrata cu ajutorul sirului aux
	com = malloc(30 * sizeof(char));
	aux = malloc(30 * sizeof(char));
	fgets(com, 30, stdin);
	while( 1 ){
		aux = strtok(com, " \n");
		if(strcmp(aux, "INITIALIZE") == 0){
			aux = strtok(NULL, " \n");
			n = atoi(aux);
			c = malloc(n * sizeof(unsigned char));
			initialize(n, c);
		}
		if(strcmp(aux, "FINALIZE") == 0){
			break;
			}
		if(strcmp(aux, "DUMP") == 0){
			dump(n, c);
		}
		if(strcmp(aux, "ALLOC") == 0){
			aux = strtok(NULL, " \n");
			size = atoi(aux);
			alloc(n, size, c);
		}
		if(strcmp(aux, "FREE") == 0){
			aux = strtok(NULL, " \n");
			index = atoi(aux);	
			my_free(index, c);
		}
		if(strcmp(aux, "FILL") == 0){
			aux = strtok(NULL, " \n");
			index = atoi(aux);	
			aux = strtok(NULL, " \n");
			size = atoi(aux);	
			aux = strtok(NULL, " \n");
			val = atoi(aux);	
			fill(index, size, val, c);
		}
		if(strcmp(aux, "SHOW") == 0){
			aux = strtok(NULL, " \n");
			if(strcmp(aux, "FREE") == 0){
				show_free(n,c);
			}
			if(strcmp(aux, "USAGE") == 0){
				show_usage(n,c);
			}
			if(strcmp(aux, "ALLOCATIONS") == 0){
				show_allocations(n,c);
			}
		}
		fgets(com, 30, stdin);
	}
	return 0;
}
// definirea functiilor folosite
void initialize(int n, unsigned char *c){
// functia initializeaza cu 0 zona de memorie corespunzatoare dimesiunii arenei
	int i;
	for(i = 0; i < n; i++){
		*((int *)(c+i)) = 0;
	}
}
void finalize(unsigned char *c){
	free(c);
}
void dump(int n,unsigned char *c){
// functia afiseaza continutul arenei
	int i;
	for(i = 0; i < n; i++){
		if(!(i%16)){
			printf("%08X\t", i);
		}
		printf("%02X ", (unsigned char) *(c+i));
		if((i+1)%8 == 0 && (i+1)%16 && (i+1) < n-1){
			printf(" ");
		}
		if(!((i+1)%16)){
			printf("\n");
		}
	}
	if(n%16){
		printf("\n");
	}
	printf("%08X\n", n);
}
void alloc(int n, int size, unsigned char *c){
// functia cauta un spatiu de dimensiunea data pentru alocare si il rezerva
// am folosit un vector auxiliar pentru cautare si 
// pentru fixarea sectiunii de gestiune
	int v[8], ok = 0, ind = 0;
	//v = (int *)malloc(8 * sizeof(int));
	v[0] = *((int *)c);
	if(!v[0]){
	// daca arena este complet libera
		if(n > size + 16){
		// daca spatiul cerut incape in arena
			*((int *)c) = 4;
			*((int *)(c + 12)) = size + 12;
			*((int *)(c + 4)) = 0;
			ok = 1;
			ind = 16;
		}
		else{
			ok = 1;
			ind = 0;
		}
	}
	else{
		if(v[0] > size + 12){
		// daca spatiul cerut poate fi inserat imediat 
		// dupa indexul de start
			v[4] = *((int *)(c + v[0] + 4));
			v[1] = v[0];
			v[2] = v[4];
			v[3] = size + 12;
			v[0] = 4;
			v[4] = v[0];
			*((int *)c) = v[0];
			*((int *)(c + 4)) = v[1];
			*((int *)(c + 8)) = v[2];
			*((int *)(c + 12)) = v[3];
			*((int *)(c + v[1] + 4)) = v[4];
			ok = 1;
			ind = v[4] + 12;
		}
		else{
		// in caz contrar se trece la urmatoarea sectiune de gestiune
			v[1] = *((int *)(c + v[0]));
			v[3] = *((int *)(c + v[0] + 8));
		}
		while( !ok ){
		// cat timp nu a fost gasit un spatiu
			if(!v[1]){
				if(n - v[0] - v[3] > size + 12){
				// daca urmatorul bloc este ultimul din arena,
				// verific daca sectiunea ceruta mai poate 
				// fi inserata dupa acest bloc
					v[1] = v[0] + v[3];
					v[4] = 0;
					v[5] = v[0];
					v[6] = size + 12;
					*((int *)(c + v[0])) = v[1];
					*((int *)(c + v[1])) = v[4];
					*((int *)(c + v[1] + 4)) = v[5];
					*((int *)(c + v[1] + 8)) = v[6];
					ok = 1;
					ind = v[1] + 12;				
				}
				else{
				// in caz contrar, se returneaza ind 0, 
				// intrucat nu a fost gasit spatiul necesar
					ind = 0;
					break;
				}
			}
			else{
			// daca mai exista blocuri in arena, se verifica 
			// spatiul dintre acestea pentru a insera 
			// sectiunea ceruta
				if(v[1] - v[0] - v[3] > size + 12){
					v[7] = *((int *)(c + v[1] + 4));
					v[4] = v[1];
					v[5] = v[7];
					v[6] = size + 12;
					v[7] = v[0] + v[3];
					v[1] = v[7];
					*((int *)(c + v[0])) = v[1];
					*((int *)(c + v[1])) = v[4];
					*((int *)(c + v[1] + 4)) = v[5];
					*((int *)(c + v[1] + 8)) = v[6];
					*((int *)(c + v[4] + 4)) = v[7];
					ok = 1;
					ind = v[1] + 12;
				}
				else{
				// daca nu exista suficient spatiu, se trece
				// la urmatoarea sectiune de date alocate
					v[0] = v[1];
					v[1] = *((int *)(c + v[0]));
					v[3] = *((int *)(c + v[0] + 8));
				}
			}
		}		
	}
	printf("%d\n", ind);	
}
void my_free(int index, unsigned char *c){
// functia elibereaza o sectiune de memorie alocata anterior,
// incepand cu indicele index
// si in cazul acestei functii, am folosit un vector auxiliar
	int v[4];
	//v =(int *)malloc(4 * sizeof(int));
	v[1] = *((int *)(c + index - 12));
	v[2] = *((int *)(c + index - 8));
	v[0] = *((int *)(c + v[2]));
	v[3] = *((int *)(c + v[1] + 4));
	if(v[1]){
	// in cazul in care mai exista blocuri in arena, in
	// continuarea sectiunii ce urmeaza sa fie stearsa,
	// sunt schimbate sectiunile de gestiune ale blocurilor vecine
		v[0] = v[1];
		v[3] = v[2];
		*((int *)(c + v[2])) = v[0];
		*((int *)(c + v[1] + 4)) = v[3];
	}
	else{
	// in caz contrar, este schimbata doar sectiunea de gestiune a 
	// blocului anterior
		v[2] = *((int *)(c + index - 8));
		*((int *)(c + v[2])) = 0;
	}
}
void fill(int index, int size, int val, unsigned char *c){
// functia insereaza valoarea val intr-o sectiune de date
// de lungime size, incepand cu indicele index
	int i;
	for(i = index; i < index + size; i++){
		*(c+i) = val;
	}
}
void show_free(int n, unsigned char *c){
// functia parcurge arena, stabilind cate blocuri de octeti liberi
// se afla in arena (bl) si cate octeti intrunesc acestea  
	int bl = 0, by = 0, v[3];
// salvez valoarea indexului de start in v[0]
	v[0] = *((int *)c);
	v[2] = 4;
	if(v[0] == 0){
// daca arena este libera, afisez mesajul corespunzator
		printf("%d blocks (%d bytes) free\n", 1, n - 4);
	}
	else{
	// daca arena nu este libera, se trece prin fiecare sectiune de date,
	// cu ajutorul sectiunii de gestiune
		if(v[0] > 4){
		// daca exista spatiu imediat dupa indexul de 
		// start, se contorizeaza
			bl++;
			by = v[0] - v[2];
		}
		v[1] = *((int *)(c + v[0]));
		v[2] = *((int *)(c + v[0] + 8));
		// cat timp nu am ajuns la ultima sectiune din arena, 
		// se contorizeaza numarul sectiunilor libere si intinderea lor
		while( 1 ){
			if(v[1] == 0){
				if(n > v[0] + v[2]){
					bl ++;
					by += n - v[0] - v[2];
				}
				break;
			}
			if(v[1] > v[0] + v[2]){
				bl ++;
				by += v[1] - v[0] - v[2];
			}
			v[0] = v[1];
			v[1] = *((int *)(c + v[0]));
			v[2] = *((int *)(c + v[0] + 8));
			
		}
		printf("%d blocks (%d bytes) free\n", bl, by);
	}
}

void show_usage(int n, unsigned char *c){
// functia parcurge arena cu ajutorul vectorului v, 
// calculand gradul de ocupare al memoriei, eficienta
// si procentul de fragmentare
	int byres = 4, byused = 0, blused, blfree = 0, v[3];
	v[0] = *((int *)c);
	if(v[0] == 0){
		printf("%d blocks (%d bytes) used\n", 0, 0);
		printf("%d%% efficiency\n", 0);
		printf("%d%% fragmentation\n", 0);	
	}
	else{	
		blused = 1;
		if(v[0] > 4){
			blfree ++;
		}
		v[1] = *((int *)(c + v[0]));
		v[2] = *((int *)(c + v[0] + 8));
		while(1){
			if(v[1] == 0){
				byused += v[2] - 12;
				byres += 12;
				if(n > v[0] + v[2]){
					blfree ++;
				}
				break;
			}
			else{
				if(v[1] > v[0] + v[2]){
					blfree ++;
				}
				blused ++;
				byused += v[2] - 12;
				byres += 12;
			}
			v[0] = v[1];
			v[1] = *((int *)(c + v[0]));
			v[2] = *((int *)(c + v[0] + 8));
		}
	}
	int eff, frag;
	eff = 100 * byused / (byused + byres);
	frag = 100 * (blfree - 1) / blused;
	printf("%d blocks (%d bytes) used\n", blused, byused);
	printf("%d%% efficiency\n", eff);
	printf("%d%% fragmentation\n", frag);		
}


void show_allocations(int n, unsigned char *c){
// functia afiseaza, pentru fiecare sectiune de date,
// dimensiunea ei si daca aceasta este ocupata sau nu 
// arena este parcursa asemenea functiei anterioare
	int v[3];
	v[0] = *((int *)c);
	v[2] = 4;
	if(v[0] == 0){
		printf("OCCUPIED %d bytes\n", 4);
		printf("FREE %d bytes\n", n - 4);
	}
	else{
		printf("OCCUPIED %d bytes\n", 4);
		if(v[0] > v[2]){
			printf("FREE %d bytes\n", v[0] - v[2]);
		}
		v[1] = *((int *)(c + v[0]));
		v[2] = *((int *)(c + v[0] + 8));
		while( 1 ){
			if(v[1] == 0){
				printf("OCCUPIED %d bytes\n", v[2]);
				printf("FREE %d bytes\n", n - v[0] - v[2]);
				break;
			}
			else{
				printf("OCCUPIED %d bytes\n", v[2]);
				if(v[1] > v[0] + v[2]){
					printf("FREE %d bytes\n", v[1] - v[0] - v[2]);
				}
			}
			v[0] = v[1];
			v[1] = *((int *)(c + v[0]));
			v[2] = *((int *)(c + v[0] + 8));	
		}
		
	}
}	
