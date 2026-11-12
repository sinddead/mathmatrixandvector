#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include "Tmathmatrix.h"

using Matrix = TMatrix<double>;

bool read_matrix(const std::string& name, Matrix& m) {
    std::cout << "Matrix " << name << " (size n first, then n rows of n numbers):\n";
    if (std::cin >> m) return true;
    if (std::cin.eof()) return false;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Input error: expected n >= 0 and n*n numbers.\n";
    return false;
}

int main() {
    std::cout << "Square matrix calculator\n";
    while (true) {
        std::cout << "\nOperation: + (add), - (subtract), * (multiply), q (quit): ";
        std::string op;
        if (!(std::cin >> op) || op == "q" || op == "Q") break;
        if (op != "+" && op != "-" && op != "*") {
            std::cout << "Unknown operation.\n";
            continue;
        }

        Matrix a, b;
        if (!read_matrix("A", a)) {
            if (std::cin.eof()) break;
            continue;
        }
        if (!read_matrix("B", b)) {
            if (std::cin.eof()) break;
            continue;
        }

        try {
            Matrix result = (op == "+") ? a + b : (op == "-") ? a - b : a * b;
            std::cout << "\nA " << op << " B =\n" << result << "\n";
        }
        catch (const std::invalid_argument&) {
            std::cout << "Error: matrices have different sizes.\n";
        }
    }
    std::cout << "Bye!\n";
}