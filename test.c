#include "stdio.h"
#include "lib.h"

int main() {
    struct app_info* app_info;
    app_info = launch_app("C:\\WINDOWS\\system32\\notepad.exe");
    await_app(app_info);
    return 0;
}