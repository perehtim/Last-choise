#include <iostream>
#include <cmath>
using namespace std;

int main() {
    // Вхідні дані
    double h, r, t, m, g = 9.81;

    cout << "type h : ";
    cin >> h;
    cout << "type r : ";
    cin >> r;
    cout << "type t : ";
    cin >> t;
    cout << "type m : ";
    cin >> m;

    // Крок 1: Знайдемо прискорення a
    double a = (2 * h) / (t * t);
    cout << "\n1. found a = 2h / t^2 = " << a << " м/с^2" << endl;

    // Крок 2: Обчислюємо β = a / r
    double beta = a / r;
    cout << "2. found β = a / r = " << beta << " 1/с^2" << endl;

    // Крок 3: Обчислюємо момент сили M = m(g - a)r
    double M = m * (g - a) * r;
    cout << "3. found M = m(g - a)r = " << M << " Н·м" << endl;

    cout << "\n--- result ---" << endl;
    cout << "a = " << a << " м/с^2" << endl;
    cout << "β = " << beta << " 1/с^2" << endl;
    cout << "M = " << M << " Н·м" << endl;

    return 0;
}
