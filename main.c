/// @file main.cpp
/// @author Jason Merino - Garcia
/// @date September 5, 2023
/// @brief This is a library program with basic add, remove entry functionallity plus reading and outputing entries

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>

using namespace std;

// Function prototypes for all the glorious function decomposition
void printMenu();
void LoadLibrary(const string& fileName, vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& invalidEntries);
void readFile(ifstream& database, vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& invalidEntries);
void isInvalid(string ISBN, const string& status, vector<vector<string>>& invalidEntries, vector<string>& Entry);
void parseData(const string& fileLine, string& bookTitle, string&ISBN, string& status, vector<int>& libraryStats, vector<string>& Entry);
void numToStatus(string& status, vector<int>& libraryStats);
void addItem(vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& ivalidEntries);
void displayLibraryEntries(vector<vector<string>>& bookEntries);
void printCheckOutStats(const vector<int> libraryStats);
void removeEntry(vector<vector<string>>& bookEntries);
int searchByName(string& Name, const vector<vector<string>>& bookEntries);
int searchByISBN(const string& ISBN, const vector<vector<string>>& bookEntries);
void searchForLibraryEntry(const vector<vector<string>>& bookEntries);
void printInvalidEntries(const vector<vector<string>>& invalidEntries, const vector<vector<string>>& bookEntries);
void outputLibraryToFile(const vector<vector<string>>& bookEntries);

int main()
{
    string command;
    string fileName;

    /*index 0 holds total number of books, index 1 holds amounts of books in library, index 2 holds amount of books checked out
      index 3 holds amount of books On Loan, index 4 holds amount of books Unknown and index 5 holds amount of books with other status*/
    vector<int> libraryStats(6);

    //Indexes of vector<string> inside vector: 0 holds book title, 1 holds ISBN, 2 hold status description, 3 holds status number
    vector<vector<string>> bookEntries;
    vector<vector<string>> invalidEntries;


    cout << "Welcome to the Library Management System" << endl;
    cout << "----------------------------------------" << endl;
    cout << endl;

    do 
    {
        // Output the menu and acquire a user selection
        printMenu();
        cout << endl << "Enter a command (case does not matter): ";

        // We use getline for all user input to avoid needing to handle
        // input buffer issues relating to using both >> and getline
        getline(cin, command);
        cout << endl;

        if (command == "A" || command == "a"){
            addItem(bookEntries, libraryStats, invalidEntries);
        } else if (command == "C" || command == "c"){
            bookEntries.clear();
            cout << "Your library is now empty." << endl;
        } else if (command == "D" || command == "d"){
            displayLibraryEntries(bookEntries);
        } else if (command == "I" || command == "i"){
            printInvalidEntries(invalidEntries, bookEntries);
        } else if (command == "L" || command == "l"){
            cout << "What database to read from? ";
            getline(cin, fileName);
            cout << endl;
            LoadLibrary(fileName, bookEntries, libraryStats, invalidEntries);
        } else if (command == "O" || command == "o"){
            outputLibraryToFile(bookEntries);
        } else if (command == "P" || command == "p"){
            printCheckOutStats(libraryStats);
        } else if (command == "R" || command == "r"){
            removeEntry(bookEntries);
        } else if (command == "S" || command == "s"){
            searchForLibraryEntry(bookEntries);
        } else if (command == "X" || command == "x"){
        } else{
            cout << "Unknown command." << endl;
        }

        cout << endl;
    } while (!(command == "x" || command == "X"));

    return 0;
}

// Function definitions for all the glorious function decomposition
// Tip: alphabetical order makes it easy to find things if you have
//      a lot of function decomposition.

/// @brief print out the main menu of the management system
void printMenu()
{
    cout << "Library Management Menu" << endl;
    cout << "-----------------------" << endl;
    cout << "A - Add Item To Library" << endl;
    cout << "C - Clear The Library Of All Entries" << endl;
    cout << "D - Display Library Entries" << endl;    
    cout << "I - List Invalid Library Entries" << endl;
    cout << "L - Load Library From File" << endl;    
    cout << "O - Output Library To File" << endl;
    cout << "P - Print Out Checkout Stats" << endl;
    cout << "R - Remove A Library Entry" << endl;      
    cout << "S - Search For A Library Entry" << endl;    
    cout << "X - Exit Program" << endl;
}

//Opens inputted file name and calls helper functions to read and parse file lines. Library stats will be updated at helper function numToStatus
void LoadLibrary(const string& fileName, vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& invalidEntries)
{
    ifstream database;

    database.open(fileName);
    if (!database.is_open()){
        cout << "Could not find the database file." << endl;
    } else{
        readFile(database, bookEntries, libraryStats, invalidEntries);
    }

    database.close();
}

/*Runs through each line of the file and calls parseData to parse the lines.
  The parsed values will be added to vector<string> Entry which will be added to
  vector<vector<string> bookEntries back in main. Entries will be checked to see if they
  are valid and also library stats will be updated*/
void readFile(ifstream& database, vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& invalidEntries)
{
    vector<string> Entry(4);
    int linesRead = 0;
    string fileLine;
    string bookTitle;
    string ISBN;
    string status;

    while(!database.eof() && database.is_open()){
        getline(database,fileLine);
        parseData(fileLine, bookTitle, ISBN, status, libraryStats, Entry);
        Entry.at(0) = bookTitle;
        Entry.at(1) = ISBN;
        Entry.at(2) = status;
        isInvalid(ISBN, status, invalidEntries, Entry);
        bookEntries.push_back(Entry);
        linesRead++;
    }

    cout << "Read in " << linesRead << " lines from the file." << endl;
}

/*Uses formula to check if ISBN number is valid or not. Or if it doesn't contain 13 characters. Or if the status
  isn't valid. If so then it's added to vector<vector<string>> invalidEntries*/
void isInvalid(string ISBN, const string& status, vector<vector<string>>& invalidEntries, vector<string>& Entry){

    int digitsValue = 0;
    int sum = 0;

    //Removes dashes from ISBN string
    while (ISBN.find("-") != string::npos){
        ISBN.erase(ISBN.find("-"), 1);
    }

    //Checks ISBN
    if (ISBN.size() != 13){
        invalidEntries.push_back(Entry);
        return;
    } else {
        int digitsValue = ISBN.at(12) - 48;
        for (size_t i = 0; i < ISBN.size() - 1; ++i){ // -1 so we don't include the last value
            if (i % 2 == 0) {
                sum += (ISBN.at(i) - 48);
            } else if (i % 2 == 1){
                sum += ((ISBN.at(i) - 48) * 3);
            }
        }
        if ( (10 - (sum % 10)) != digitsValue && sum != 0){
            invalidEntries.push_back(Entry);
            return;
        }
    }

    //checks Status
    if (status != "In Library" && status != "Checked Out" && status != "On Loan" && status != "Unknown State"){
        invalidEntries.push_back(Entry);
        return;
    }
}

/*Parses the fileLine based on the index location of the commas in the csv file.
  Edge cases for if the first or second commas arent there means theres no
  ISBN or Status. Also the status number will be converted to a string statement description in numToStatus helper function.*/
void parseData(const string& fileLine, string& bookTitle, string&ISBN, string& status, vector<int>& libraryStats, vector<string>& Entry){
    bookTitle = "";
    ISBN = "";
    status = "";
    int firstComma = fileLine.find(",");
    int secondComma = fileLine.find(",", firstComma + 1);

    if (firstComma != string::npos){
        bookTitle = fileLine.substr(0, firstComma);
    } else {
        bookTitle = fileLine;
        ISBN = "000-0-00-000000-0";
        status = "0";
    }

    if (secondComma != string::npos && firstComma != string::npos){
        ISBN = fileLine.substr(firstComma + 2, secondComma - (firstComma + 2));
        status = fileLine.substr(secondComma + 2);
    } else if (secondComma == string::npos && firstComma != string::npos){
        ISBN = fileLine.substr(firstComma + 2);
        status = "0";
    }

    //Removes trailing white space at the end
    if (bookTitle.back() == ' '){
        bookTitle.pop_back();
    }
    if (ISBN.back() == ' '){
        ISBN.pop_back();
    }
    if (status.back() == ' '){
        status.pop_back();
    }

    //Storing Status number before converting it to a status description
    Entry.at(3) = status;
    numToStatus(status, libraryStats); //Will be stored in Entry.at(2)
}

//number in status string is converted to the according status description string. libraryStats of the according statuses are also updated while we're at it
void numToStatus(string& status, vector<int>& libraryStats){
    libraryStats.at(0) += 1; //Updates number of books in library

    if (status == "0"){
        status = "In Library";
        libraryStats.at(1) += 1;
    } else if (status == "1"){
        status = "Checked Out";
        libraryStats.at(2) += 1;
    } else if (status == "2"){
        status = "On Loan";
        libraryStats.at(3) += 1;
    } else if (status == "3"){
        status = "Unknown State";
        libraryStats.at(4) += 1;
    } else {
        status = "Invalid State";
        libraryStats.at(5) += 1;
    }
}

/*Adds a vector<string> Entry to bookEntries containing the book titel, isbn and status
  to vector bookEntries back in main. libraryStats is also updated accordingly, refer to comment on vector<int> libraryStats 
  declaration in main for more details on libraryStats. We also check if the title contains commas. If the entry contains invalid
  values it will also be added to the vector of invalid entries*/
void addItem(vector<vector<string>>& bookEntries, vector<int>& libraryStats, vector<vector<string>>& invalidEntries){
    string bookTitle;
    string ISBN;
    
    cout << "What is the book title? ";
    getline(cin, bookTitle);
    if (bookTitle.find(",") == string::npos){ //valid book title
        cout << endl;
        cout << "What is the 13-digit ISBN (with hyphens)? ";
        getline(cin, ISBN);
        cout << endl;

        vector<string> Entry(4);
        Entry.at(0) = bookTitle;
        Entry.at(1) = ISBN;
        Entry.at(2) = "In Library";
        Entry.at(3) = "0";
        libraryStats.at(0) += 1;
        libraryStats.at(1) += 1;
        bookEntries.push_back(Entry);

        string status = "In Library";
        isInvalid(ISBN, status, invalidEntries, Entry);

        cout << "The Following Entry Was Added" << endl;
        cout << "-----------------------------" << endl;
        cout << bookTitle << " --- " << ISBN << " --- In Library" << endl;
    } else { // invalid book title
        cout << "The book title cannot contain commas." << endl;
    }
}

//Runs through each vector in bookEntries and prints out the book title, ISBN and status
void displayLibraryEntries(vector<vector<string>>& bookEntries){
    cout << "Your Current Library" << endl;
    cout << "--------------------" << endl;
    for (size_t i = 0; i < bookEntries.size(); ++i){
        cout << bookEntries.at(i).at(0) << " --- " << bookEntries.at(i).at(1) << " --- " << bookEntries.at(i).at(2) << endl;
    }

    if (bookEntries.size() == 0){
        cout << "The library has no books." << endl;
    }
}

//Prints out check out stats of the library based of the data stored in libraryStats of the according statuses. Nothing is changed
void printCheckOutStats(const vector<int> libraryStats){
    cout << "Your Current Library's Stats" << endl;
    cout << "----------------------------" << endl;
    cout << "Total Books: " << libraryStats.at(0) << endl;
    cout << "   In Library: " << libraryStats.at(1) << endl;
    cout << "   Checked Out: " << libraryStats.at(2) << endl;
    cout << "   On Loan: " << libraryStats.at(3) << endl;
    cout << "   Unknown: " << libraryStats.at(4) << endl;
    cout << "   Other: " << libraryStats.at(5) << endl;
}

//Searches library for matching title or ISBN returns results using helper functions searchByName or searchByISBN to find the index
void searchForLibraryEntry(const vector<vector<string>>& bookEntries){
    string name = "";
    string ISBN = "";
    string numericChoice = "";
    int index;

    cout << "Would you like to search by (1) name or (2) ISBN." << endl;
    cout << "Enter the numeric choice: " << endl;
    getline(cin, numericChoice);

    if (numericChoice == "1"){
        cout << "Enter the book name: ";
        getline(cin, name);
        cout << endl;
        index = searchByName(name, bookEntries);
        if (index != -1){
            cout << "The Following Are Your Search Results" << endl;
            cout << "-------------------------------------" << endl;
            cout << bookEntries.at(index).at(0) << " --- " << bookEntries.at(index).at(1) << " --- " << bookEntries.at(index).at(2) << endl;
        } else {
            cout << "The Following Are Your Search Results" << endl;
            cout << "-------------------------------------" << endl;
            cout << "No matching entry found in the library." << endl;
        }
    } else if (numericChoice == "2"){
        cout << "Enter the book 13-digit ISBN (with dashes): ";
        getline(cin, ISBN);
        cout << endl;
        index = searchByISBN(ISBN, bookEntries);
        if (index != -1){
            cout << "The Following Are Your Search Results" << endl;
            cout << "-------------------------------------" << endl;
            cout << bookEntries.at(index).at(0) << " --- " << bookEntries.at(index).at(1) << " --- " << bookEntries.at(index).at(2) << endl;
        } else {
            cout << "The Following Are Your Search Results" << endl;
            cout << "-------------------------------------" << endl;
            cout << "No matching entry found in the library." << endl;
        }
    } else {
        cout << "Invalid search by choice option." << endl;
    }
}

//The matching entry will be removed by using the index it was found at. The index will be -1 if no match was found
void removeEntry(vector<vector<string>>& bookEntries){
    string numericChoice;
    string name = "";
    string ISBN = "";
    int index = 0; 

    cout << "Would you like remove by (1) name or (2) ISBN." << endl;
    cout << "Enter the numeric choice: " << endl;
    getline(cin, numericChoice);
    
    if (numericChoice == "1"){
        cout << "Enter the book name: ";
        getline(cin, name);
        cout << endl;
        index = searchByName(name, bookEntries);
        if (index != -1){
            cout << "The Following Entry Was Removed From The Library" << endl;
            cout << "------------------------------------------------" << endl;
            cout << bookEntries.at(index).at(0) << " --- " << bookEntries.at(index).at(1) << " --- " << bookEntries.at(index).at(2) << endl;
            bookEntries.erase(bookEntries.begin() + index);
        } else {
            cout << "The Following Entry Was Removed From The Library" << endl;
            cout << "------------------------------------------------" << endl;
            cout << "No matching entry found in the library." << endl;
        }
    } else if ( numericChoice ==  "2"){
        cout << "Enter the book 13-digit ISBN (with dashes): ";
        getline(cin, ISBN);
        cout << endl;
        index = searchByISBN(ISBN, bookEntries);
        if (index != -1){
            cout << "The Following Entry Was Removed From The Library" << endl;
            cout << "------------------------------------------------" << endl;
            cout << bookEntries.at(index).at(0) << " --- " << bookEntries.at(index).at(1) << " --- " << bookEntries.at(index).at(2) << endl;
            bookEntries.erase(bookEntries.begin() + index);
        } else {
            cout << "The Following Entry Was Removed From The Library" << endl;
            cout << "------------------------------------------------" << endl;
            cout << "No matching entry found in the library." << endl;
        }
    } else {
        cout << "Invalid remove by choice option." << endl;
    }
}

/*We're gonna run through each entry in bookEntry and compare the ISBN in there with the input. Then we're gonna return the index*/
int searchByISBN(const string& ISBN, const vector<vector<string>>& bookEntries){
    int index = -1; 
    for (size_t i = 0; i < bookEntries.size(); ++i){
        if (bookEntries.at(i).at(1) == ISBN){
            index = i;
        }
    }

    return index; //negative one will be return if no match was found
}

/*Were gonna first modify inputed Name directly since we won't be returning it later. However we'll be modifying a copy of
  name in bookEntries because we don't want to convert that to uppercase. We'll then compare the input string and bookTitle string
  in bookEntries. Once a match is found we'll return the index of the entry. If not return -1*/
int searchByName(string& Name, const vector<vector<string>>& bookEntries){
    int index = -1;

    for (size_t i = 0; i < bookEntries.size(); ++i){
        if (Name == bookEntries.at(i).at(0)){
            index = i;
        }
    }

    return index;
}

void printInvalidEntries(const vector<vector<string>>& invalidEntries, const vector<vector<string>>& bookEntries){
    cout << "The Following Library Entries Have Invalid Data" << endl;
    cout << "-----------------------------------------------" << endl;
    for (size_t i = 0; i < invalidEntries.size(); ++i){
        cout << invalidEntries.at(i).at(0) << " --- " << invalidEntries.at(i).at(1) << " --- " << invalidEntries.at(i).at(2) << endl;
    }

    if (bookEntries.size() == 0){
        cout << "The library has no books." << endl;
    } else if (invalidEntries.size() == 0){
        cout << "The library has no invalid entries." << endl;
    }
}

//Prints current library entries out to a file.
void outputLibraryToFile(const vector<vector<string>>& bookEntries){
    string fileName;
    cout << "Where should the database output to? ";
    getline(cin, fileName);
    cout << endl;

    ofstream outFS;
    outFS.open(fileName);

    for (size_t i = 0; i < bookEntries.size(); ++i){
        outFS << bookEntries.at(i).at(0) << ", " << bookEntries.at(i).at(1) << ", " << bookEntries.at(i).at(3) << endl; //Accessing index three because we want the status number not description
    }

    cout << bookEntries.size() << " lines of data written to " << fileName << endl;

    outFS.close();
}
