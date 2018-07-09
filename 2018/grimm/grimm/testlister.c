#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    char buffer[268];
    strcpy(buffer, argv[1]);
    DIR *pdir;
    struct dirent *pdent;

    buffer[strlen(buffer) - 2] = 0;
    printf("buffer: %s\n", buffer);
    pdir = opendir(buffer);
    if (!pdir) {
        fprintf(stderr, "Cannot open dir\n");
        return 1;
    }

    while (true) {
        pdent = readdir(pdir);
        if (!pdent) {
            break;
        }
        printf("%s\n", pdent->d_name);
    }

    closedir(pdir);
    return 0;
}
