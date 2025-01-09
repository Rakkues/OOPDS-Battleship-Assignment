#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    string myText;
    ifstream MyReadFile("game.txt");

    while (getline(MyReadFile, myText))
    {
        cout << myText << endl;
    }

    MyReadFile.close();
}