#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include "Scanner.h"

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

    Scanner *scanner = new Scanner(filename);
    Scanner::Token token;

    do {
        token = scanner->getToken();
        if (token.getType() != -1) {
            cout << token;
        }
    } while (token.getType() != errorState);

    scanner->getToken();

    return 0;
}
