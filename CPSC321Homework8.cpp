#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <sys/types.h>
#include <fstream> //this allows for file opening and closing
#include <algorithm> 
#include <mysql_connection.h>
#include <driver.h>
#include <statement.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <exception.h>
#include "HW8.h"

using namespace std;

void mainMenu();
int getInput();
void listCountries();
void addCountries();
void findCountries();
void updateCountries();
string to_upper(const string& s);

int main() {
    mainMenu();
    return 0;
}

void mainMenu() {
    int input = -1;
        cout << 
        "1. List countries" << endl <<
        "2. Add country" << endl <<
        "3. Find countries based on gdp and inflation" << endl <<
        "4. Update country's gdp and inflation" << endl <<
        "5. Exit" << endl <<
        "Enter your choice (1-5):" << endl;
        input = getInput();
        while (input >= 1 && input <= 4) {
            switch(input) {
                case 1:
                    listCountries();
                    break;
                case 2:
                    addCountries();
                    break;
                case 3:
                    findCountries();
                    break;
                case 4: 
                    updateCountries();
                    break;
            }
            mainMenu();
        }
        exit(0);
}

int getInput() {
    int input = -1;
    string holder = "-1";
    cin >> holder;
    if (stoi(holder) >= 1 && stoi(holder) <= 5) {
        input = stoi(holder);
    }
    else {
        cout << "Enter a valid input." << endl;
        return getInput();
    }
    return input;
}

void listCountries() {
    try {
        sql::Driver *driver = get_driver_instance();
	cout << HOST << " " << USER << " " << PASS << endl;
        sql::Connection *con = driver->connect(HOST, USER, PASS);
        con->setSchema("cffrench_DB");

        sql::Statement *stmt = con->createStatement();
        string q = "SELECT * FROM Countries";
        sql::ResultSet *res = stmt->executeQuery(q);
        while (res->next()) {
            string countryCode = res->getString("Country_code");
            string countryName = res->getString("Country_Name");
            cout << countryName << " (" << countryCode << ")" << endl;
        }
    cout << endl;

    delete res;
    delete stmt;
    delete con;
    
    } 
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

void addCountries() {
    try {
        // create a connection
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect(HOST, USER, PASS);
        con->setSchema("cffrench_DB");

        // get the New Country info from the user
        string user_input_1;
        string user_input_2;
        int user_input_3;
        int user_input_4;
        cout << "Country code................: ";
        cin >> user_input_1;
        cout << "Country name................: ";
        cin >> user_input_2;
        cout << "Country per capita gdp (USD): ";
        cin >> user_input_3;
        cout << "Country inflation (pct).....: ";
        cin >> user_input_4;



        // check to see if there is already a country with that code. 
        string q1 = "SELECT * FROM Countries WHERE Country_code = ?";
        sql::PreparedStatement *stmt = con->prepareStatement(q1);
        stmt->setString(1, to_upper(user_input_1));
        sql::ResultSet *test = stmt->executeQuery();
        if (test->next()) {
            cout << "Country Code " << user_input_1 << " already exists." << endl;
            delete stmt;
            delete test;
            return;
        }

        // create and execute query
        string q2 =
        "INSERT INTO Countries (Country_code, Country_name, GDP, Inflation) "
        "VALUES (?, ?, ?, ?)";
        sql::PreparedStatement *prep_stmt = con->prepareStatement(q2);
        prep_stmt->setString(1, to_upper(user_input_1));
        prep_stmt->setString(2, user_input_2);
        prep_stmt->setInt(3, user_input_3);
        prep_stmt->setInt(4, user_input_4);

        cout << endl;

        sql::ResultSet *res = prep_stmt->executeQuery();

        delete stmt;
        delete test;
        delete res;
        delete prep_stmt;
        delete con;

    } 
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
  
}


void findCountries() {    
    try {
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect(HOST, USER, PASS);
        con->setSchema("cffrench_DB");

        int user_input_1;
        int user_input_2;
        int user_input_3;
        cout << "Number of countries to display: ";
        cin >> user_input_1;
        cout << "Minimum per capita gdp (USD)..: ";
        cin >> user_input_2;
        cout << "Maximum inflation (pct).......: ";
        cin >> user_input_3;

        string q =
        "SELECT * " 
        "FROM Countries "
        "WHERE GDP > ? "
        "AND Inflation < ? "
        "LIMIT ? ";
        sql::PreparedStatement *prep_stmt = con->prepareStatement(q);
        prep_stmt->setInt(1, user_input_2);
        prep_stmt->setInt(2, user_input_3);
        prep_stmt->setInt(3, user_input_1);
        sql::ResultSet *res = prep_stmt->executeQuery();

        // res = prep_stmt->executeQuery();


        while (res->next()) {
            string countryCode = res->getString("Country_code");
            string countryName = res->getString("Country_Name");
            int GDP = res->getInt("GDP");
            int Inflation = res->getInt("Inflation");
            cout << countryName << " (" << countryCode << "), " << 
            GDP << ", " << Inflation << endl;
        }

    cout << endl;

    delete prep_stmt;
    delete con;
    delete res;
    
    } 
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }


}

void updateCountries() {
    try {
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect(HOST, USER, PASS);
        con->setSchema("cffrench_DB");

        string user_input_1;
        int user_input_2;
        int user_input_3;
        cout << "Country code................: ";
        cin >> user_input_1;
        cout << "Country per capita gdp (USD): ";
        cin >> user_input_2;
        cout << "Country inflation (pct).....: ";
        cin >> user_input_3;

        string q1 = "SELECT * FROM Countries WHERE Country_code = ?";
        sql::PreparedStatement *stmt = con->prepareStatement(q1);
        stmt->setString(1, to_upper(user_input_1));
        sql::ResultSet *test = stmt->executeQuery();
        if (! test->next()) {
            cout << "Country Code " << user_input_1 << " does not exist." << endl;
            delete con;
            delete test;
            delete stmt;
            return;
        }

        string q =
        "UPDATE Countries " 
        "SET GDP = ?, Inflation = ? "
        "WHERE Country_code = ? ";
        sql::PreparedStatement *prep_stmt = con->prepareStatement(q);
        prep_stmt->setInt(1, user_input_2);
        prep_stmt->setInt(2, user_input_3);
        prep_stmt->setString(3, to_upper(user_input_1));
        sql::ResultSet *res = prep_stmt->executeQuery();

        res = prep_stmt->executeQuery();


    cout << endl;

    delete test;
    delete prep_stmt;
    delete con;
    delete res;
    
    } 
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
}

// helper function 
string to_upper(const string& s) {
    string result = "";
    for(size_t i = 0; i < s.length(); i++)
        result.append(1, toupper(s[i]));
    return result;
}