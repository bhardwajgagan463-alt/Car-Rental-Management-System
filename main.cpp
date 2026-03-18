#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <conio.h>
using namespace std;

/* ***********************************************
Car Class : Represents each car in the system
**************************************************/
class Car
{
public:
    int carID;
    string model;
    string cartype;
    float rentPerhour;
    bool isAvailable;

    Car(int id, string m, string t, float rent, bool avail = true)
    {
        carID = id;
        model = m;
        cartype = t;
        rentPerhour = rent;
        isAvailable = avail;
    }

    void display()
    {
        cout << left << setw(8) << carID
             << setw(25) << model
             << setw(12) << cartype
             << "Rs." << setw(10) << rentPerhour
             << setw(12) << (isAvailable ? "Yes" : "No")
             << endl;
    }
};

/* ***********************************************
Rental Class : Stores details of each rental
**************************************************/
class Rental
{
public:
    int rentalID;
    int carID;
    string customerName;
    string rentDate;
    string returnDate;
    float totalAmount;

    Rental(int r, int c, string n, string rentD, string retD, float amount)
    {
        rentalID = r;
        carID = c;
        customerName = n;
        rentDate = rentD;
        returnDate = retD;
        totalAmount = amount;
    }
};

/* ***********************************************
                Global Storage
**************************************************/

vector<Car> Cars;

/* ***********************************************
                Load cars
**************************************************/
void loadCars()
{
    Cars.push_back(Car(1, "Swift Desire", "SEDAN", 200));
    Cars.push_back(Car(2, "Honda City", "SEDAN", 350));
    Cars.push_back(Car(3, "Volkswagen Virtus", "SEDAN", 400));
    Cars.push_back(Car(4, "Maruti Ciaz", "SEDAN", 450));
    Cars.push_back(Car(5, "Toyota Etios", "SEDAN", 500));
    Cars.push_back(Car(6, "Mahindra XUV700", "SUV", 500));
    Cars.push_back(Car(7, "Mahindra Scorpio-N", "SUV", 600));
    Cars.push_back(Car(8, "Tata Safari", "SUV", 700));
    Cars.push_back(Car(9, "Hyundai Alcazar", "SUV", 800));
    Cars.push_back(Car(10, "Mahindra Thar", "SUV", 1000));
    Cars.push_back(Car(11, "Jaguar F-Type", "SPORTS", 3000));
    Cars.push_back(Car(12, "BMW M-5 Competition", "SPORTS", 3500));
    Cars.push_back(Car(13, "Porsche 911", "SPORTS", 4000));
    Cars.push_back(Car(14, "Audi R8", "SPORTS", 4500));
    Cars.push_back(Car(15, "Mercedes CL63", "SPORTS", 5000));
}

/* ***********************************************
                List cars
**************************************************/
void listCars()
{
    cout << "\n--- All Cars ---\n";
    cout << left << setw(8) << "Car ID"
         << setw(25) << "Model"
         << setw(12) << "Type"
         << setw(13) << "Rent/hour"
         << setw(12) << "Available"
         << endl;

    cout << string(70, '-') << endl;

    for (int i = 0; i < Cars.size(); i++)
    {
        Cars[i].display();
    }
    system("pause");
}

/* ***********************************************
                Current Date -Time
**************************************************/
string getCurrentDateTime()
{
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localtime(&t));
    return string(buffer);
}

/* ***********************************************
                Rent cars
**************************************************/
void rentCar()
{
    listCars();
    int id;
    string name;
    cout << "\nEnter Car ID to rent: ";
    cin >> id;

    for (auto &car : Cars)
    {
        if (car.carID == id)
        {
            if (car.isAvailable == false)
            {
                cout << "This car is already rented.\n";
                system("pause");
                return;
            }

            cout << "Enter your name: ";
            cin.ignore();
            getline(cin, name);

            string rentDate = getCurrentDateTime();
            car.isAvailable = false;

            ofstream fout("active_rentals.txt", ios::app);
            fout << id << "," << name << "," << rentDate << endl;
            fout.close();

            cout << "\nCar Rented Successfully!";
            cout << "\nRented on: " << rentDate << endl;
            system("pause");
            return;
        }
    }
    cout << "Car ID not found.\n";
    system("pause");
}

/* ***********************************************
                Cars That Are Rented
**************************************************/
void activeRental()
{
    ofstream fout("active_rentals.txt", ios::app);
    if (!fout)
    {
        cout << "File donesn't exist!";
        return;
    }
    ifstream fin("active_rentals.txt", ios::in);
    if (!fin)
    {
        cout << "File donesn't exist!";
        return;
    }
    string line;
    while (getline(fin, line))
    { // Read line by line
        cout << line << endl;
    }
    fin.close();
    system("pause");
}

/* ***********************************************
 Return Car (Fixed with real-time rent calculation)
**************************************************/
void returnCar()
{
    cout << "---------------------------------------" << endl;
    cout << "       Active Renatls" << endl;
    cout << "---------------------------------------" << endl;
    activeRental();
    int id;
    cout << "\nEnter Car ID to return: ";
    cin >> id;

    ifstream fin("active_rentals.txt");
    ofstream temp("temp.txt");
    string line;
    bool found = false;

    while (getline(fin, line))
    {
        int fileCarID;
        string name, rentDate;

        stringstream ss(line);
        string tempStr;

        getline(ss, tempStr, ',');
        fileCarID = stoi(tempStr); // stoi means string to integer
        getline(ss, name, ',');
        getline(ss, rentDate, ',');

        if (fileCarID == id)
        {
            found = true;
            string returnDate = getCurrentDateTime();

            tm tmRent = {};
            istringstream ssRent(rentDate);
            ssRent >> get_time(&tmRent, "%d-%m-%Y %H:%M:%S");
            time_t rentTime = mktime(&tmRent);

            // Current return time
            time_t returnTime = time(nullptr);

            // Calculate time difference in hours
            double seconds = difftime(returnTime, rentTime);
            int hours = max(1, (int)(seconds / 3600)); // at least 1 hour charge

            float total = 0;
            for (auto &car : Cars)
            {
                if (car.carID == id)
                {
                    car.isAvailable = true;
                    total = hours * car.rentPerhour;
                    break;
                }
            }

            cout << "\nCar Returned Successfully!";
            cout << "\nCustomer: " << name;
            cout << "\nRented On: " << rentDate;
            cout << "\nReturned On: " << returnDate;
            cout << "\nTotal Hours: " << hours;
            cout << "\nTotal Amount: Rs." << total << endl;

            ofstream history("rental_history.txt", ios::app);
            history << id << "," << name << "," << rentDate << "," << returnDate << "," << total << endl;
            history.close();
        }
        else
        {
            temp << line << endl;
        }
    }
    fin.close();
    temp.close();

    remove("active_rentals.txt");
    rename("temp.txt", "active_rentals.txt");

    if (!found)
    {
        cout << "No active rental found for Car ID " << id << " (Car already returned or never rented).\n";
    }
    system("pause");
}

/* ***********************************************
                Rental History
**************************************************/
void rentalHistory()
{
    ifstream fin("rental_history.txt");
    string line;
    cout << "\n--- Rental History ---\n";
    cout << left << setw(8) << "CarID"
         << setw(15) << "Customer"
         << setw(22) << "Rent Date"
         << setw(22) << "Return Date"
         << setw(10) << "Amount"
         << endl;
    cout << string(80, '-') << endl;

    while (getline(fin, line))
    {
        int carID;
        string name, rentDate, returnDate, amount;

        stringstream ss(line);
        string temp;

        getline(ss, temp, ',');
        carID = stoi(temp); // stoi means string to integer conversion
        getline(ss, name, ',');
        getline(ss, rentDate, ',');
        getline(ss, returnDate, ',');
        getline(ss, amount, ',');
        cout << left << setw(8) << carID
             << setw(15) << name
             << setw(22) << rentDate
             << setw(22) << returnDate
             << "Rs." << amount
             << endl;
    }
    fin.close();
    system("pause");
}

/* ***********************************************
                Main Menu of Switch-Case
**************************************************/
void MainMenu()
{
    int choice;
    do
    {
        cout << "\n--- WELCOME TO MY CAR RENTAL PLATFORM ---\n";
        cout << "1. List Cars\n";
        cout << "2. Rent a Car\n";
        cout << "3. Return a Car\n";
        cout << "4. Rental History\n";
        cout << "5. Exit\n";
        cout << "Enter a Choice: ";
        cin >> choice;
        
        /* **************************
             Check invalid input
        *****************************/
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            listCars();
            break;
        case 2:
            rentCar();
            break;
        case 3:
            returnCar();
            break;
        case 4:
            rentalHistory();
            break;
        case 5:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 5);
}

/* ***********************************************
                Login Menu
**************************************************/
bool login()
{
    string user, pass;
    char ch;
    cout << "\n----- Login -----\n";
    cout << "Username: ";
    cin >> user;
    cout << "Password: ";
    pass = "";
    ch = _getch();

    while (ch != 13)
    {
        if (ch == 8){
            if (!pass.empty())
            {
                cout << "\b \b";
                pass.pop_back();
            }
        }else{
            pass.push_back(ch);
            cout << '*';
        }
        ch = _getch();
    }
    cout << endl;
    if (user == "admin" && pass == "123")
    {
        cout << "Login Successful." << endl;
        return true;
    }
    else if (user != "admin" && pass != "123")
    {
        cout << "Both Username and Password are Invalid." << endl;
        return false;
    }
    else if (user != "admin")
    {
        cout << "Invalid Username." << endl;
        return false;
    }
    else
    {
        cout << "Invalid Password." << endl;
        return false;
    }
}
/* ***********************************************
                Main Menu
**************************************************/
int main()
{
    if (!login()) return 0;
    loadCars();
    MainMenu();
    return 0;
}
