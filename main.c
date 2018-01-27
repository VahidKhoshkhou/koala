#include <stdio.h>
#include <stdlib.h>
#define FASI 4
#define PEGGIORE -10000

typedef struct _e {
	char *nome;
	int dispendio;
	int nFasi;
	int *fasi;
	int usato;
} esercizio;

esercizio *leggi_file(char *nomefile, int* nEs);
void soluzione(int m, int n, esercizio *esercizi, int nEs);
void ricorri(int livello, int n, int *soluzione_attuale, int *soluzione_migliore, int dispendio_attuale, int* dispendio_migliore, esercizio *esercizi, int m, int nEs, int fase);
int esercizio_compatibile(int indice, int livello, int *soluzione_attuale, esercizio *esercizi, int fase);
void stampa_togli(esercizio *esercizi, int *soluzione_migliore, int n);

int main(int argc, char *argv[])
{
	char buf[30];
	int nEs, n, m;
	esercizio *lista;

	printf("File di ingresso? ");
	scanf("%s", buf);
	lista = leggi_file(buf, &nEs);
	printf("Numero di esercizi massimi per fase? ");
	scanf("%d", &n);
	printf("Dispendio energetico massimo? ");
	scanf("%d", &m);
	soluzione(m, n, lista, nEs);

	system("PAUSE");	
	return 0;
}

esercizio *leggi_file(char *nomefile, int* nEs) {
	int i, j, N;
	FILE *fp;			/* Dichiarazione dimenticata sull'elaborato svolto */
	char buf[20+1];
	esercizio *vettore;
	
	fp = fopen(nomefile, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	fscanf(fp, "%d", &N);
	*nEs = N;
	vettore = (esercizio*) calloc(N, sizeof(esercizio));
	for (i = 0; i < N; i++) {
		/* & per gli interi dimenticate sull'elaborato svolto */
		fscanf(fp, "%s%d%d", buf, &vettore[i].dispendio, &vettore[i].nFasi);
		vettore[i].nome = strdup(buf);
		vettore[i].fasi = (int*) calloc(vettore[i].nFasi, sizeof(int));
		for (j = 0; j < vettore[i].nFasi; j++) 
			fscanf(fp, "%d", &vettore[i].fasi[j]);
	}
	fclose(fp);
	return vettore;
}

void soluzione(int m, int n, esercizio *esercizi, int nEs) {
/* Differenze con l'elaborato svolto:
	1)	ricorri è void e dunque non ha un ritorno, eliminati tutti tranne quello 
		in questa funzione
	2)	sistemate meglio le dichiarazioni di soluzione e dispendio (attuale e migliore) 
		per ridurre gli "azzeramenti" dopo ogni soluzione migliore trovata 
		(l'ho indicato in ultima pagina, ma mancava poco per correggere tutto)
	3)	ciclo da 0 a N, mentre le fasi vanno da 1 a N+1 (risolvo incrementando 
		il risultato stampato)
	4)	cambiato qualche nome alle variabili
*/		
	int *soluzione_attuale;
	int *soluzione_migliore;
	int dispendio_attuale;
	int dispendio_migliore;
	int i;

	for (i = 0; i < FASI; i++) {
		soluzione_attuale = (int*) calloc(n, sizeof(int));
		soluzione_migliore = (int*) calloc(n, sizeof(int));
		dispendio_attuale = 0;
		dispendio_migliore = PEGGIORE;
		ricorri(0, n, soluzione_attuale, soluzione_migliore, dispendio_attuale, &dispendio_migliore, esercizi, m, nEs, i);
		printf("FASE %d\n", i+1);
		stampa_togli(esercizi, soluzione_migliore, n);
		dispendio_attuale = 0;
		dispendio_migliore = PEGGIORE; 
		free(soluzione_attuale);
		free(soluzione_migliore);
	}
	return;
}

void ricorri(int livello, int n, int *soluzione_attuale, int *soluzione_migliore, int dispendio_attuale, int* dispendio_migliore, esercizio *esercizi, int m, int nEs, int fase) {
/* Differenze con l'elaborato svolto
	1)	il campo usato va gestito fuori dalla ricorsione, una volta stabilita 
		la soluzione migliore per una determinata fase
	2)  invertito il controllo sul massimo e la condizione di terminazione 
	3)	cambiato qualche nome alle variabili
*/
	int i, j;
	if (dispendio_attuale > m) return;

	if (livello == n) {
		if ((dispendio_attuale >= *dispendio_migliore)) {
			*dispendio_migliore = dispendio_attuale;
			for (i = 0; i < n; i++)
				soluzione_migliore[i] = soluzione_attuale[i];
		}
		return;
	}

	for (i = -1; i < nEs; i++) {
		if (esercizio_compatibile(i, livello, soluzione_attuale, esercizi, fase)) {
			soluzione_attuale[livello] = i;
			if (i != -1) dispendio_attuale += esercizi[i].dispendio;
			ricorri(livello + 1, n, soluzione_attuale, soluzione_migliore, dispendio_attuale, dispendio_migliore, esercizi, m, nEs, fase);
			if (i != -1) dispendio_attuale -= esercizi[i].dispendio;		
		}
	}
}

int esercizio_compatibile(int indice, int livello, int *soluzione_attuale, esercizio *esercizi, int fase) {
/* Differenze con l'elaborato svolto
	1)	il controllo viene fatto su fase+1 e non su fase (in quanto per la i+1 esima fase ricevo l'indice i)
	2) 	dimenticato il controllo se l'esercizio è già stato usato (e in quel caso non posso più usarlo)
	3) 	eliminata qualche variabile "superflua"
	4)	cambiato qualche nome alle variabili
*/
	int i = 0, forse = 0;
	if (indice == -1) return 1;
	if (esercizi[indice].usato == 1) return 0;
	while (i < esercizi[indice].nFasi) {
		if (esercizi[indice].fasi[i] == fase + 1) 
			forse = 1;
		i++;
	}
	if (forse == 0) return 0;
	i = 0;
	while (i < livello) {
		if (soluzione_attuale[i] == indice) return 0;
		i++;
	}
	return 1;
}

void stampa_togli(esercizio *esercizi, int *soluzione_migliore, int n) {
/* Differenze con l'elaborato svolto
	1)	in base al risultato della soluzione migliore, oltre a stampare, segno l'esercizio come usato
*/
	int i = 0;
	for (i = 0; i < n; i++) {
		if (soluzione_migliore[i] != -1) {
			printf("%s %d\n", esercizi[soluzione_migliore[i]].nome, esercizi[soluzione_migliore[i]].dispendio);
			esercizi[soluzione_migliore[i]].usato = 1;
		}
	}
}


