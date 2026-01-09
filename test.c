#include "stdio.h"
#include "lib.h"

int main() {
    struct app_info* app_info;
#ifdef _WIN32
    char* app_path = "C:\\WINDOWS\\system32\\notepad.exe";
#else // POSIX
    char* app_path = "/bin/geany";
#endif

    app_info = launch_app(app_path);
    await_app(app_info);
    return 0;
}