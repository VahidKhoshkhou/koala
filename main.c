
/*
 * main.c
 *
 *  Created on: Dec 12, 2017
 *      Author: yas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
typedef struct {
	char *kualaId;
	char *familyId;
	int treeId;
	int numPopularTrees;
	int *popularTrees;
} koala_t;

typedef struct {
	char *family_1_Id;
	char *family_2_Id;
} enemy_t;
koala_t *loadKoalas(int);
koala_t *loadFamilies(koala_t *,int , int);
enemy_t *loadEnemies(int *);
void freeKoalas(koala_t *, int);
void freeEnemies(enemy_t *, int );
int checkPopularityOfTree(koala_t , int );
void displaySol(koala_t *,int *,int,int);
int placeKoalas_oneSol(koala_t *koalas,enemy_t *enemies,int *trees,koala_t  *buff,int numCurrentTree,int level,int numKoala,int numEnemies,int treeCapacity);
int incompatibleFamily(koala_t *koalas,enemy_t *enemies,int numkoala,int numEnemies,koala_t *buff,int start,int level);
int areEnemy(enemy_t *enemies,int numEnemies,koala_t *buff);
int main(int argc, char *argv[]) {
	int nTree, nFamily, nKoala, nCapacity_Tree,numEnemy;
	koala_t *koalas;
	enemy_t *enemies;
	int *trees,i;
	if (argc != 5) {
		printf("error in C.L parameter !!\n");
		exit(0);
	}
	nTree = atoi(argv[1]);
	nFamily = atoi(argv[2]);
	nKoala = atoi(argv[3]);
	nCapacity_Tree = atoi(argv[4]);
    trees=(int *)calloc(nTree,sizeof(int));
	koalas = loadKoalas(nKoala);
	koalas = loadFamilies(koalas,nFamily,nKoala);
    enemies=loadEnemies(&numEnemy);

    koala_t *buff;

    buff= (koala_t *) malloc(2 * sizeof(koala_t));
    int found=0;
    for(i=1;i<=nTree&& found==0;i++){
       if(placeKoalas_oneSol(koalas,enemies,trees,buff,i,0,nKoala,numEnemy,nCapacity_Tree)==1){
    	   displaySol(koalas,trees,i,nKoala);
        found=1;
       }
    }

    if(found==0){
    	printf("there was no solution , Sorry\n");
    }

	freeKoalas(koalas, nKoala);
	free(buff);
	freeEnemies(enemies,numEnemy);
	return 0;
}


int placeKoalas_oneSol(koala_t *koalas,enemy_t *enemies,int *trees,koala_t  *buff,int numCurrentTree,int level,int numKoala,int numEnemies,int treeCapacity){
	int i;
	if(level ==numKoala  &&  !incompatibleFamily(koalas,enemies,numKoala,numEnemies,buff,0,0) ){
		return 1;
	 }
	for(i=0;i<numCurrentTree;i++){
         if((trees[i]+1)<=treeCapacity && checkPopularityOfTree(koalas[level],i)){
        	 koalas[level].treeId=i;
        	 trees[i]++;
        	 if(placeKoalas_oneSol(koalas,enemies,trees,buff,numCurrentTree,level+1,numKoala,numEnemies,treeCapacity)){
        		 return 1;
        	 }
        	 koalas[level].treeId=-1;// backtrack
        	 trees[i]--;
         }
	   }

	return 0;
}

int checkPopularityOfTree(koala_t koala, int treeId){
	int i;
	for(i=0;i<koala.numPopularTrees;i++){
		if(koala.popularTrees[i]==treeId){
			return 1;
		}
	}
	return 0;
}
int incompatibleFamily(koala_t *koalas,enemy_t *enemies,int numkoala,int numEnemies,koala_t *buff,int start,int level){
               int i;

             if(level>=2 ){
            	 if(areEnemy(enemies,numEnemies,buff)){
            	 return 1;
            	 }else{
            		 return 0;
            	 }
             }

               for(i=start;i<numkoala;i++){
                    buff[level]=koalas[i];
                    if(incompatibleFamily(koalas,enemies,numkoala,numEnemies,buff,i+1,level+1)==1){
                    	return 1;
                    }
                 }
	return 0;
}

int areEnemy(enemy_t *enemies,int numEnemies,koala_t *buff){
	int i;
    /** are not in the same tree  OR they are from same family  => so can not be enemies */
	if(buff[0].treeId != buff[1].treeId || strcmp(buff[0].familyId,buff[1].familyId)==0){
             return 0;
	}
	for(i=0;i<numEnemies;i++){
		if((strcmp(enemies[i].family_1_Id,buff[0].familyId)==0 && strcmp(enemies[i].family_2_Id,buff[1].familyId)==0)||
		   (strcmp(enemies[i].family_1_Id,buff[1].familyId)==0 && strcmp(enemies[i].family_2_Id,buff[0].familyId)==0) ){
			return 1;
		}
	}

	return 0;
}

enemy_t *loadEnemies(int *numEnemy) {
	FILE *fp;
	enemy_t *enemies;
	char familyId1_tmp[4], familyId2_tmp[4];
	int cnt;
	fp = fopen("enemies.txt", "r");
	if (fp == NULL) {
		printf("error in opening file enemies.txt!\n");
		exit(0);
	}
	cnt = 0;
	while (fscanf(fp, "%s %s", familyId1_tmp, familyId2_tmp) != EOF) {
		cnt++;
	}
	*numEnemy = cnt;
	fclose(fp);

	fp = fopen("enemies.txt", "r");
	if (fp == NULL) {
		printf("error in opening file enemies.txt!\n");
		exit(0);
	}
	enemies = (enemy_t *) malloc(*numEnemy * sizeof(enemy_t));
	cnt = 0;
	while (fscanf(fp, "%s %s", familyId1_tmp, familyId2_tmp) != EOF) {
		enemies[cnt].family_1_Id = strdup(familyId1_tmp);
		enemies[cnt].family_2_Id = strdup(familyId2_tmp);
		cnt++;
	}

	fclose(fp);

	return enemies;
}

koala_t *loadFamilies(koala_t *koalas,int numFamily,int totNumKoala) {
	FILE *fp;
	char koalaId_tmp[4], familyId_tmp[4];
	int numKoala, i, cnt, j;
	fp = fopen("families.txt", "r");
	if (fp == NULL) {
		printf("error in opening file families.txt!\n");
		exit(0);
	}
	cnt = 0;
	while (cnt < numFamily && fscanf(fp, "%s %d", familyId_tmp, &numKoala) != EOF) {
		for (i = 0; i < numKoala; i++) {
			fscanf(fp, "%s", koalaId_tmp);
			for(j=0;j<totNumKoala;j++){
				if(strcmp(koalaId_tmp,koalas[j].kualaId)==0){
					koalas[j].familyId=strdup(familyId_tmp);
				}
			}
		}
		cnt++;
	}
	fclose(fp);

	return koalas;
}

koala_t *loadKoalas(int numKoalas) {
	FILE *fp;
	koala_t *koalas;
	char koalaId_tmp[4];
	int numPopularTree, i, cnt;
	fp = fopen("habitat.txt", "r");
	if (fp == NULL) {
		printf("error in opening file!\n");
		exit(0);
	}

	koalas = (koala_t *) malloc(numKoalas * sizeof(koala_t));
	cnt = 0;

	while (cnt < numKoalas
			&& fscanf(fp, "%s %d", koalaId_tmp, &numPopularTree) != EOF) {
		koalas[cnt].kualaId = strdup(koalaId_tmp);
		koalas[cnt].numPopularTrees = numPopularTree;
		koalas[cnt].treeId = -1;
		koalas[cnt].popularTrees = (int *) malloc(numPopularTree * sizeof(int));
		for (i = 0; i < numPopularTree; i++) {
			fscanf(fp, "%d", &koalas[cnt].popularTrees[i]);
		}
		cnt++;
	}

	fclose(fp);

	return koalas;
}

void displaySol(koala_t *koalas,int *trees,int numTrees,int numKoala){
	int i,j;
	 printf("##################  solution on %d tree : ################## \n",numTrees);
	 for(i=0;i<numTrees;i++){
		 printf("in tree #%d : \n",i);
	            	   for(j=0;j<numKoala;j++){
	            	             if(koalas[j].treeId==i) {
	            	            	 printf(" %s %d \n",koalas[j].kualaId,koalas[j].treeId);
	            	             }
	            	      }
	               }
}

void freeKoalas(koala_t *koalas, int numKoala) {
	int i;
	for (i = 0; i < numKoala; i++) {
		free(koalas[i].kualaId);
		free(koalas[i].popularTrees);
	}
	free(koalas);
}


void freeEnemies(enemy_t *enemies, int numEnemy){
	int i;
		for (i = 0; i < numEnemy; i++) {
			free(enemies[i].family_1_Id);
			free(enemies[i].family_2_Id);
		}
		free(enemies);
}

