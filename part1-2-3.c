#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ETATS 100
#define MAX_TRANSITIONS 100

// Définir les couleurs ANSI
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
    int id;
    int estInitial;
    int estFinal;
} Etat;

typedef struct {
    int etatDepart;
    char symbole;
    int etatArrivee;
} Transition;

typedef struct {
    int nbEtats;
    int nbTransitions;
    Etat etats[MAX_ETATS];
    Transition transitions[MAX_TRANSITIONS];
} Automate;

// Fonction pour ajouter un automate manuellement
void ajouterAutomate(Automate *automate) {
    printf(ANSI_COLOR_CYAN "Entrer le Nombre d'états : " ANSI_COLOR_RESET);
    scanf("%d", &automate->nbEtats);

    for (int i = 0; i < automate->nbEtats; i++) {
        printf(ANSI_COLOR_CYAN "Libellé de l'état %d : " ANSI_COLOR_RESET, i + 1);
        scanf("%d", &automate->etats[i].id);
        printf(ANSI_COLOR_CYAN "Est-ce un état initial ? (1: Oui, 0: Non) : " ANSI_COLOR_RESET);
        scanf("%d", &automate->etats[i].estInitial);
        printf(ANSI_COLOR_CYAN "Est-ce un état final ? (1: Oui, 0: Non) : " ANSI_COLOR_RESET);
        scanf("%d", &automate->etats[i].estFinal);
    }

    printf(ANSI_COLOR_CYAN "Entrer le Nombre de transitions : " ANSI_COLOR_RESET);
    scanf("%d", &automate->nbTransitions);

    for (int i = 0; i < automate->nbTransitions; i++) {
        printf(ANSI_COLOR_CYAN "État de départ : " ANSI_COLOR_RESET);
        scanf("%d", &automate->transitions[i].etatDepart);

        printf(ANSI_COLOR_CYAN "Donner l'étiquette : " ANSI_COLOR_RESET);
        scanf(" %c", &automate->transitions[i].symbole); // Ajout d'un espace avant %c

        printf(ANSI_COLOR_CYAN "État d'arrivée : " ANSI_COLOR_RESET);
        scanf("%d", &automate->transitions[i].etatArrivee);
    }
}

// Fonction pour afficher un automate
void afficherAutomate(const Automate *automate) {
    printf(ANSI_COLOR_YELLOW "\n États :\n" ANSI_COLOR_RESET);
    for (int i = 0; i < automate->nbEtats; i++) {
        printf(ANSI_COLOR_YELLOW "État %d (Initial: %d, Final: %d)\n" ANSI_COLOR_RESET,
               automate->etats[i].id,
               automate->etats[i].estInitial,
               automate->etats[i].estFinal);
    }

    printf(ANSI_COLOR_YELLOW "\nTransitions :\n" ANSI_COLOR_RESET);
    for (int i = 0; i < automate->nbTransitions; i++) {
        printf(ANSI_COLOR_YELLOW "%d --(%c)--> %d\n" ANSI_COLOR_RESET,
               automate->transitions[i].etatDepart,
               automate->transitions[i].symbole,
               automate->transitions[i].etatArrivee);
    }
}

// Fonction pour enregistrer un automate dans un fichier .dot
void enregistrerAutomateDot(const Automate *automate, const char *nomFichier) {
    FILE *f = fopen(nomFichier, "w");
    if (!f) {
        printf(ANSI_COLOR_RED "Erreur lors de l'ouverture du fichier.\n" ANSI_COLOR_RESET);
        return;
    }

    fprintf(f, "digraph Automate {\n");

    // Écrire les états
    for (int i = 0; i < automate->nbEtats; i++) {
        fprintf(f, "  %d", automate->etats[i].id);
        if (automate->etats[i].estInitial)
            fprintf(f, " [shape=doublecircle]");
        if (automate->etats[i].estFinal)
            fprintf(f, " [peripheries=2]"); // Double contour pour les états finaux
        fprintf(f, ";\n");
    }

    // Écrire les transitions
    for (int i = 0; i < automate->nbTransitions; i++) {
        fprintf(f, "  %d -> %d [label=\"%c\"];\n", // Utilisation de "label" au lieu de "etiquette"
                automate->transitions[i].etatDepart,
                automate->transitions[i].etatArrivee,
                automate->transitions[i].symbole);
    }

    fprintf(f, "}\n");
    fclose(f);
    printf(ANSI_COLOR_GREEN "Automate enregistré dans %s avec succès\n" ANSI_COLOR_RESET, nomFichier);
}

// Fonction pour charger un automate depuis un fichier .dot
void chargerAutomateDot(Automate *automate, const char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    if (!f) {
        printf(ANSI_COLOR_RED "Erreur lors de l'ouverture du fichier.\n" ANSI_COLOR_RESET);
        return;
    }

    automate->nbEtats = 0;
    automate->nbTransitions = 0;

    char ligne[200];
    while (fgets(ligne, sizeof(ligne), f)) {
        int etat1, etat2;
        char symbole;

        // Lire une transition avec l'attribut "label"
        if (sscanf(ligne, " %d -> %d [label=\"%c\"]", &etat1, &etat2, &symbole) == 3) {
            automate->transitions[automate->nbTransitions].etatDepart = etat1;
            automate->transitions[automate->nbTransitions].etatArrivee = etat2;
            automate->transitions[automate->nbTransitions].symbole = symbole;
            automate->nbTransitions++;
        }

        // Lire un état (initial ou final)
        else if (sscanf(ligne, " %d [shape=doublecircle]", &etat1) == 1 ||
                 sscanf(ligne, " %d [peripheries=2]", &etat1) == 1) {
            int estFinal = (strstr(ligne, "peripheries=2") != NULL) ? 1 : 0;
            int estInitial = (strstr(ligne, "shape=doublecircle") != NULL) ? 1 : 0;

            automate->etats[automate->nbEtats].id = etat1;
            automate->etats[automate->nbEtats].estInitial = estInitial;
            automate->etats[automate->nbEtats].estFinal = estFinal;
            automate->nbEtats++;
        }

        // Lire un état sans indication initiale/finale
        else if (sscanf(ligne, " %d ;", &etat1) == 1) {
            automate->etats[automate->nbEtats].id = etat1;
            automate->etats[automate->nbEtats].estInitial = 0;
            automate->etats[automate->nbEtats].estFinal = 0;
            automate->nbEtats++;
        }
    }

    fclose(f);
    printf(ANSI_COLOR_GREEN "Automate chargé depuis %s avec succès\n" ANSI_COLOR_RESET, nomFichier);
}

// Fonction récursive pour explorer tous les chemins possibles
int explorerChemins(const Automate *automate, int etatCourant, const char *mot, int index) {
    // Si on a parcouru tout le mot, vérifier si l'état courant est final
    if (mot[index] == '\0') {
        for (int i = 0; i < automate->nbEtats; i++) {
            if (automate->etats[i].id == etatCourant && automate->etats[i].estFinal) {
                return 1; // Le mot est accepté
            }
        }
        return 0; // Le mot est rejeté
    }

    // Parcourir toutes les transitions pour trouver les états suivants possibles
    for (int i = 0; i < automate->nbTransitions; i++) {
        if (automate->transitions[i].etatDepart == etatCourant &&
            automate->transitions[i].symbole == mot[index]) {
            // Explorer récursivement le chemin à partir de l'état d'arrivée
            if (explorerChemins(automate, automate->transitions[i].etatArrivee, mot, index + 1)) {
                return 1; // Un chemin valide a été trouvé
            }
        }
    }

    return 0; // Aucun chemin valide trouvé
}

// Fonction pour vérifier si un mot est engendré par l'automate
int motEstEngendre(const Automate *automate, const char *mot) {
    // Parcourir tous les états initiaux
    for (int i = 0; i < automate->nbEtats; i++) {
        if (automate->etats[i].estInitial) {
            // Explorer les chemins à partir de cet état initial
            if (explorerChemins(automate, automate->etats[i].id, mot, 0)) {
                return 1; // Le mot est accepté
            }
        }
    }

    return 0; // Aucun chemin valide trouvé
}

// Fonction pour afficher l'état (ou les états) ayant le plus grand nombre de transitions sortantes
void afficherEtatsAvecMaxTransitionsSortantes(const Automate *automate) {
    int maxTransitions = 0;
    int count[MAX_ETATS] = {0};

    // Compter le nombre de transitions sortantes pour chaque état
    for (int i = 0; i < automate->nbTransitions; i++) {
        count[automate->transitions[i].etatDepart]++;
    }

    // Trouver le nombre maximum de transitions sortantes
    for (int i = 0; i < automate->nbEtats; i++) {
        if (count[automate->etats[i].id] > maxTransitions) {
            maxTransitions = count[automate->etats[i].id];
        }
    }

    // Afficher les états ayant le nombre maximum de transitions sortantes
    printf(ANSI_COLOR_YELLOW "États avec le plus grand nombre de transitions sortantes (%d) :\n" ANSI_COLOR_RESET, maxTransitions);
    for (int i = 0; i < automate->nbEtats; i++) {
        if (count[automate->etats[i].id] == maxTransitions) {
            printf(ANSI_COLOR_YELLOW "État %d\n" ANSI_COLOR_RESET, automate->etats[i].id);
        }
    }
}

// Fonction pour afficher les états ayant au moins une transition avec une lettre donnée
void afficherEtatsAvecTransitionParLettre(const Automate *automate, char lettre) {
    printf(ANSI_COLOR_YELLOW "États ayant au moins une transition avec la lettre '%c' :\n" ANSI_COLOR_RESET, lettre);
    for (int i = 0; i < automate->nbTransitions; i++) {
        if (automate->transitions[i].symbole == lettre) {
            printf(ANSI_COLOR_YELLOW "État %d\n" ANSI_COLOR_RESET, automate->transitions[i].etatDepart);
        }
    }
}

// Fonction pour lire un fichier .txt contenant une liste de mots et afficher ceux qui sont acceptés
void lireFichierEtAfficherMotsAcceptes(const Automate *automate, const char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    if (!f) {
        printf(ANSI_COLOR_RED "Erreur lors de l'ouverture du fichier.\n" ANSI_COLOR_RESET);
        return;
    }

    char mot[100];
    while (fgets(mot, sizeof(mot), f)) {
        mot[strcspn(mot, "\n")] = '\0'; // Supprimer le saut de ligne
        if (motEstEngendre(automate, mot)) {
            printf(ANSI_COLOR_GREEN "Mot accepté : %s\n" ANSI_COLOR_RESET, mot);
        } else {
            printf(ANSI_COLOR_RED "Mot rejeté : %s\n" ANSI_COLOR_RESET, mot);
        }
    }

    fclose(f);
}

Automate unionAutomates(const Automate *a1, const Automate *a2) {
    Automate result;
    memset(&result, 0, sizeof(Automate)); // Initialiser toutes les données à zéro

    if (!a1 || !a2) {
        printf("Erreur : Pointeur nul passé à la fonction.\n");
        return result;
    }

    result.nbEtats = a1->nbEtats + a2->nbEtats + 2;
    result.nbTransitions = a1->nbTransitions + a2->nbTransitions + 4;

    // Ajouter le nouvel état initial
    result.etats[0].id = 0;
    result.etats[0].estInitial = 1;
    result.etats[0].estFinal = 0;

    // Copier les états de a1 sans décaler les IDs
    for (int i = 0; i < a1->nbEtats; i++) {
        result.etats[i + 1] = a1->etats[i];
        result.etats[i + 1].estInitial = 0; // Les états de a1 ne sont plus initiaux
        result.etats[i + 1].estFinal = 0;   // Les états de a1 ne sont plus finaux
    }

    // Copier les états de a2 sans décaler les IDs
    for (int i = 0; i < a2->nbEtats; i++) {
        result.etats[a1->nbEtats + 1 + i] = a2->etats[i];
        result.etats[a1->nbEtats + 1 + i].estInitial = 0; // Les états de a2 ne sont plus initiaux
        result.etats[a1->nbEtats + 1 + i].estFinal = 0;   // Les états de a1 ne sont plus finaux
    }

    // Ajouter le nouvel état final
    int nouvelEtatFinal = a1->nbEtats + a2->nbEtats + 1;
    result.etats[nouvelEtatFinal].id = nouvelEtatFinal;
    result.etats[nouvelEtatFinal].estInitial = 0;
    result.etats[nouvelEtatFinal].estFinal = 1;

    // Copier les transitions de a1 sans décaler les IDs
    for (int i = 0; i < a1->nbTransitions; i++) {
        result.transitions[i] = a1->transitions[i];
    }

    // Copier les transitions de a2 sans décaler les IDs
    for (int i = 0; i < a2->nbTransitions; i++) {
        result.transitions[a1->nbTransitions + i] = a2->transitions[i];
    }

    // Ajouter des e-transitions du nouvel état initial vers les états initiaux de a1 et a2
    int indexTransitionEpsilon = a1->nbTransitions + a2->nbTransitions;

    for (int i = 0; i < a1->nbEtats; i++) {
        if (a1->etats[i].estInitial) {
            result.transitions[indexTransitionEpsilon].etatDepart = 0;
            result.transitions[indexTransitionEpsilon].symbole = 'e';
            result.transitions[indexTransitionEpsilon].etatArrivee = a1->etats[i].id;
            indexTransitionEpsilon++;
        }
    }

    for (int i = 0; i < a2->nbEtats; i++) {
        if (a2->etats[i].estInitial) {
            result.transitions[indexTransitionEpsilon].etatDepart = 0;
            result.transitions[indexTransitionEpsilon].symbole = 'e';
            result.transitions[indexTransitionEpsilon].etatArrivee = a2->etats[i].id;
            indexTransitionEpsilon++;
        }
    }

    // Ajouter des e-transitions des anciens états finaux de a1 et a2 vers le nouvel état final
    for (int i = 0; i < a1->nbEtats; i++) {
        if (a1->etats[i].estFinal) {
            result.transitions[indexTransitionEpsilon].etatDepart = a1->etats[i].id;
            result.transitions[indexTransitionEpsilon].symbole = 'e';
            result.transitions[indexTransitionEpsilon].etatArrivee = nouvelEtatFinal;
            indexTransitionEpsilon++;
        }
    }

    for (int i = 0; i < a2->nbEtats; i++) {
        if (a2->etats[i].estFinal) {
            result.transitions[indexTransitionEpsilon].etatDepart = a2->etats[i].id;
            result.transitions[indexTransitionEpsilon].symbole = 'e';
            result.transitions[indexTransitionEpsilon].etatArrivee = nouvelEtatFinal;
            indexTransitionEpsilon++;
        }
    }
    return result;
}

Automate concatenationAutomates(const Automate *a1, const Automate *a2) {
    Automate result;
    memset(&result, 0, sizeof(Automate)); // Initialiser toutes les données à zéro

    // Vérification des pointeurs
    if (!a1 || !a2) {
        printf("Erreur : Pointeur nul passé à la fonction.\n");
        return result; // Retourne un automate vide
    }

    // Trouver le dernier ID du premier automate
    int dernierIdA1 = 0;
    for (int i = 0; i < a1->nbEtats; i++) {
        if (a1->etats[i].id > dernierIdA1) {
            dernierIdA1 = a1->etats[i].id;
        }
    }

    // Vérifier si un état du deuxième automate a le même ID qu'un état du premier automate
    bool besoinDecalage = false;
    for (int i = 0; i < a2->nbEtats; i++) {
        for (int j = 0; j < a1->nbEtats; j++) {
            if (a2->etats[i].id == a1->etats[j].id) {
                besoinDecalage = true;
                break;
            }
        }
        if (besoinDecalage) break;
    }

    int decalage = besoinDecalage ? dernierIdA1 + 1 : 0; // Décalage uniquement si nécessaire

    // Calcul du nombre total d'états et de transitions
    result.nbEtats = a1->nbEtats + a2->nbEtats;
    result.nbTransitions = a1->nbTransitions + a2->nbTransitions;

    // Copier les états de a1 sans modification
    for (int i = 0; i < a1->nbEtats; i++) {
        result.etats[i] = a1->etats[i];
        result.etats[i].estFinal = 0; // Les états finaux de a1 ne sont plus finaux
    }

    // Copier les états de a2 avec décalage des IDs si nécessaire
    for (int i = 0; i < a2->nbEtats; i++) {
        result.etats[a1->nbEtats + i] = a2->etats[i];
        if (besoinDecalage) {
            result.etats[a1->nbEtats + i].id += decalage; // Décaler les IDs
        }
        result.etats[a1->nbEtats + i].estInitial = 0; // Les états initiaux de a2 ne sont plus initiaux
    }

    // Copier les transitions de a1 sans modification
    for (int i = 0; i < a1->nbTransitions; i++) {
        result.transitions[i] = a1->transitions[i];
    }

    // Copier les transitions de a2 avec décalage des IDs si nécessaire
    for (int i = 0; i < a2->nbTransitions; i++) {
        result.transitions[a1->nbTransitions + i] = a2->transitions[i];
        if (besoinDecalage) {
            result.transitions[a1->nbTransitions + i].etatDepart += decalage; // Décaler les IDs
            result.transitions[a1->nbTransitions + i].etatArrivee += decalage; // Décaler les IDs
        }
    }

    // Ajouter des transitions epsilon entre les états finaux de a1 et les états initiaux de a2
    for (int i = 0; i < a1->nbEtats; i++) {
        if (a1->etats[i].estFinal) {
            for (int j = 0; j < a2->nbEtats; j++) {
                if (a2->etats[j].estInitial) {
                    if (result.nbTransitions < MAX_TRANSITIONS) { // Vérification des limites
                        result.transitions[result.nbTransitions].etatDepart = a1->etats[i].id;
                        result.transitions[result.nbTransitions].symbole = 'e';
                        result.transitions[result.nbTransitions].etatArrivee = besoinDecalage ? a2->etats[j].id + decalage : a2->etats[j].id;
                        result.nbTransitions++;
                    } else {
                        printf("Erreur : Trop de transitions ajoutées !\n");
                        return result; // Retourner l'automate partiellement construit
                    }
                }
            }
        }
    }

    return result;
}

// Fonction pour afficher le menu et gérer les choix
void menu() {
    Automate automate, automate1, automate2, result;
    int choix;
    char nomFichier[100];
    char lettre;
    char mot[100];
    char expression[100];

    do {
        printf(ANSI_COLOR_YELLOW "\n--------------------------------Menu --------------------------------------------------\n" ANSI_COLOR_RESET);
        printf("1. Ajouter un automate manuellement\n" ANSI_COLOR_RESET);
        printf("2. Afficher l'automate\n" ANSI_COLOR_RESET);
        printf("3. Enregistrer l'automate dans un fichier .dot\n" ANSI_COLOR_RESET);
        printf("4. Charger un automate depuis un fichier .dot\n" ANSI_COLOR_RESET);
        printf("5. Afficher l'état (ou les états) ayant le plus grand nombre de transitions sortantes\n" ANSI_COLOR_RESET);
        printf("6. Afficher les états ayant au moins une transition avec une lettre donnée\n" ANSI_COLOR_RESET);
        printf("7. Vérifier si un mot est engendré par l'automate\n" ANSI_COLOR_RESET);
        printf("8. Lire un fichier .txt contenant une liste de mots et afficher ceux qui sont acceptés\n" ANSI_COLOR_RESET);
        printf("9. Concaténation de deux automates\n" ANSI_COLOR_RESET);
        printf("10. Union de deux automates\n" ANSI_COLOR_RESET);
        printf("11. Quitter\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_YELLOW "Choix : " ANSI_COLOR_RESET);
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouterAutomate(&automate);
                break;
            case 2:
                afficherAutomate(&automate);
                break;
            case 3:
                printf(ANSI_COLOR_CYAN "Nom du fichier : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                enregistrerAutomateDot(&automate, nomFichier);
                break;
            case 4:
                printf(ANSI_COLOR_CYAN "Nom du fichier : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                chargerAutomateDot(&automate, nomFichier);
                break;
            case 5:
                afficherEtatsAvecMaxTransitionsSortantes(&automate);
                break;
            case 6:
                printf(ANSI_COLOR_CYAN "Entrez une lettre : " ANSI_COLOR_RESET);
                scanf(" %c", &lettre);
                afficherEtatsAvecTransitionParLettre(&automate, lettre);
                break;
            case 7:
                printf(ANSI_COLOR_CYAN "Entrez un mot : " ANSI_COLOR_RESET);
                scanf("%s", mot);
                if (motEstEngendre(&automate, mot)) {
                    printf(ANSI_COLOR_GREEN "Le mot \"%s\" est accepté.\n" ANSI_COLOR_RESET, mot);
                } else {
                    printf(ANSI_COLOR_RED "Le mot \"%s\" est rejeté.\n" ANSI_COLOR_RESET, mot);
                }
                break;
            case 8:
                printf(ANSI_COLOR_CYAN "Nom du fichier : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                lireFichierEtAfficherMotsAcceptes(&automate, nomFichier);
                break;
            case 9:
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour le premier automate : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                chargerAutomateDot(&automate1, nomFichier);
            
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour le second automate : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                chargerAutomateDot(&automate2, nomFichier);
            
                result = concatenationAutomates(&automate1, &automate2);
                afficherAutomate(&result);
            
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour enregistrer l'automate résultant : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                enregistrerAutomateDot(&result, nomFichier);
                break;
            case 10:
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour le premier automate : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                chargerAutomateDot(&automate1, nomFichier);
            
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour le second automate : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                chargerAutomateDot(&automate2, nomFichier);
            
                result = unionAutomates(&automate1, &automate2);
                afficherAutomate(&result);
            
                printf(ANSI_COLOR_CYAN "Entrez le nom du fichier .dot pour enregistrer l'automate résultant : " ANSI_COLOR_RESET);
                scanf("%s", nomFichier);
                enregistrerAutomateDot(&result, nomFichier);
                break;
            case 11:
                printf(ANSI_COLOR_RED "Au revoir !\n" ANSI_COLOR_RESET);
                break;
            default:
                printf(ANSI_COLOR_RED "Choix invalide, veuillez réessayer.\n" ANSI_COLOR_RESET);
        }
    } while (choix != 11);
}

// Fonction principale
int main() {
    // Configurer le terminal pour supporter UTF-8
    system("chcp 65001 > nul");

    menu();
    return 0;
}