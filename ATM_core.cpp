#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include <limits>
#include <sstream>


class ATM {
private:
    const long int account_no_;
    std::string name_;
    int pin_;
    double balance_;
    std::string mobile_no_;
    std::vector<std::string> transaction_history_;
    bool is_locked_;
    int pin_attempts_;

    static constexpr int MAX_PIN_ATTEMPTS = 3;
    static constexpr double MAX_WITHDRAWAL = 20000.0;
    static constexpr int LOCKOUT_SECONDS = 30;

    // add timestamp to transactions
    [[nodiscard]] std::string getCurrentTimestamp() const noexcept {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    void addTransaction(const std::string& transaction) {
        transaction_history_.emplace_back("[" + getCurrentTimestamp() + "] " + transaction);
    }

public:
    // Constructor with member initializer list
    ATM(long int account_no, std::string name, int pin, double balance, std::string mobile_no)
        : account_no_(account_no), name_(std::move(name)), pin_(pin),
          balance_(balance), mobile_no_(std::move(mobile_no)),
          is_locked_(false), pin_attempts_(0) {
        transaction_history_.reserve(100); // Pre-allocate for efficiency
    }

    // Delete copy operations (ATM accounts shouldn't be copied)
    ATM(const ATM&) = delete;
    ATM& operator=(const ATM&) = delete;

    // Getters with [[nodiscard]] attribute
    [[nodiscard]] long int getAccountNo() const noexcept { return account_no_; }
    [[nodiscard]] const std::string& getName() const noexcept { return name_; }
    [[nodiscard]] int getPIN() const noexcept { return pin_; }
    [[nodiscard]] double getBalance() const noexcept { return balance_; }
    [[nodiscard]] const std::string& getMobileNo() const noexcept { return mobile_no_; }
    [[nodiscard]] const std::vector<std::string>& getTransactionHistory() const noexcept {
        return transaction_history_;
    }
    [[nodiscard]] bool isLocked() const noexcept { return is_locked_; }
    [[nodiscard]] int getPinAttempts() const noexcept { return pin_attempts_; }

    // Update mobile number with validation
    bool updateMobile(const std::string& old_mobile, const std::string& new_mobile) {
        if (old_mobile != mobile_no_) {
            std::cout << "\nIncorrect old mobile number!\n";
            return false;
        }

        if (new_mobile.length() != 10) {
            std::cout << "\nInvalid mobile number! Must be 10 digits.\n";
            return false;
        }

        mobile_no_ = new_mobile;
        addTransaction("Mobile number updated");
        std::cout << "\nSuccessfully updated mobile number.\n";
        return true;
    }

    // Withdraw cash with proper validation
    bool withdrawCash(double amount) {
        if (amount <= 0) {
            std::cout << "\nInvalid amount! Must be positive.\n";
            return false;
        }

        if (amount > balance_) {
            std::cout << "\nInsufficient balance!\n";
            std::cout << "Available balance: ₹" << std::fixed << std::setprecision(2) << balance_ << "\n";
            return false;
        }

        if (amount > MAX_WITHDRAWAL) {
            std::cout << "\n Withdrawal limit exceeded!\n";
            std::cout << "Maximum withdrawal per transaction: ₹" << MAX_WITHDRAWAL << "\n";
            return false;
        }

        balance_ -= amount;
        std::stringstream ss;
        ss << "Withdrew ₹" << std::fixed << std::setprecision(2) << amount;
        addTransaction(ss.str());

        std::cout << "\n✓ Please collect your cash\n";
        std::cout << "Amount withdrawn: ₹" << std::fixed << std::setprecision(2) << amount << "\n";
        std::cout << "Available balance: ₹" << std::fixed << std::setprecision(2) << balance_ << "\n";
        return true;
    }

    // Deposit cash
    bool depositCash(double amount) {
        if (amount <= 0) {
            std::cout << "\nInvalid amount! Must be positive.\n";
            return false;
        }

        balance_ += amount;
        std::stringstream ss;
        ss << "Deposited ₹" << std::fixed << std::setprecision(2) << amount;
        addTransaction(ss.str());

        std::cout << "\nAmount deposited successfully\n";
        std::cout << "New balance: ₹" << std::fixed << std::setprecision(2) << balance_ << "\n";
        return true;
    }

    void displayBalance() const {
        std::cout << "\n💰 Current Balance: ₹" << std::fixed
                  << std::setprecision(2) << balance_ << "\n";
    }

    void displayUserDetails() const {
        std::cout << "║       USER DETAILS             ║\n";
        std::cout << "Account No : " << account_no_ << "\n";
        std::cout << "Name       : " << name_ << "\n";
        std::cout << "Balance    : ₹" << std::fixed << std::setprecision(2) << balance_ << "\n";
        std::cout << "Mobile     : " << mobile_no_ << "\n";
        std::cout << "Status     : " << (is_locked_ ? "🔒 Locked" : "🔓 Active") << "\n";
    }

    void displayTransactionHistory() const {
        std::cout << "║           TRANSACTION HISTORY                      ║\n";

        if (transaction_history_.empty()) {
            std::cout << "No transactions yet.\n";
        } else {
            for (const auto& transaction : transaction_history_) {
                std::cout << "• " << transaction << "\n";
            }
        }
    }

    void lockAccount() {
        is_locked_ = true;
        addTransaction("Account locked");
        std::cout << "\n Account has been locked for security.\n";
    }

    void unlockAccount() {
        is_locked_ = false;
        pin_attempts_ = 0;
        std::cout << "\n Account unlocked. You may try again.\n";
    }

    void incrementPinAttempts() {
        ++pin_attempts_;
        if (pin_attempts_ >= MAX_PIN_ATTEMPTS) {
            lockAccount();
        }
    }

    void resetPinAttempts() noexcept {
        pin_attempts_ = 0;
    }

    // Static method to wait for lockout period
    static void waitForLockout() {
        std::cout << "\n Please wait " << LOCKOUT_SECONDS << " seconds before retrying...\n";
        std::this_thread::sleep_for(std::chrono::seconds(LOCKOUT_SECONDS));
    }
};

class ATMSystem {
private:
    std::unique_ptr<ATM> user_;

    void clearScreen() const {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void clearInputBuffer() const {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void pauseScreen() const {
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void displayMainMenu() const {
        std::cout << "║       ATM MAIN MENU            ║\n";
        std::cout << "1. Check Balance\n";
        std::cout << "2. Withdraw Cash\n";
        std::cout << "3. Deposit Cash\n";
        std::cout << "4. Show User Details\n";
        std::cout << "5. Update Mobile Number\n";
        std::cout << "6. View Transaction History\n";
        std::cout << "7. Lock Account\n";
        std::cout << "8. Exit\n";
        std::cout << "\nEnter your choice: ";
    }

    bool authenticate() {
        long int account_no;
        int pin;
        std::cout << "║      WELCOME TO ATM            ║\n";
        std::cout << "Enter Account Number: ";

        if (!(std::cin >> account_no)) {
            clearInputBuffer();
            return false;
        }

        std::cout << "Enter PIN: ";
        if (!(std::cin >> pin)) {
            clearInputBuffer();
            return false;
        }

        clearInputBuffer();

        if (account_no == user_->getAccountNo() && pin == user_->getPIN()) {
            user_->resetPinAttempts();
            return true;
        }

        std::cout << "\n Invalid credentials!\n";
        user_->incrementPinAttempts();

        int remaining = 3 - user_->getPinAttempts();
        if (remaining > 0) {
            std::cout << " " << remaining << " attempt(s) remaining.\n";
        }

        return false;
    }

    void handleMainMenu() {
        int choice;

        while (true) {
            if (user_->isLocked()) {
                std::cout << "\n Account is locked. Please contact support.\n";
                pauseScreen();
                return;
            }

            clearScreen();
            displayMainMenu();

            if (!(std::cin >> choice)) {
                clearInputBuffer();
                std::cout << "\n Invalid input! Please enter a number.\n";
                pauseScreen();
                clearInputBuffer();
                continue;
            }

            clearInputBuffer();

            switch (choice) {
                case 1: {
                    user_->displayBalance();
                    pauseScreen();
                    break;
                }
                case 2: {
                    double amount;
                    std::cout << "\nEnter withdrawal amount: ₹";
                    if (std::cin >> amount) {
                        user_->withdrawCash(amount);
                    } else {
                        std::cout << "\n Invalid amount!\n";
                    }
                    clearInputBuffer();
                    pauseScreen();
                    break;
                }
                case 3: {
                    double amount;
                    std::cout << "\nEnter deposit amount: ₹";
                    if (std::cin >> amount) {
                        user_->depositCash(amount);
                    } else {
                        std::cout << "\n Invalid amount!\n";
                    }
                    clearInputBuffer();
                    pauseScreen();
                    break;
                }
                case 4: {
                    user_->displayUserDetails();
                    pauseScreen();
                    break;
                }
                case 5: {
                    std::string old_mobile, new_mobile;
                    std::cout << "\nEnter old mobile number: ";
                    std::cin >> old_mobile;
                    std::cout << "Enter new mobile number: ";
                    std::cin >> new_mobile;
                    user_->updateMobile(old_mobile, new_mobile);
                    clearInputBuffer();
                    pauseScreen();
                    break;
                }
                case 6: {
                    user_->displayTransactionHistory();
                    pauseScreen();
                    break;
                }
                case 7: {
                    user_->lockAccount();
                    pauseScreen();
                    return;
                }
                case 8: {
                    std::cout << "\n Thank you for using our ATM. Goodbye!\n";
                    return;
                }
                default: {
                    std::cout << "\n Invalid choice! Please select 1-8.\n";
                    pauseScreen();
                    break;
                }
            }
        }
    }

public:
    ATMSystem() {
        // Initialize with sample user data
        user_ = std::make_unique<ATM>(987654321, "Hardik", 1234, 50000.0, "9370054900");
    }

    void run() {
        while (true) {
            clearScreen();

            if (!authenticate()) {
                if (user_->isLocked()) {
                    char retry;
                    std::cout << "\nWould you like to try again? (y/n): ";
                    std::cin >> retry;
                    clearInputBuffer();

                    if (retry == 'y' || retry == 'Y') {
                        ATM::waitForLockout();
                        user_->unlockAccount();
                        continue;
                    } else {
                        std::cout << "\n Goodbye!\n";
                        break;
                    }
                }
                pauseScreen();
                clearInputBuffer();
                continue;
            }

            handleMainMenu();

            char continue_session;
            std::cout << "\n\nStart new session? (y/n): ";
            std::cin >> continue_session;
            clearInputBuffer();

            if (continue_session != 'y' && continue_session != 'Y') {
                std::cout << "\n Thank you for using our ATM. Goodbye!\n";
                break;
            }
        }
    }
};

int main() {
    try {
        ATMSystem atm;
        atm.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
