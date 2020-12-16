#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Stock file - database path
const char* DB_PATH = "C:\\Users\\Geo\\Desktop\\Vending Machine C program\\src\\stock.txt";
const char* SECRET_KEY = "admin";
const char* TABLE_HEADER = "Product Number     Name       Quantity      Price";
const int   RACK_SIZE = 10;
const int   MAX_SIZE = 20;
const int   MAX_WORD_SIZE = 20; //20 characters max per word
const int   PRODUCT_COUNT = 20;

int PRODUCT_ID[20];
char* PRODUCT_NAME[20];     //Product list array
int PRODUCT_QUANTITY[20];
int PRODUCT_PRICE[20];

int parseDatabase();
int addProduct(int productID, char *productName, int productQuantity, double productPrice);
void initializeNameArray();
void fillArrays();
void printProductList();

int main(){

    //Allocate memory to product list array
    initializeNameArray();

    printf("Choose your item: ");
    char* input = (char*)malloc(sizeof(char)*20);
    scanf("%s", input);
 
    printf("You chose %s\n", input);
    if (parseDatabase()){
        printf("Error reading database.\n");
    }

    return 0;
}

void printProductList(){
    for (int i = 0; i < PRODUCT_COUNT; i++){
        
    }
}

int parseDatabase(){

    //Open database as read only
    FILE *fileRead= fopen(DB_PATH, "r");
    if (fileRead == NULL){
        return 1;
    }

    char charRead = getc(fileRead);     //Read single character from database (DB) file
    int charCount = 0;                  //Keep track of the character count in a word
    int rowCount = 0;                   //Keep track of entry count, each row is a product
    int colCount = 0;                   //Keep track of which column we are reading from DB
    char buffer[15];                    //Buffer to read words
    strcpy(buffer, "");                 //Initialize buffer

    //Loop through DB file lines
    while (charRead != EOF){
        
        //Use "/" as delimiter between columns in DB
        if (charRead != '/'){
            strncat(buffer, &charRead, 1);  //Append the character to buffer word
            //putchar(charRead);              //Print on screen
            charCount++;                    //increament character count of the current word
        }
        else {
            
            //Fill product data according to column number
            //COL1 = Id,        COL2 = Name
            //COL3 = Quantity,  COL4 = Price
            switch (colCount)
            {
            case 0: //col1 -> product ID
                PRODUCT_ID[rowCount] = rowCount;
                printf("%i", PRODUCT_ID[rowCount]);
                break;

            case 1: //col2 -> product Name
                strcpy(PRODUCT_NAME[rowCount], buffer);         //Copy product name from buffer to corresponding row
                                                                //each row corresponds to an element of the PRODUCT_NAME array
                                                                //such that each row of the table represents a product
                printf("%s", PRODUCT_NAME[rowCount]);
                break;

            case 2: //col3 -> Product Quantity
                PRODUCT_QUANTITY[rowCount] = atoi(buffer);      //convert buffer value to int
                                                                //here, value in buffer is the string value of the quantity of the product
                                                                //so we are converting a string of numbers into an integer 
                printf("%i", PRODUCT_QUANTITY[rowCount]);
                break;

            case 3: //col4 -> Product Price
                PRODUCT_PRICE[rowCount] = atoi(buffer);
                printf("%s", buffer);
                break;
            
            default:
                break;
            }

            //Create even horizontal spacing between columns
            for (int i = 0; i < 20 - charCount; i++){
                printf(" ");
            }

            colCount++;
            charCount = 0;
            strcpy(buffer, "");
        }

        if (charRead == '\n'){
            printf("\n");
            charCount = 0;
            rowCount++;
            colCount = 0;
        }
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

    fseek(fileWrite, 2, SEEK_SET);
    fprintf(fileWrite, "%s", "          ");
    fclose(fileWrite);
    return 0;
    
}

//Allocate memory to product name array
void initializeNameArray(){
    for (int i = 0; i < 20; i++){
        PRODUCT_NAME[i] = (char *)malloc(sizeof(char)*MAX_WORD_SIZE + 1);
    }
}