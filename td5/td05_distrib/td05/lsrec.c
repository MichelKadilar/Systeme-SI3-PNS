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

#define PATH_STORAGE_MAX 1024
#define CURRENT_DIRECTORY (".")

void print_fileinfo(char *file_path, char *filename);

void list(char *filename);

int main(int argc, char *argv[]) {
    if(argc > 1){
        for(int i=1; i<argc;i++){
            list(argv[i]);
            printf("-------------------------------------------------------------------------------------------------\n");
        }
    }
    else {
        list(CURRENT_DIRECTORY);
    }
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
    char *buff = malloc(sizeof(char) * (20 + 1)); // valeur minimum que j'ai trouvée à tâton pour mon format de date
    struct tm *timeinfo = localtime(&time);
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    buff[20] = '\0';
    return buff;
}

void print_fileinfo(char *file_path, char *filename) {
    struct stat *buf = malloc(sizeof(struct stat));
    stat(file_path, buf);
    printf("%s %ld %s %s %ld %s %s\n", get_permissions(*buf), buf->st_nlink, get_file_owner_username(buf->st_uid),
           get_file_groupname(buf->st_gid), buf->st_size, get_date(buf->st_ctime), filename);
};

int count_files_and_subfiles(char *file_path) { // Développé par erreur
    if (is_dir(file_path)) {

        DIR *dir = opendir(file_path);

        if (!dir) return -1;

        struct dirent *d;

        int number_of_files_and_subfiles_of_a_directory = 0;

        while ((d = readdir(dir)) != NULL) {
            number_of_files_and_subfiles_of_a_directory++;
        }
        rewinddir(dir);
        char path[PATH_STORAGE_MAX] = {};
        while ((d = readdir(dir)) != NULL) {
            if (!is_dot_dir(d->d_name)) {
                sprintf(path, "%s/%s", file_path, d->d_name);
                if (is_dir(path)) {
                    number_of_files_and_subfiles_of_a_directory += count_files_and_subfiles(path);
                }
            }
        }
        closedir(dir);
        return number_of_files_and_subfiles_of_a_directory;
    } else return 0;
}


int get_size(char *file_path) {
    struct stat *buf = malloc(sizeof(struct stat));
    stat(file_path, buf);
    return buf->st_size;
}

int get_total_size(char *file_path) {
    if (is_dir(file_path)) {
        DIR *dir = opendir(file_path);
        if (!dir) return -1;
        struct dirent *d;

        int total_size = 0;

        while ((d = readdir(dir)) != NULL) {
            char path[PATH_STORAGE_MAX];
            sprintf(path, "%s/%s", file_path,
                    d->d_name);
            if (!is_dir(d->d_name) || is_dot_dir(d->d_name)) {
                total_size += get_size(path);
            } else {
                printf("TOTO : %s\n", d->d_name);
                total_size += get_total_size(path);
            }
        }
        closedir(dir);
        return total_size;
    } else {
        return get_size(file_path);
    }
}


void list(char *filename) {
    if (is_dir(filename)) {

        DIR *dir = opendir(filename);

        if (!dir) return;

        struct dirent *d;

        printf("%s:\n", filename);
        int directory_size = get_total_size(filename);
        printf("Total : %d\n", directory_size);
        int nb_files_and_subfiles = count_files_and_subfiles(filename);
        printf("Nombre de fichiers et sous-fichiers : %d\n", nb_files_and_subfiles);
        while ((d = readdir(dir)) != NULL) {
            //char path[PATH_STORAGE_MAX];
            //sprintf(path, "%s/%s", filename,
              //      d->d_name); // pour le cas où filename = ./Tests et d->d_name = bar ou foo par exemple
            print_fileinfo(filename, d->d_name);
        }
        printf("\n");
        rewinddir(dir);
        char path[PATH_STORAGE_MAX] = {};
        while ((d = readdir(dir)) != NULL) {
            if (!is_dot_dir(d->d_name)) {
                sprintf(path, "%s/%s", filename, d->d_name);
                if (is_dir(path)) {
                    //nb_files_and_subfiles = count_files_and_subfiles(path);
                    //printf("Total %d\n", nb_files_and_subfiles);
                    list(path);
                }
            }
        }
        closedir(dir);
    } else {

        print_fileinfo(filename, filename);
    }
}