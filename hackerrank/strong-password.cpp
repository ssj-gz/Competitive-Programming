// Simon St James (ssjgz) - 2018-02-23
#include <iostream>

using namespace std;

int main()
{
    const char* numbers = "0123456789";
    const char* lowerCase = "abcdefghijklmnopqrstuvwxyz";
    const char* upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char* specialCharacters = "!@#$%^&*()-+";

    int n;
    cin >> n;

    string s;
    cin >> s;

    int passwordWithAllCharTypesLength = s.size();

    if (s.find_first_of(numbers) == string::npos)
        passwordWithAllCharTypesLength++;
    if (s.find_first_of(lowerCase) == string::npos)
        passwordWithAllCharTypesLength++;
    if (s.find_first_of(upperCase) == string::npos)
        passwordWithAllCharTypesLength++;
    if (s.find_first_of(specialCharacters) == string::npos)
        passwordWithAllCharTypesLength++;

    const int minPasswordLength = 6;
    if (passwordWithAllCharTypesLength >= minPasswordLength)
        cout << (passwordWithAllCharTypesLength - s.size()) << endl;
    else
        cout << (minPasswordLength - s.size()) << endl;

}
