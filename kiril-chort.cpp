#include <iostream>
#include <cmath>    // для fabs()

using namespace std;

int main() {
    double x, eps;          // змінні для x та точності ε
    double a, S;            // поточний член ряду та сума
    int n;                  // лічильник ітерацій

    cout << "Print x: ";
    cin >> x;
    cout << "Print  e: ";
    cin >> eps;

    // Початкові значення
    a = x;      // перший член ряду a1 = x
    S = 0;      // початкова сума
    n = 1;      // лічильник ітерацій

    // Обчислення суми ряду з точністю eps
    while (fabs(a) > eps) {
        S += a;                 // додаємо поточний член
        n++;                    // збільшуємо номер члена
        a = a * x / n;          // рекурентне співвідношення: a_i = a_{i-1} * x / i
    }

    cout << " S in total = " << S << endl;
    cout << "Numberof iteration n = " << n << endl;

    return 0;
}
