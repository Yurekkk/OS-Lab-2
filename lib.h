#include "stddef.h"
#include "stdio.h"

#ifdef _WIN32
    #include "windows.h"
#endif

struct app_info {
#ifdef _WIN32
    HANDLE hProcess;
#else // POSIX
    unsigned int pid;
#endif
};

struct app_info* launch_app(char* ApplicationName) {

#ifdef _WIN32

    // Всякая разная информация
    STARTUPINFOA si = {0};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    // Запускаем программу
    WINBOOL success = CreateProcessA(
        ApplicationName,
        NULL,       // lpCommandLine
        NULL,       // lpProcessAttributes,
        NULL,       // lpThreadAttributes,
        FALSE,      // bInheritHandles,
        0,          // dwCreationFlags,
        NULL,       // lpEnvironment,
        NULL,       // lpCurrentDirectory,
        &si,        // lpStartupInfo,
        &pi         // lpProcessInformation
    );

    if (!success) {
        printf("Application did not open.\n");
        return NULL;
    }

    // Поток не нужен, закрываем его дескриптор
    CloseHandle(pi.hThread);

    // Возвращаем указатель на процесс
    struct app_info* info = malloc(sizeof(struct app_info));
    info->hProcess = pi.hProcess;
    return info;

#else // POSIX

    // Создаем дочерний процесс
    pid_t pid = fork();

    if (pid == 0) {
        // Дочерний процесс
        char* const argv[] = {ApplicationName, NULL};
        execv(ApplicationName, argv);

        // Если execv успешен — дочерний 
        // процесс больше не выполняет код.

        // Если execv неуспешен
        _exit(127);
    } 
    else if (pid > 0) {
        // Родительский процесс
        struct app_info* info = malloc(sizeof(struct app_info));
        info->pid = pid;
        return info;
    } else {
        // fork не сработал
        return NULL;
    }

#endif
}   

void await_app(struct app_info* app_info) {

#ifdef _WIN32

    // Ожидаем завершения процесса
    const unsigned long awaitTime = INFINITE;
    WaitForSingleObject(app_info->hProcess, awaitTime);

    // Получаем код возврата
    DWORD exitCode;
    GetExitCodeProcess(app_info->hProcess, &exitCode);
    printf("Application exited with code: %lu\n", exitCode);

    // Закрываем процесс
    CloseHandle(app_info->hProcess);


#else // POSIX

    int status;
    process_id_t result = waitpid(app_info->pid, &status, 0);

    if (result == -1) {
        perror("waitpid failed");
        free(app_info);
        return;
    }

    int exit_code = -1;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        exit_code = -WTERMSIG(status);  // отрицательный — убит сигналом
    }

    printf("Application exited with code: %d\n", exit_code);

#endif

    free(app_info);

}