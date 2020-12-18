#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Windows.h>
#include <math.h>

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

bool validateInput = false;

int parseDatabase();
int addProduct(int productID, char *productName, int productQuantity, double productPrice);
void initializeNameArray();
void fillArrays();
void printProductList();
int updateDatabase();
bool startTransaction(int quantity, int choice, double productPrice, bool purchaseSuccessful);
void reloadQuantity();
void setPrice();
void clearAllQuantities();
void setAllQuantities();
bool adminInterface(char* input, bool adminMode);

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
    bool doTransaction = false;
    bool enableAdmin = false;
    

    //prompt user
    do {
        
        if (!enableAdmin){
            //Only ask if user is not ordering
            if (!orderInProgress){
                system("cls");
                printProductList();

                printf("Choose your item: ");
                scanf("%s", input);

                //IMPLEMENT
                if (strcmp(input, "admin") == 0){
                    system("cls");
                    Sleep(200);
                    enableAdmin = true;

                    enableAdmin = adminInterface(input, enableAdmin);
                    
                    //break;
                }
                else {
                    choice = atoi(input);
                    choice--;   //take into account that list numbering starts at 1 on user interface
                    orderInProgress = true;
                }
                
            }
            
            if (choice >= 0 && choice < PRODUCT_COUNT){

                //Get data from arrays into variables
                productID = PRODUCT_ID[choice];
                productName = PRODUCT_NAME[choice];
                productPrice = PRODUCT_PRICE[choice];
                productQuantity = PRODUCT_QUANTITY[choice];

                //temporary variable to store user's required quantity
                int quantity;                                   
                
                //Proceed with the trade if the product is in stock
                if (productQuantity > 0){

                    //Ask the user only if a new session has started
                    //retry is true when the user later inputs invalid data
                    if (!retry){
                        printf("You chose %s\n", productName);
                        printf("Enter quantity: ");
                        scanf("%i", &quantity);
                    }
                    
                    //when user asks for more than the quantity in stock,
                    //offer the maximum available amount of this product
                    if (productQuantity < quantity && !doTransaction){
                        printf("Sorry, not enough in stock. Do you want to take ");
                        printf("%i of %s instead? (type: yes/no) ", productQuantity, productName);

                        char* answer = (char*) malloc(sizeof(char) * 10);
                        scanf("%s", answer);

                        //If user answers with yes
                        if (strcmp(answer, "yes") == 0 || strcmp(answer, "Yes") == 0 || strcmp(answer, "YES") == 0){
                            quantity = productQuantity;
                            doTransaction = true;
                        }
                        //If user answers with no
                        else if(strcmp(answer, "no") == 0 || strcmp(answer, "No") == 0){
                            orderInProgress = false;
                        }
                        else {
                            printf("Invalid answer.\n");
                            retry = true;               //raise the retry flag to force user to retry answering
                        }
                        free(answer);                   //free the memory allocated for answer
                    }
                    else {
                        doTransaction = true;           //Allow for initiating the transaction
                    }
                    
                    if (doTransaction){

                        //Perform the transaction
                        //function returns true if the transaction is successful
                        purchaseSuccessful = startTransaction(quantity, choice, productPrice, purchaseSuccessful);
                        if (purchaseSuccessful){
                            doTransaction = false;
                            retry = false;
                        }
                    }
                }
                else{
                    printf("%s is out of stock. Returning to main menu.\n", productName);
                    orderInProgress = false;
                    retry = false;
                    Sleep(3000);
                }
                
            }
            else {
                if (!validateInput){
                    //Invalid product ID is entered
                    printf("%s\n", input);
                    printf("Please enter a product ID from 1 to %i\n", PRODUCT_COUNT);
                    orderInProgress = false;
                    retry = false;
                    Sleep(3000);
                }
            }

            //Print info message upon successful transaction
            if (purchaseSuccessful){

                printf("\n*************************************************************\n\n");
                printf("Please take your %s from the slot below.\n", PRODUCT_NAME[choice]);
                printf("Thank you for using SmartVendo 3000!\n");
                printf("Returning to main menu.\n");

                //Update the stock file
                updateDatabase();
                Sleep(10000);
                orderInProgress = false;            //Stop the current sale session
                
            }
        }
        
    }
    while(true);
    return 0;
}

bool adminInterface(char* input, bool enableAdmin){
    while(enableAdmin){

    printf("**********************************************\n");
    printf("******************ADMIN MODE******************\n");
    printf("**********************************************\n\n");
    Sleep(200);
    printf("++++++++++++++ Select an option ++++++++++++++\n\n");
    Sleep(200);
    printf("1. Reload a product's stock count.\n");
    Sleep(200);
    printf("2. Modify a product's price.\n");
    Sleep(200);
    printf("3. Add a product to stock list.\n");
    Sleep(200);
    printf("4. Delete a product from stock list.\n");
    Sleep(200);
    printf("5. Clear all products' quantities in stock.\n");
    Sleep(200);
    printf("6. Set all products' quantities in stock.\n");
    Sleep(200);
    printf("Choice: ");

    int option;
    char input_choice[4];

    scanf("%s", &input_choice);
    if (strcmp(input_choice, "quit") == 0 || strcmp(input_choice, "QUIT") == 0
        || strcmp(input_choice, "Quit") == 0){
            printf("Quitting Administrator Mode...\n");
            Sleep(1000);
            strcpy(input, input_choice);
            validateInput = true;
            return false;
            break;
    }
    else {
        option = atoi(input_choice);
        if (option > 0 && option <= 6){
            switch (option)
            {
                case 1:
                    reloadQuantity();
                    updateDatabase();
                    break;
                
                case 2:
                    setPrice();
                    updateDatabase();
                    break;

                case 3:
                    
                    break;

                case 4:
                    
                    break;

                case 5:
                    clearAllQuantities();
                    updateDatabase();
                    break;
                    
                case 6:
                    setAllQuantities();
                    updateDatabase();
                    break;
                default:
                    break;
            }
        }
    }
    
    system("cls");
    }
}

//Set all quantities to value chosen by admin
void setAllQuantities(){

    //prompt user
    int quantity;
    printf("Enter quantity to set for all products: ");
    scanf("%i", &quantity);

    //validate input
    if (quantity >= 0){

        //Set all quantities one by one
        for (int i = 0; i < PRODUCT_COUNT; i++){
            PRODUCT_QUANTITY[i] = quantity;
        }   
    }
}

//Reset all quantities to 0
void clearAllQuantities(){
    for (int i = 0; i < PRODUCT_COUNT; i++){
        PRODUCT_QUANTITY[i] = 0;
    }
}

//Set price of a specific product
void setPrice(){
    int ID;
    printf("Enter product ID of item you want to modify: ");
    scanf("%i", &ID);

    double newPrice = -1;

    while(newPrice <= 0){
        printf("Enter new price for %s: ", PRODUCT_NAME[ID]);
        scanf("%lf", &newPrice);

        if (newPrice <= 0){
            printf("Enter a number greater than zero.\n");
        }
    }

    PRODUCT_PRICE[ID] = newPrice;
}

//Set new quantity to any product
void reloadQuantity(){
    printf("Enter product ID of item you want to modify: ");
                                
    int ID;
    scanf("%i", &ID);
    printf("Enter new quantity for %s: ", PRODUCT_NAME[ID]);
    
    int Q;
    scanf("%i", &Q);
    printf("%s new quantity set to %i.\n", PRODUCT_NAME[ID], Q);

    PRODUCT_QUANTITY[ID] = Q;
}

//Call this function when the user should pay
bool startTransaction(int quantity, int choice, double productPrice, bool purchaseSuccessful){
    char inputCoins[10];    //buffer for input of amount of money

    //Prompt the user
    printf("Please insert $ %.2lf in coin slot. (Type the amount): ", productPrice * quantity);
    scanf("%s", &inputCoins);

    double amountInput = atof(inputCoins);                  //calculate float value of the input
    double change = amountInput - productPrice * quantity;  //calculate change

    //Compare the double "change" with 0.01 instead of 0, since doubles are not precise to 0.0
    //return cash to the user
    if (change >= 0.01){

        printf("\n*************************************************************\n");
        printf("Please take $ %.2lf from the slot to the right.\n", change);

        PRODUCT_QUANTITY[choice] -= quantity;               //Decrease stock quantity value of item

        //Update the database
        if (updateDatabase()){
            printf("Error updating stock file. Technical support notified!\n");
        }
        return true;    //return true for done signal
        
    }
    //don't return anything if user paid the exact amount
    else if (change == 0){
        PRODUCT_QUANTITY[choice] -= quantity;
        return true;
    }
    //The user needs to insert more coins
    else if (change < 0){
        while(fabs(change) > 10e-7){
            double remaining = -change;                     //Toggle sign of "change"
            printf("Insert $ %.2lf more: ", remaining);       //request more coins

            char inputCoins[10];                            //buffer for input
            scanf("%s", &inputCoins);                       
            change = atof(inputCoins) - remaining;          //calculate change
            if (change > 0){
                break;                                      //break in case change is greater than 0
                                                            //meaning, the vending machine now owes the user
                                                            //in other words, the user inserted more coins than requested
            }
        }
        
        PRODUCT_QUANTITY[choice] -= quantity;               //decrease stock
    
        //If the vending machine owes the user coins, return it
        if (change >= 0.01){
            printf("Please take $ %.2lf from the slot to the right.\n", change);
            return true;
        }
        return true;
        
    }
    return false;
}

//Print the available products
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

//Update the existing stock file with modifications
int updateDatabase(){
    
    FILE *fileWrite = fopen(DB_PATH, "w");                              //Open the stock file in write mode
    if (fileWrite == NULL){
        return 1;
    }

    //Loop around the 20 rows of the stock file
    for (int i = 0; i < 20; i++){   

        //Loop around the 4 columns of the stock file
        for (int j = 0; j < 4; j++){

            //Write each cell of the table individually based on the column numnber "j"
            switch (j)
            {   
                //Append a forward slash which acts as a delimiter when we read the stock file
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
                    //Avoid printing an extra new line at the end of the stock file
                    if (i < 19) fprintf(fileWrite, "%.2lf/\n", PRODUCT_PRICE[i]);
                    else fprintf(fileWrite, "%.2lf/", PRODUCT_PRICE[i]);            //notice the missing '\n'
                    break;
                
                default:
                    break;
            }
        }
    }
    fclose(fileWrite);
}

//Read the stock file and fill in the appropriate arrays
//We copy the stock file content to arrays to avoid- 
//constantly opening and closing the file stream.
//this way, we can easily read and write data by modifying these arrays
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

//Add a product
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