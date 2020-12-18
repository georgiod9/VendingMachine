#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Windows.h>

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
int updateDatabase();

int main(){

    //Setup
    //Allocate memory to product list array
    initializeNameArray();

    //parseDatabase() will read the products along with
    //their details from the stockfile into appropriate arrays
    if (parseDatabase()){
        printf("Error reading database.\n");
    }

    /*
    Modify stock file here, before writing to database
    */

   //copy changes, if any, to database (stock file)
    if (updateDatabase()){
        printf("Something went wrong.\n");
    }
    
    char* input = (char*) malloc(sizeof(char)*20);
    char* productName = (char*) malloc(sizeof(char)*MAX_WORD_SIZE);
    int productID;
    int productQuantity;
    double productPrice;
    bool orderInProgress = false;
    int choice;
    bool retry = false;
    bool purchaseSuccessful = false;

    //prompt user
    do {
        
        //Only ask if user is not ordering
        if (!orderInProgress){
            system("cls");
            printProductList();

            printf("Choose your item: ");
            scanf("%s", input);

            choice = atoi(input);
            choice--;   //take into account that list numbering starts at 1 on user interface
            orderInProgress = true;
        }
        
        if (choice >= 0 && choice < PRODUCT_COUNT){
            productID = PRODUCT_ID[choice];
            productName = PRODUCT_NAME[choice];
            productPrice = PRODUCT_PRICE[choice];
            productQuantity = PRODUCT_QUANTITY[choice];
            int quantity;
            

            if (productQuantity > 0){

                if (!retry){
                    printf("You chose %s\n", productName);
                    printf("Enter quantity: ");
                    scanf("%i", &quantity);
                }
                
                if (productQuantity < quantity){
                    printf("Sorry, not enough in stock. Do you want to take ");
                    printf("%i of %s instead? (type: yes/no)", productQuantity, productName);

                    char* answer = (char*) malloc(sizeof(char) * 10);
                    scanf("%s", answer);

                    if (answer == "yes" || answer == "Yes" || answer == "YEs" 
                    || answer == "YES" || answer == "yEs" || answer == "YeS"){
                        quantity = productQuantity;
                    }
                    else if(answer == "NO" || answer == "No" || answer == "no"){
                        orderInProgress = false;
                    }
                    else {
                        printf("Invalid answer.\n");
                        retry = true;
                    }
                }
                else {
                    printf("Please insert $ %.2lf in coin slot. (Type the amount): ", productPrice * quantity);

                    char inputCoins[10];
                    scanf("%s", &inputCoins);

                    double amountInput = atof(inputCoins);
                    double change = amountInput - productPrice * quantity;

                    if (change > 0){
                        printf("Please take $ %.2lf from the slot to the right.\n", change);
                        PRODUCT_QUANTITY[choice] -= quantity;
                        if (updateDatabase()){
                            printf("Error updating stock file. Technical support notified!\n");
                        }
                        purchaseSuccessful = true;
                        retry = false;
                        
                    }
                    else if (change == 0){
                        purchaseSuccessful = true;
                        PRODUCT_QUANTITY[choice] -= quantity;
                    }
                    else if (change < 0){
                        while(change < 0){
                            double remaining = -change;
                            printf("Insert $ %.2lf more: ", remaining);

                            char inputCoins[10];
                            scanf("%s", &inputCoins);
                            change = atof(inputCoins) - remaining;
                        }
                        
                        PRODUCT_QUANTITY[choice] -= quantity;
                        if (change == 0){
                            purchaseSuccessful = true;
                        }
                        else if (change > 0){
                            printf("Please take $ %.2lf from the slot to the right.\n", change);
                            purchaseSuccessful = true;
                        }
                        
                    }
                }
            }
            else{
                printf("%s is out of stock.\n", productName);
            }
            
        }
        else {
            printf("Please enter a product ID from 1 to %i\n", PRODUCT_COUNT);
        }

        if (purchaseSuccessful){
            printf("Please take your %s from the slot below.\n", PRODUCT_NAME[choice]);
            printf("Thank you for using SmartVendo 3000!\n");
            printf("Returning to main menu.\n");
            updateDatabase();
            Sleep(10000);
            orderInProgress = false;
            
        }
    }
    while(input != "admin");
    
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

int updateDatabase(){

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