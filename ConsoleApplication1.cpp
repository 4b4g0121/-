#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>

using namespace std;

class User {
protected:
    string name;
    string memberType;

public:
    User(string n = "", string type = "Normal") : name(n), memberType(type) {}
    virtual ~User() {}

    virtual double getDiscount() = 0;
    virtual void display() = 0;

    string getName() const { return name; }
    string getMemberType() const { return memberType; }
};

class NormalMember : public User {
public:
    NormalMember(string n = "") : User(n, "Normal") {}

    double getDiscount() override { return 1.0; }

    void display() override {
        cout << "一般會員 : " << name << " (無折扣)" << endl;
    }
};

class VIPMember : public User {
public:
    VIPMember(string n = "") : User(n, "VIP") {}

    double getDiscount() override { return 0.8; }

    void display() override {
        cout << "VIP 會員 : " << name << " (享 8 折優惠!)" << endl;
    }
};

class Movie {
private:
    string movieName;
    string category;
    int duration;
    double basePrice;

public:
    Movie() : duration(0), basePrice(280) {}
    Movie(string m, string c, int d, double price = 280)
        : movieName(m), category(c), duration(d), basePrice(price) {
    }

    string getMovieName() const { return movieName; }
    string getCategory() const { return category; }
    int getDuration() const { return duration; }
    double getBasePrice() const { return basePrice; }

    void displayMovie() const {
        cout << left
            << setw(25) << movieName
            << setw(15) << category
            << setw(12) << to_string(duration) + " 分鐘"
            << "$" << basePrice << endl;
    }
};

class Ticket {
private:
    string customerName;
    string movieName;
    string seat;
    double finalPrice;

public:
    Ticket() : finalPrice(0) {}
    Ticket(string c, string m, string s, double price)
        : customerName(c), movieName(m), seat(s), finalPrice(price) {
    }

    string getCustomerName() const { return customerName; }
    string getMovieName() const { return movieName; }
    string getSeat() const { return seat; }
    double getFinalPrice() const { return finalPrice; }

    void displayTicket() const {
        cout << "消費者: " << left << setw(12) << customerName
            << " | 電影: " << setw(20) << movieName
            << " | 座位: " << setw(6) << seat
            << " | 票價: $" << fixed << setprecision(0) << finalPrice << endl;
    }
};

class CinemaSystem {
private:
    vector<Movie> movies;
    vector<Ticket> tickets;
    map<string, shared_ptr<User>> members;

public:
    void registerMember(const string& name, int type) {
        if (type == 2) {
            members[name] = make_shared<VIPMember>(name);
        }
        else {
            members[name] = make_shared<NormalMember>(name);
        }
    }

    void loadData() {
        ifstream fin_m("movies.txt");
        if (fin_m) {
            string name, category, dur_str, price_str;
            while (getline(fin_m, name, ',') &&
                getline(fin_m, category, ',') &&
                getline(fin_m, dur_str, ',') &&
                getline(fin_m, price_str)) {
                if (!name.empty()) {
                    movies.push_back(Movie(name, category, stoi(dur_str), stod(price_str)));
                }
            }
            fin_m.close();
        }

        ifstream fin_mem("members.txt");
        if (fin_mem) {
            string name, type;
            while (getline(fin_mem, name, ',') && getline(fin_mem, type)) {
                if (!name.empty()) {
                    if (type == "VIP") members[name] = make_shared<VIPMember>(name);
                    else members[name] = make_shared<NormalMember>(name);
                }
            }
            fin_mem.close();
        }

        ifstream fin_t("tickets.txt");
        if (fin_t) {
            string customer, movie, seat, price_str;
            while (getline(fin_t, customer, ',') &&
                getline(fin_t, movie, ',') &&
                getline(fin_t, seat, ',') &&
                getline(fin_t, price_str)) {
                if (!customer.empty()) {
                    tickets.push_back(Ticket(customer, movie, seat, stod(price_str)));
                }
            }
            fin_t.close();
        }
    }

    void saveData() {
        ofstream fout_m("movies.txt");
        for (const auto& m : movies) {
            fout_m << m.getMovieName() << "," << m.getCategory() << ","
                << m.getDuration() << "," << m.getBasePrice() << "\n";
        }
        fout_m.close();

        ofstream fout_mem("members.txt");
        for (const auto& pair : members) {
            fout_mem << pair.second->getName() << "," << pair.second->getMemberType() << "\n";
        }
        fout_mem.close();

        ofstream fout_t("tickets.txt");
        for (const auto& t : tickets) {
            fout_t << t.getCustomerName() << "," << t.getMovieName() << ","
                << t.getSeat() << "," << t.getFinalPrice() << "\n";
        }
        fout_t.close();
    }

    void addMovie() {
        string name, category;
        int duration;
        double price;

        cout << "\n請輸入電影名稱: ";
        cin.ignore();
        getline(cin, name);
        cout << "電影類型: ";
        getline(cin, category);
        cout << "片長 (分鐘): ";
        cin >> duration;
        cout << "基本票價: ";
        cin >> price;

        movies.push_back(Movie(name, category, duration, price));
        cout << "\n>>> 電影新增成功！ <<<\n";
    }

    void showMovies() const {
        cout << "\n====================== 電影清單 ======================\n";
        cout << left << setw(6) << "編號" << setw(25) << "電影名稱" << setw(15) << "類型" << setw(12) << "片長" << "票價\n";
        cout << "--------------------------------------------------------\n";
        for (size_t i = 0; i < movies.size(); ++i) {
            cout << left << setw(6) << i + 1;
            movies[i].displayMovie();
        }
    }

    void bookTicket() {
        if (movies.empty()) {
            cout << "\n目前沒有電影可供訂票。\n";
            return;
        }

        string customer;
        int choice, memType;
        string seat;

        cout << "\n請輸入顧客姓名: ";
        cin.ignore();
        getline(cin, customer);

        if (members.find(customer) == members.end()) {
            cout << "此顧客尚未註冊身分，請選擇：1. 一般會員  2. VIP會員。請輸入選擇: ";
            cin >> memType;
            registerMember(customer, memType);
        }

        cout << "\n[目前會員身分] ";
        members[customer]->display();

        showMovies();
        cout << "\n請輸入欲觀看的電影編號: ";
        cin >> choice;

        if (choice < 1 || choice > static_cast<int>(movies.size())) {
            cout << "\n無效的編號！\n";
            return;
        }

        cout << "請輸入座位號碼 (例如 A12): ";
        cin >> seat;

        double discount = members[customer]->getDiscount();
        double finalPrice = movies[choice - 1].getBasePrice() * discount;

        tickets.push_back(Ticket(customer, movies[choice - 1].getMovieName(), seat, finalPrice));

        cout << "\n>>> 訂票成功！最終票價為: $" << finalPrice << " <<<\n";
    }

    void cancelTicket() {
        string customer;
        cout << "\n請輸入要退票的顧客姓名: ";
        cin.ignore();
        getline(cin, customer);

        bool found = false;
        for (auto it = tickets.begin(); it != tickets.end(); ) {
            if (it->getCustomerName() == customer) {
                it = tickets.erase(it);
                found = true;
            }
            else {
                ++it;
            }
        }

        if (found) {
            cout << "\n>>> 已成功取消 " << customer << " 的所有訂票紀錄。 <<<\n";
        }
        else {
            cout << "\n找不到該顧客的訂票紀錄。\n";
        }
    }

    void showTickets() const {
        cout << "\n====================== 訂票紀錄清單 ======================\n";
        if (tickets.empty()) {
            cout << "目前沒有任何訂票紀錄。\n";
            return;
        }
        for (const auto& t : tickets) {
            t.displayTicket();
        }
    }
};

int main() {
    // 強制將 Windows 終端機編碼改為 UTF-8，防止中文顯示亂碼
    system("chcp 950 > nul");
    CinemaSystem system;
    system.loadData();

    int choice;
    do {
        cout << "\n====================================\n";
        cout << "       電影院影城訂票管理系統\n";
        cout << "====================================\n";
        cout << "1. 新增電影資訊\n";
        cout << "2. 瀏覽所有電影\n";
        cout << "3. 現場顧客訂票 (享會員折扣)\n";
        cout << "4. 取消訂票 (退票)\n";
        cout << "5. 瀏覽所有訂票紀錄\n";
        cout << "6. 儲存當前資料\n";
        cout << "0. 離開系統\n";
        cout << "------------------------------------\n";
        cout << "請輸入您的選擇: ";
        cin >> choice;

        switch (choice) {
        case 1: system.addMovie(); break;
        case 2: system.showMovies(); break;
        case 3: system.bookTicket(); break;
        case 4: system.cancelTicket(); break;
        case 5: system.showTickets(); break;
        case 6:
            system.saveData();
            cout << "\n資料已成功儲存。\n";
            break;
        case 0:
            system.saveData();
            cout << "\n感謝使用本系統，再見！\n";
            break;
        default:
            cout << "\n無效的選項！請重新輸入。\n";
        }
    } while (choice != 0);

    return 0;
}