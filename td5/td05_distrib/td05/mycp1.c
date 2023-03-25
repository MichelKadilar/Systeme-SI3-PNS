#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define COPY_FILE_IN_FILE 3
#define FIRST_FILE 1
#define BUFFER_SIZE 1024

// copie d'un fichier dans un répertoire ou d'un fichier dans un autre fichier
// Je ne supporte pas la création d'un dossier si jamais il n'existe pas
void copy_file_in_file(char *src_filepath, char *dst_filepath);

// Je ne supporte pas la création d'un dossier si jamais il n'existe pas
// Donc je ne supporte pas la copie du sous-dossier d'un dossier dans un autre dossier
void copy_folder_in_folder(char *src_dirpath, char *dst_dirpath);

int main(int argc, char *argv[]) {
    // 0 : nom du programme, 1 : premier fichier, 2 : deuxième fichier...
    if (argc > 2) { // S'il y a plus que 2 arguments en entrée du programme
        if (argc == COPY_FILE_IN_FILE) {
            if (is_dir(argv[1]) && is_dir(argv[2])) {
                copy_folder_in_folder(argv[1], argv[2]);
            } else if (!is_dir(argv[1])) {
                copy_file_in_file(argv[1], argv[2]);
            } else {
                printf("Opération non supportée\n");
                exit(1);
            }
        } else {
            if (is_dir(argv[1])) {
                printf("Opération non supportée\n");
                exit(1);
            }
            // pas argc -1 car on ne veut pas atteindre le dernier argument qui est le nom du répertoire de destination
            for (int i = FIRST_FILE; i < argc - 2; i++) {
                copy_file_in_file(argv[i], argv[argc - 1]);
            }
        }
    } else {
        printf("Pas assez d'arguments\n");
        exit(1);
    }
}

void copy_folder_in_folder(char *src_dirpath, char *dst_dirpath) {
    DIR *dir = opendir(src_dirpath);
    if (!dir) {
        printf("Problème dans l'ouverture du répertoire\n");
        exit(1);
    }

    struct dirent *d;
    char complete_src_path[BUFFER_SIZE] = {};
    while ((d = readdir(dir)) != NULL) { // on parcourt tous les fichiers du répertoire courant
        sprintf(complete_src_path, "%s/%s", src_dirpath, d->d_name);
        if (is_dir(complete_src_path) || is_dot_dir(complete_src_path)) {
            printf("Tentative de copie d'un dossier dans un autre dossier. Opération non-supportée. Le programme continue.\n");
        }
        else {
            copy_file_in_file(complete_src_path, dst_dirpath);
        }
    }
}


void copy_file_in_file(char *src_filepath, char *dst_path) {
    char complete_dst_path[BUFFER_SIZE] = {};
    int fd_file2 = -1;
    if (is_dir(dst_path)) { // si le chemin de destination est : ./Tests ou ./Tests/ par exemple
        // on concatène avec le NOM du fichier source (toto.txt par exemple), pour obtenir : ./Tests/toto.txt
        sprintf(complete_dst_path, "%s/%s", dst_path, get_basename(src_filepath));

        fd_file2 = open(complete_dst_path,
                        O_RDWR | O_CREAT | O_TRUNC, S_IWGRP); // on accède en écriture au fichier de destination,
        // on s'autorise à créer le fichier s'il n'existe pas et à écraser le contenu du fichier s'il existe,
        // Et on donne le droit d'écriture à tous les utilisateurs membres du groupe (avec le même GID que le fichier mycp1.c)
    } else {
        fd_file2 = open(dst_path,
                        O_RDWR | O_CREAT | O_TRUNC, S_IWGRP); // on accède en écriture au fichier de destination,
        // on s'autorise à créer le fichier s'il n'existe pas et à écraser le contenu du fichier s'il existe,
        // Et on donne le droit d'écriture à tous les utilisateurs membres du groupe (avec le même GID que le fichier mycp1.c)
    }

    if (!file_exists(src_filepath)) {
        printf("Le fichier source au chemin : %s , n'existe pas\n", src_filepath);
    }
    int fd_file1 = open(src_filepath, O_RDONLY);

    if (fd_file1 == -1 || fd_file2 == -1) {
        printf("Problèmes dans l'ouverture du fichier source ou de destination\n");
        exit(1); // Si l'un des deux fichiers n'a pas pu être ouvert, on sort
    }
    int char_read; // Nombre de caractères lus
    int char_written; // Nombre de caractères écrits;
    char buf[BUFFER_SIZE] = {}; // buffer dans lequel on aura les données lues par bloc de 1024 caractères
    while ((char_read = read(fd_file1, buf, BUFFER_SIZE)) > 0) { // tant qu'on lit des caractères
        if ((char_written = write(fd_file2, buf, char_read)) !=
            char_read) { // Si on n'a pas écrit autant de caractères qu'on en a lu
            printf("Erreur : octets lus : %d != octets écrits : %d\n", char_read, char_written);
        }
    }
    close(fd_file2);
    close(fd_file1);
}