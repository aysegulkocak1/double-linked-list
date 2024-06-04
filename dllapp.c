#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 200
struct Node {
    char *address;
    int counter;
    struct Node *next;
    struct Node *prev;
};

char ** readAndCopy(char *filename,int *lineCount);
int getNodeCount(struct Node* head);
void increaseCountOrCreateNode(struct Node **head, struct Node **tail, char *page, int threshold, int capacity);
void print(struct Node* head);
void deleteList(struct Node** head, struct Node** tail);

int main() {
    struct Node *head = NULL;
    struct Node *tail = NULL;
    int threshold;
    int capacity;

    printf("Esik degeri giriniz: ");
    scanf("%d", &threshold);
    printf("Kapasite degerini giriniz: ");
    scanf("%d", &capacity);
	printf("1- manuel\n2- read to file\n");
	int choice;
	do{
	
		scanf("%d",&choice);
	}while(choice > 2 || choice <=0);
	
    char *page=(char *)malloc(MAX_LENGTH*sizeof(char));
    if (choice == 1){
    	printf("sayfa adreslerini giriniz cikmak icin exit yaziniz\n");
    	do{
    		printf("adress: ");
    		scanf("%s",page);
    		if(strcmp(page, "exit") != 0){
    			increaseCountOrCreateNode(&head, &tail, page, threshold, capacity);
    			print(head);
    			printf("\n");
			}
		}
		while(strcmp(page, "exit") != 0);
		printf("Cache i temizlemek istiyor musunuz?(e/h)");
		char choiceForDel;
		scanf(" %c",&choiceForDel);
		if(choiceForDel =='e' || choiceForDel =='E'){
			deleteList(&head,&tail);
			printf("temizlendi\n");
		}
		free(page);
	}
	else if(choice == 2){
		printf("dosya ismini giriniz\n");
		char * filename =(char*)malloc(MAX_LENGTH*sizeof(char));
		scanf("%s",filename);
		char ** adresses;
		int count =0;
		adresses = readAndCopy(filename,&count);
		int i;
		for(i=0;i<count;i++){
			increaseCountOrCreateNode(&head,&tail,adresses[i],threshold,capacity);
			print(head);
			printf("\n");
		}
		
		
		printf("Cache i temizlemek istiyor musunuz?(e/h)");
		char choiceForDel;
		scanf(" %c",&choiceForDel);
		if(choiceForDel =='e' || choiceForDel =='E'){
		
			deleteList(&head,&tail);
			printf("temizlendi\n");
		}
		for(i=0;i<count;i++){
			free(adresses[i]);
		}
		free(adresses);
	}
	

    return 0;
}

//  dosyayi oku ve icerigini kopyala
// kullanicidan alinan dosya ismindeki dosyayi okuma modunda ac
//  dosyadaki satir sayisini buldur ve dosyanin basina d�n
//  dosyanin iceri�ini satir satir char ** tipindeki degiskene kopyala
// char ** tipindeki dosyanin icerigi kopyalanan degisken
char ** readAndCopy(char *filename,int *lineCount){
	FILE *file =fopen(filename,"r");
	char *buffer = (char*)malloc(MAX_LENGTH*sizeof(char));
	fseek(file,0,SEEK_SET);
	while(fgets(buffer, MAX_LENGTH,file)!=NULL){
		(*lineCount)+=1;// satir sayisi
	}
	rewind(file);
	char ** allAdresses=(char**)malloc((*lineCount)*sizeof(char*));
	int i=0;
	while(fgets(buffer, MAX_LENGTH,file)!=NULL){
		allAdresses[i]=(char*)malloc(strlen(buffer)+1);
		if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
		strcpy(allAdresses[i],buffer);
		i++;
	}
	return allAdresses;
	
}
// d�g�m sayisini bul
//  parametre olarak listenin basini al ve current adinda struct Node* tipindeki degiskene at
//  while d�ng�s� icerisinde bir current degiskeninin nextlerine bak her iterasyonda counti arttir
// node sayisini veren count degiskenini dondur

int getNodeCount(struct Node* head) {
    int count = 0;
    struct Node* current = head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
// girdinin islenmesi , cache buffer in bakimi
//  eger listede d�g�m yoksa head null olur head nullsa ilk elemani heade ve taile ekle
//  eger liste doluysa gelen istek daha once geldiyse counterini bir arttir ve esik degeri astiysa o d�g�m� basa getir
//  Third eger istek yeni bir istekse listenin basina eklenir 
// getNodeCount fonksiyonu cagirilarak kac d�g�m oldugu elde edilir d�g�m sayisi kapasiteyi astiysa son d�g�m silinir


void increaseCountOrCreateNode(struct Node **head, struct Node **tail, char *page, int threshold, int capacity) {
	if (*head == NULL) {
        // �lk node 
        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->address = (char *)malloc(strlen(page) + 1);
        strcpy(newNode->address, page);
        newNode->counter = 1;
        newNode->prev = NULL;
        newNode->next = NULL;
        *head = newNode;
        *tail = newNode;
        
    }else{
		// istek daha once geldiyse counteri arttir
	    struct Node *current = *head;
	    int flag = 0;
	    while (current != NULL && flag != 1) {
	        if (strcmp(current->address, page) == 0) {
	            current->counter++;
	            if (current->counter > threshold) {
	                if (current != *head) {
	                    current->prev->next = current->next;
	                    if (current->next != NULL)
	                        current->next->prev = current->prev;
	                    else
	                        *tail = current->prev;
	                        
	                    current->prev = NULL;
	                    current->next = *head;
	                    (*head)->prev = current;
	                    *head = current;
	                }
	            }
	        	flag = 1;
	        } else {
	            current = current->next;
	        }
	    }
		// istek yoksa
	    if (flag == 0) {
	    	// istegi basa ekle
	        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
	        newNode->address = (char *)malloc(strlen(page)+1);
	        strcpy(newNode->address, page);
	        newNode->counter = 1;
	        newNode->prev = NULL;
	        newNode->next = *head;
	        (*head)->prev = newNode;
	        *head = newNode;
			// kapasite asildiysa son d�g�m� sil
	        int currentCapacity = getNodeCount(*head);
	        if (currentCapacity > capacity) {
	            struct Node *temp = *tail;
	            *tail = (*tail)->prev;
	        	(*tail)->next = NULL;
	            free(temp->address);
	            free(temp);
	        }
	    }
	}
}
// listeyi yazdir
// parametre olarak listenin headini al ve struct Node* tipinde current adindaki degiskene headi ata
// while d�ng�s� icerisinde currentin nextlerine giderek listenin t�m nodelarinin adreslerini ve sayaclarini yazdir
// Nothing

void print(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf(" %s,%d ", current->address, current->counter);
        printf("<->");
        current = current->next;
    }
}
// listeyi sil, bufferi temizle
// listenin basini ve sonunu parametre olarak al ve current a basini ata
// while d�ng�s� icerisinde currentlarin nextlerine giderek listenin t�m nodelarini freele
// listenin basini ve sonunu NULL olarak ata

void deleteList(struct Node** head, struct Node** tail) {
    struct Node* current = *head;
    while (current != NULL) {
        struct Node* temp = current;
        current = current->next;
        free(temp->address);
        free(temp);
    }
    *head = NULL;
    *tail = NULL;
}


