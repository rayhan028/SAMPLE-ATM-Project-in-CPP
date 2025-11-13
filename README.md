# SAMPLE-ATM-Project-in-CPP

A sleek, command-line ATM simulator built with modern C++17 featuring smart pointers, move semantics, and professional banking operations.

## Features
Secure Authentication - PIN-based login with lockout mechanism
Cash Operations - Withdraw and deposit with transaction limits
Transaction History - Timestamped records of all activities
Account Management - Update mobile number and view details
Security Features - Auto-lock after failed attempts, manual account lock
Clean UI - Formatted output with symbols and visual separators

# Clone the repository
git clone https://github.com/yourusername/atm-system.git
cd atm-system

# Compile
g++ -std=c++17 ATM_core.cpp -o atm

# Run
./atm
# Demo Credentials
Account Number: 987654321
PIN: 1234
Initial Balance: 50,000

# Requirements
C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
CMake 3.26+ (for CLion/CMake builds)
