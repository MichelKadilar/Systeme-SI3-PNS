#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define PATH_STORAGE_MAX 500

void print_fileinfo(char *filename);

void list(char *filename);

int main(int argc, char *argv[]) {
    // A Changer, on peut avoir plusieurs paramètres en entrée du programme.
    /*if(argc > 2) printf("Trop d'arguments donnés en paramètre du programme lsrec");
    if(argc > 1){

    }
    else {

    }*/

    list(argv[1]);

}

char *get_permissions(struct stat buf) {
    char *s = malloc(sizeof(char) * (10 + 1)); // +1 pour le \0

    if (S_ISDIR(buf.st_mode)) s[0] = 'd'; else s[0] = '-';
    if (buf.st_mode & S_IRUSR) s[1] = 'r'; else s[1] = '-';
    if (buf.st_mode & S_IWUSR) s[2] = 'w'; else s[2] = '-';
    if (buf.st_mode & S_IXUSR) s[3] = 'x'; else s[3] = '-';
    if (buf.st_mode & S_IRGRP) s[4] = 'r'; else s[4] = '-';
    if (buf.st_mode & S_IWGRP) s[5] = 'w'; else s[5] = '-';
    if (buf.st_mode & S_IXGRP) s[6] = 'x'; else s[6] = '-';
    if (buf.st_mode & S_IROTH) s[7] = 'r'; else s[7] = '-';
    if (buf.st_mode & S_IWOTH) s[8] = 'w'; else s[8] = '-';
    if (buf.st_mode & S_IXOTH) s[9] = 'x'; else s[9] = '-';
    s[10] = '\0';
    return s;
}

char *get_file_owner_username(short uid) {
    struct passwd *pws = getpwuid(uid);
    return pws->pw_name;
}

char *get_file_groupname(short uid) {
    struct group *grp = getgrgid(uid);
    return grp->gr_name;
}

char *get_date(time_t time) {
    char *buff = malloc(sizeof(char) * (13+1)); // valeur minimum que j'ai trouvée à tâton pour mon format de date
    struct tm *timeinfo = localtime(&time);
    strftime(buff, 13, "%b %d %H:%M", timeinfo);
    buff[13] = '\0';
    return buff;
}

void print_fileinfo(char *filename) {
    int fd = open(filename, O_RDONLY);
    struct stat *buf = malloc(sizeof(struct stat));
    stat(filename, buf);
    printf("%s %ld %s %s %ld %s %s\n", get_permissions(*buf), buf->st_nlink, get_file_owner_username(buf->st_uid),
           get_file_groupname(buf->st_gid), buf->st_size, get_date(buf->st_ctime), filename);
    close(fd);
};


void list(char *filename) {
    if (is_dir(filename)) {

        struct dirent *d;
        DIR *dir = opendir(filename);
        printf("%s:\n", filename);
        if (dir) {
            while ((d = readdir(dir)) != NULL) {
                printf("%s\t", d->d_name);
            }
            printf("\n");
            rewinddir(dir);
            while ((d = readdir(dir)) != NULL) {
                if (!is_dot_dir(d->d_name)) {
                    char name[PATH_STORAGE_MAX];
                    sprintf(name, "%s/%s", filename, d->d_name);
                    list(name);
                }
            }
            closedir(dir);
        } else {
            printf("Erreur dans l'ouverture du repertoire : %s\n", filename);
        }
    } else {
        print_fileinfo(filename);
    }
}