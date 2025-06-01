#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define DOSSIER "../images/"
#define JPEG2PPM "../jpeg2ppm"
#define REPETITIONS 5

// Vérifie si l'extension est .jpg ou .jpeg (insensible à la casse)
int a_extension_jpeg(const char *nom) {
    const char *ext = strrchr(nom, '.');
    return ext && (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0);
}

// Vérifie si le fichier est régulier
int est_fichier_regulier(const char *chemin) {
    struct stat s;
    return stat(chemin, &s) == 0 && S_ISREG(s.st_mode);
}

// Mesure le temps moyen d'exécution d'une commande
double mesurer_temps_exec(char *const args[]) {
    double total_ms = 0.0;

    for (int i = 0; i < REPETITIONS; i++) {
        struct timeval debut, fin;
        gettimeofday(&debut, NULL);

        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("Erreur exec");
            exit(EXIT_FAILURE);
        } else {
            int status;
            wait(&status);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                return -1.0;
            }
        }

        gettimeofday(&fin, NULL);
        double ms = (fin.tv_sec - debut.tv_sec) * 1000.0 +
                    (fin.tv_usec - debut.tv_usec) / 1000.0;
        total_ms += ms;
    }

    return total_ms / REPETITIONS;
}

int main(void) {
    // Affiche le répertoire courant
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Répertoire courant : %s\n\n", cwd);
    } else {
        perror("Erreur getcwd()");
    }

    // Vérifie que jpeg2ppm est présent
    if (access(JPEG2PPM, X_OK) != 0) {
        fprintf(stderr, "Erreur : l'exécutable %s est introuvable ou non exécutable.\n", JPEG2PPM);
        return EXIT_FAILURE;
    }

    DIR *rep = opendir(DOSSIER);
    if (!rep) {
        perror("Erreur lors de l'ouverture du dossier images/");
        return EXIT_FAILURE;
    }

    struct dirent *ent;
    while ((ent = readdir(rep)) != NULL) {
        char chemin[512];
        snprintf(chemin, sizeof(chemin), "%s%s", DOSSIER, ent->d_name);

        if (a_extension_jpeg(ent->d_name) && est_fichier_regulier(chemin)) {
            printf("Image : %s\n", chemin);

            // Appel rapide
            char *args_rapide[] = {JPEG2PPM, chemin, NULL};
            double temps_rapide = mesurer_temps_exec(args_rapide);
            if (temps_rapide < 0) {
                printf("  Erreur : traitement rapide impossible\n\n");
                continue;
            }

            // Appel classique
            char *args_classique[] = {JPEG2PPM, chemin, "--classic", NULL};
            double temps_classique = mesurer_temps_exec(args_classique);
            if (temps_classique < 0) {
                printf("  Erreur : traitement classique impossible\n\n");
                continue;
            }

            printf("  Temps rapide   : %.3f ms (moyenne sur %d)\n", temps_rapide, REPETITIONS);
            printf("  Temps classique: %.3f ms (moyenne sur %d)\n\n", temps_classique, REPETITIONS);
        }
    }

    closedir(rep);
    return 0;
}