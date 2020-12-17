#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Stock file - database path
const char* DB_PATH = "C:\\Users\\Geo\\Desktop\\Vending Machine C program\\src\\stock.txt";
const char* SECRET_KEY = "admin";
const char* TABLE_HEADER = "Product Number     Name       Quantity      Price";
const int   RACK_SIZE = 10;
const int   MAX_SIZE = 20;
const int   MAX_WORD_SIZE = 20; //20 characters max per word
const int   PRODUCT_COUNT = 20;
const int   COLUMN_WIDTH = 30;  //width of table

int PRODUCT_ID[20];
char* PRODUCT_NAME[20];     //Product list array
int PRODUCT_QUANTITY[20];
double PRODUCT_PRICE[20];

int parseDatabase();
int addProduct(int productID, char *productName, int productQuantity, double productPrice);
void initializeNameArray();
void fillArrays();
void printProductList();
int writeDatabase();

int main(){

    //Setup
    //Allocate memory to product list array
    initializeNameArray();

    if (parseDatabase()){
        printf("Error reading database.\n");
    }
    PRODUCT_QUANTITY[9] = 10;
    PRODUCT_PRICE[9] = 0.85;

    if (writeDatabase()){
        printf("Something went wrong.\n");
    }
    printProductList();

    printf("Choose your item: ");
    char* input = (char*) malloc(sizeof(char)*20);
    scanf("%s", input);
 
    printf("You chose %s\n", input);
    
    
    return 0;
}

void printProductList(){
    int charCount = 0;                          //Keep track of character count to adjust column width
    int temp = 0;                               //temporary variable used to calculate character count
    for (int i = 0; i < PRODUCT_COUNT; i++){    //this loops around the rows of the database
        for (int j = 0; j < 4; j++){            //this loops around the columns and prints each cell
            switch (j)
            {
            case 0:                             //Print the product number
                printf("%i", PRODUCT_ID[i] + 1);
            
                temp = PRODUCT_ID[i];           //get product number from pre-populated array of product numbers

                if (temp == 0){                 //in case the id == 0, the char count is 1
                    charCount = 1;
                }
                else if (temp == 9){            // 10/10 gives 0, so take that into account
                    charCount = 2;
                }
                else {
                    while(temp > 0){            //calculate digit count
                    charCount++;                //increment as long as the result of dividing by 10 is less than 0
                    temp /= 10;                 //keep dividing until number is 0
                    }
                }
                
                break;

            case 1:
                printf("%s", PRODUCT_NAME[i]);
                charCount = strlen(PRODUCT_NAME[i]) - 1;    //char count is calculated directly from the char*
                break;

            case 2:
                printf("%i", PRODUCT_QUANTITY[i]);

                //same calculation as in case 0
                temp = PRODUCT_QUANTITY[i]; 
                while(temp > 0){
                    charCount++;
                    temp /= 10;
                }
                break;

            case 3:
                printf("%.2lf", PRODUCT_PRICE[i]);  //.2 lf --> .2 sets precision to 2 decimal places and lf is for long float/double type

                //same calculations is case 2
                temp = PRODUCT_PRICE[i];
                while(temp > 0){
                    charCount++;
                    temp /= 10;
                }
                break;
            
            default:
                break;
            }

            //Print spaces while adjusting column width to 30 characters wide
            
            for (int k = 0; k < COLUMN_WIDTH - charCount; k++){
                printf(" ");
            }
            charCount = 0;      //reset char count when searching for next word
        }

        //new line at the end of each row
        printf("\n");
    }
}

int writeDatabase(){

    FILE *fileWrite = fopen(DB_PATH, "w");
    if (fileWrite == NULL){
        return 1;
    }
    for (int i = 0; i < 20; i++){
        for (int j = 0; j < 4; j++){
            switch (j)
            {
            case 0:
                fprintf(fileWrite, "%d/", PRODUCT_ID[i]);
                break;

            case 1:
                fprintf(fileWrite, "%s/", PRODUCT_NAME[i]);
                break;

            case 2:
                fprintf(fileWrite, "%d/", PRODUCT_QUANTITY[i]);
                break;

            case 3:
                if (i < 19) fprintf(fileWrite, "%.2lf/\n", PRODUCT_PRICE[i]);
                else fprintf(fileWrite, "%.2lf/", PRODUCT_PRICE[i]);
                break;
            
            default:
                break;
            }
        }
    }
    fclose(fileWrite);
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
    char buffer[30];                    //Buffer to read words
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
                //printf("%i", PRODUCT_ID[rowCount]);
                break;

            case 1: //col2 -> product Name
                //Copy product name from buffer to corresponding row
                //each row corresponds to an element of the PRODUCT_NAME array
                //such that each row of the table represents a product
                strcpy(PRODUCT_NAME[rowCount], buffer);         
                //printf("%s", PRODUCT_NAME[rowCount]);
                break;

            case 2: //col3 -> Product Quantity
                //convert buffer value to int
                //here, value in buffer is the string value of the quantity of the product
                //so we are converting a string of numbers into an integer
                PRODUCT_QUANTITY[rowCount] = atoi(buffer);       
                //printf("%i", PRODUCT_QUANTITY[rowCount]);
                break;

            case 3: //col4 -> Product Price
                PRODUCT_PRICE[rowCount] = (double) atof(buffer);
                break;
            
            default:
                break;
            }

            colCount++;
            charCount = 0;
            strcpy(buffer, "");
        }

        if (charRead == '\n'){
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