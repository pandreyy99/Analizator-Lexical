#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>

#define GetCurrentDir getcwd
#endif

#include "Scanner.h"
#include <fstream>

using namespace std;

string get_current_dir() {
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

int main() {
    // cout << get_current_dir() << endl;

    char *filename = "../resources/in.txt";
    char *filename_out = "../resources/out.txt";
    ofstream out;
    out.open(filename_out, ios::out);

    Scanner *scanner = new Scanner(filename);
    Scanner::Token token;

    do {
        token = scanner->getToken();
        if (token.getType() != -1) {
            // daca automatul s-a blocat, afisam tokenul
            out << token;
        }
    } while (token.getType() != errorState);

    scanner->getToken();

    return 0;
}
