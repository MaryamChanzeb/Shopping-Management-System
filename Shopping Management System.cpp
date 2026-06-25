#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

// ─────────────────────────────────────────────
//  UI HELPERS
// ─────────────────────────────────────────────

void printLine(char ch = '-', int len = 56) {
    cout << string(len, ch) << endl;
}

void printHeader(const string& title) {
    printLine('=');
    int pad = (56 - (int)title.size()) / 2;
    cout << string(pad, ' ') << title << endl;
    printLine('=');
}

void printSection(const string& title) {
    printLine('-');
    cout << "  " << title << endl;
    printLine('-');
}

void printSuccess(const string& msg) { cout << "  [OK] " << msg << endl; }
void printError(const string& msg) { cout << "  [!!] " << msg << endl; }
void printInfo(const string& msg) { cout << "  [>]  " << msg << endl; }

// FIX 1: robust flush — clears EVERYTHING left in the buffer including
// multiple newlines, so getline always reads a fresh line.
void flushInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readLine(const string& prompt) {
    string val;
    cout << prompt;
    flushInput();      // discard anything left from previous cin>>
    getline(cin, val);
    return val;
}

// ─────────────────────────────────────────────
//  CORE FUNCTIONS
// ─────────────────────────────────────────────

void displayCheckoutReceipt(string products[], int cartCount, int cart[][3], int prices[]) {
    printSection("CHECKOUT RECEIPT");
    cout << "  " << left << setw(20) << "Product"
        << setw(10) << "Price"
        << setw(8) << "Qty"
        << "Subtotal" << endl;
  //  printLine('-');
    for (int i = 0; i < cartCount; i++) {
        cout << "  " << left << setw(20) << products[cart[i][0]]
            << "$" << left << setw(9) << cart[i][1]
            << setw(8) << cart[i][2]
            << "$" << cart[i][1] * cart[i][2] << endl;
    }
}

void displayProducts(string products[], int productCount) {
    printSection("AVAILABLE PRODUCTS");
    cout << "  " << left << setw(5) << "No." << "Product" << endl;
    printLine('-');
    for (int i = 0; i < productCount; i++) {
        cout << "  " << left << setw(5) << (i + 1) << products[i] << endl;
    }
}

double calculateTotalPrice(int cartCount, int cart[][3], int prices[]) {
    double total = 0;
    for (int i = 0; i < cartCount; i++)
        total += prices[cart[i][0]] * cart[i][2];
    return total;
}

double applyDiscounts(double totalPrice, double discounts[], int cartCount) {
    double discounted = totalPrice;
    printSection("DISCOUNTS APPLIED");
    for (int i = 0; i < cartCount; i++) {
        double amount = discounts[i] * totalPrice;
        cout << "  Discount " << (i + 1) << ": "
            << (discounts[i] * 100) << "%  (-$"
            << fixed << setprecision(2) << amount << ")" << endl;
        discounted -= amount;
    }
    return discounted;
}

void addToCart(string products[], int productCount, string product, int stock[],
    int& cartCount, int cart[][3], int prices[]) {
    bool found = false;
    for (int i = 0; i < productCount; i++) {
        if (products[i] == product) {
            found = true;
            cout << "  Enter number of units: ";
            int units;
            cin >> units;
            if (units > 0) {
                if (stock[i] >= units) {
                    cart[cartCount][0] = i;
                    cart[cartCount][1] = prices[i];
                    cart[cartCount][2] = units;
                    stock[i] -= units;
                    cartCount++;
                    printSuccess("'" + product + "' x" + to_string(units) + " added to cart.");
                }
                else {
                    printError("Not enough stock for '" + product + "'. Available: " + to_string(stock[i]));
                }
            }
            else {
                printError("Please enter a positive number.");
            }
            break;   // stop searching once found, regardless of outcome
        }
    }
    if (!found)
        printError("'" + product + "' not found in store.");
}

void removeFromCart(string products[], string name, int productCount,
    int& cartCount, int cart[][3], int stock[]) {
    bool found = false;
    for (int i = 0; i < cartCount; i++) {
        if (products[cart[i][0]] == name) {
            found = true;
            stock[cart[i][0]] += cart[i][2];
            cartCount--;
            for (int j = i; j < cartCount; j++) {
                cart[j][0] = cart[j + 1][0];
                cart[j][1] = cart[j + 1][1];
                cart[j][2] = cart[j + 1][2];
            }
            printSuccess("'" + name + "' removed from cart.");
            break;
        }
    }
    if (!found)
        printError("'" + name + "' is not in the cart.");
}

void viewStock(string products[], int productCount, int stock[]) {
    printSection("CURRENT STOCK");
    cout << "  " << left << setw(5) << "No."
        << setw(20) << "Product" << "Stock" << endl;
    printLine('-');
    for (int i = 0; i < productCount; i++) {
        string tag = (stock[i] == 0) ? "  [OUT OF STOCK]" : "";
        cout << "  " << left << setw(5) << (i + 1)
            << setw(20) << products[i]
            << stock[i] << tag << endl;
    }
}

void addStock(string products[], int productCount, int stock[], string name, int units) {
    if (units <= 0) { printError("Units must be a positive number."); return; }
    // FIX 2: moved the "not found" message OUTSIDE the loop so it only
    // prints once after all products have been checked, not on every mismatch.
    for (int i = 0; i < productCount; i++) {
        if (products[i] == name) {
            stock[i] += units;
            printSuccess("Stock updated: '" + name + "' now has " + to_string(stock[i]) + " units.");
            return;
        }
    }
    printError("Product '" + name + "' not found.");
}

bool checkStock(string product, int stock) {
    if (stock > 0) { printInfo("'" + product + "' is in stock (" + to_string(stock) + " units)."); return true; }
    else { printError("'" + product + "' is out of stock."); return false; }
}

void addProduct(string products[], int& productCount, string name, int prices[], int price) {
    if (productCount == 10) {
        printError("Maximum product limit (10) reached.");
    }
    else {
        products[productCount] = name;
        prices[productCount] = price;
        productCount++;
        printSuccess("'" + name + "' added. Total products: " + to_string(productCount));
    }
}

void removeProduct(string products[], int& productCount, string name, int prices[]) {
    for (int i = 0; i < productCount; i++) {
        if (products[i] == name) {
            for (int j = i; j < productCount - 1; j++) {
                products[j] = products[j + 1];
                prices[j] = prices[j + 1];
            }
            productCount--;
            printSuccess("'" + name + "' removed. Total products: " + to_string(productCount));
            return;
        }
    }
    printError("'" + name + "' not found.");
}

void searchProduct(string products[], int productCount, string name) {
    for (int i = 0; i < productCount; i++) {
        if (products[i] == name) { printSuccess("'" + name + "' found in store."); return; }
    }
    printError("'" + name + "' not found in store.");
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────

int main() {
    int    productCount = 0;
    string products[10];
    int    prices[10] = { 0 };
    int    stock[10] = { 0 };
    int    size = sizeof(products) / sizeof(products[0]);

    ifstream file("products.txt");
    if (file.is_open()) {
        while (file >> products[productCount] >> prices[productCount] >> stock[productCount]) {
            productCount++;
            if (productCount >= size) break;
        }
        file.close();
    }

    int    cartCount = 0;
    int    cart[10][3] = { 0 };
    double discounts[3] = { 0.10, 0.05, 0.08 };
    double discountedPrice, totalPrice;
    bool   exitApp = false;

    do {
        cout << "\n";
        printHeader("  SHOP MANAGEMENT SYSTEM  ");
        cout << "\n";
        cout << "  PRODUCT MANAGEMENT\n";
        cout << "    1. Add product\n";
        cout << "    2. Remove product\n";
        cout << "    3. Search product\n";
        cout << "    4. View stock\n";
        cout << "    5. Add stock\n";
        cout << "\n";
        cout << "  ORDER PROCESSING\n";
        cout << "    6. Add to cart\n";
        cout << "    7. Remove from cart\n";
        cout << "    8. View cart\n";
        cout << "    9. Checkout\n";
        cout << "\n";
        cout << "   10. Exit\n\n";
        printLine('=');
        cout << "  Choice: ";

        int choice;
        while (!(cin >> choice)) {
            printError("Invalid input. Enter a number:");
            cout << "  Choice: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout << "\n";

        string name;

        switch (choice) {
        case 1:
            printHeader("ADD PRODUCT");
            name = readLine("  Product name: ");
            {
                cout << "  Price ($): ";
                int price; cin >> price;
                addProduct(products, productCount, name, prices, price);
            }
            break;

        case 2:
            printHeader("REMOVE PRODUCT");
            name = readLine("  Product name: ");
            removeProduct(products, productCount, name, prices);
            break;

        case 3:
            printHeader("SEARCH PRODUCT");
            name = readLine("  Product name: ");
            searchProduct(products, productCount, name);
            break;

        case 4:
            printHeader("STOCK OVERVIEW");
            viewStock(products, productCount, stock);
            break;

        case 5:
            printHeader("ADD STOCK");
            name = readLine("  Product name: ");
            {
                cout << "  Units to add: ";
                int units; cin >> units;
                addStock(products, productCount, stock, name, units);
            }
            break;

        case 6:
            printHeader("ADD TO CART");
            displayProducts(products, productCount);
            name = readLine("\n  Product name: ");
            addToCart(products, productCount, name, stock, cartCount, cart, prices);
            break;

        case 7:
            printHeader("REMOVE FROM CART");
            name = readLine("  Product name: ");
            removeFromCart(products, name, productCount, cartCount, cart, stock);
            break;

        case 8:
            printHeader("YOUR CART");
            if (cartCount == 0) {
                printInfo("Cart is empty.");
            }
            else {
                cout << "  " << left << setw(20) << "Product"
                    << setw(10) << "Price" << setw(8) << "Qty" << "Subtotal" << endl;
               // printLine('-');
                for (int i = 0; i < cartCount; i++) {
                    cout << "  " << left << setw(20) << products[cart[i][0]]
                        << "$" << left << setw(9) << cart[i][1]
                        << setw(8) << cart[i][2]
                        << "$" << cart[i][1] * cart[i][2] << endl;
                }
            }
            break;

        case 9:
            printHeader("CHECKOUT");
            if (cartCount == 0) {
                printInfo("Cart is empty. Nothing to checkout.");
            }
            else {
                displayCheckoutReceipt(products, cartCount, cart, prices);
                totalPrice = calculateTotalPrice(cartCount, cart, prices);
                cout << "\n  Subtotal:  $" << fixed << setprecision(2) << totalPrice << endl;
                discountedPrice = applyDiscounts(totalPrice, discounts, cartCount);
              //  printLine('=');
                cout << "  TOTAL DUE: $" << fixed << setprecision(2) << discountedPrice << endl;
               // printLine('=');
            }
            break;

        case 10:
            printHeader("GOODBYE");
            cout << "  Thank you for shopping with us!\n";
            printLine('=');
            exitApp = true;
            break;

        default:
            printError("Invalid choice. Please enter 1-10.");
            break;
        }

    } while (!exitApp);

    ofstream outfile("products.txt");
    if (outfile.is_open()) {
        for (int i = 0; i < productCount; i++)
            outfile << products[i] << " " << prices[i] << " " << stock[i] << "\n";
        outfile.close();
    }

    return 0;
}