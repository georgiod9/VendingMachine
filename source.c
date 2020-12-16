#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Stock file - database path
const char* DB_PATH = "C:\\Users\\Geo\\Desktop\\Vending Machine C program\\stock.txt";
const char* SECRET_KEY = "admin";
const char* TABLE_HEADER = "Product Number     Name       Quantity      Price";

int displayItems();
int addProduct(int productID, char *productName, int productQuantity, double productPrice);

int main(){
    /*
    FILE *filePtr;
    filePtr = fopen(DB_PATH, "w+");

    //exit with code 1 if stock file does not exist
    if (filePtr == NULL){
        return 1;
    }

    fprintf(filePtr, "%s", TABLE_HEADER);

    fclose(filePtr);
    */
    printf("Choose your item: ");
    char* input = (char*)malloc(sizeof(char)*20);
    scanf("%s", input);
 
    printf("You chose %s\n", input);
    if (displayItems()){
        printf("Error reading database.\n");
    }

    if (addProduct(1, "CHIPS", 10, 4)){
        printf("Error reading database.\n");
    }

    if (displayItems()){
        printf("Error reading database.\n");
    }
    return 0;
}

int displayItems(){

    FILE *fileRead= fopen(DB_PATH, "r");
    if (fileRead == NULL){
        return 1;
    }

    char charRead = getc(fileRead);
    while (charRead != EOF){
       putchar(charRead);
       charRead = getc(fileRead);
    }
    
    fclose(fileRead);

    return 0;
}

int addProduct(int productID, char *productName, int productQuantity, double productPrice){
    FILE *fileWrite = fopen(DB_PATH, "w+");
    if (fileWrite == NULL){
        return 1;
    }

    fseek(fileWrite, 5, SEEK_CUR);
    fprintf(fileWrite, "%s", "          ");
    fclose(fileWrite);
    return 0;
    
}