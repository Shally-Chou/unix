#include <iostream>
#include "myfstream.h" // 引用您寫的函式庫

using namespace std;

int main() {
    myfstream file;
    string word;

    cout << "Attempting to open input.dat..." << endl;

    if (file.Open("input.dat")) {
        cout << "File opened successfully. Reading contents:" << endl;
        
        while (file >> word) {
            cout << "Read word: " << word << endl;
        }
        
        file.Close();
        cout << "File closed." << endl;
    } else {
        cerr << "Error: Could not open input.dat" << endl;
        return 1; // 返回錯誤碼
    }

    return 0;
}
