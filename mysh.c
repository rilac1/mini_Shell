/* My shell by KangJeonghyun (rilac1@naver.com) */
/* Dankook Univ, 2020/10/30 */
#include <sys/types.h>
#include <unistd.h>     //getcwd
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // strtok
#include <fcntl.h>      // O_RDWR, O_CREAT

int tokenizer(char* command, char* argv[]) {
    char* token;
    int tokenIndex = 0;
    command[strlen(command) - 1] = '\0';        // '\n' 지우기

    token = strtok(command, " ");
    for(; token != NULL; tokenIndex++){
        argv[tokenIndex] = token;
        token = strtok(NULL, " ");
    }
    argv[tokenIndex] = NULL;
    return tokenIndex;
}

int in_cd(char* argv[]){
    if(argv[1] == NULL)
        chdir(getenv("HOME"));
    else {
        if(chdir(argv[1]))
            printf("bash: cd: %s: No such file or directory\n", argv[1]);
    }
    return 0;
}

int in_help() {
    printf("cd      : change directory\n");
    printf("exit    : exit this shell\n");
    printf("quit    : quit this shell\n");
    printf("help    : show this help\n");
    printf("?       : show this help\n");
    return 0;
}

// 명령어 실행 함수 (return 값이 0이면 계속, 1이면 중단)
int execute(char* command) {
    char* argv[64];
    int argc = tokenizer(command, argv);
    pid_t fork_return;
    int backGround = 0;

    // 백그라운드 실행 여부 조사
    if (*argv[argc-1] == '&') {    
        backGround = 1;
        argv[argc-1] == '\0';
    }

    // 내부 명령어 실행
    if ((strcmp(argv[0], "exit") * strcmp(argv[0], "quit")) == 0)  // exit, quit
        return 1;
    else if (strcmp(argv[0], "cd") == 0)                           // cd
        return in_cd(argv);
    else if (strcmp(argv[0], "help")==0 || *argv[0] == '?')        // help, ?
        return in_help();

    // 외부 명령어 실행
    if ((fork_return = fork()) < 0) {
        perror("fork error"); exit(1);
    } 
    else if (fork_return == 0) {
        if (argc>1 && *argv[argc-2] == '>') {                      // 리다이렉션 구현
            dup2(open(argv[argc-1], O_RDWR | O_CREAT, 0641), STDOUT_FILENO); 
            argv[argc-2] = '\0';
        }
        execvp(argv[0], argv);
        printf("%s: command not found\n", argv[0]);
    } 
    else if(!backGround){
        wait();
    }
    return 0;
}

int main() {
    char command[1024];
    while(1){
        printf("%s$ ",	getcwd(command, 1024));
        fgets(command, sizeof(command), stdin);
        if(execute(command))
            break;
    }
}
