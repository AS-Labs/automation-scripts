#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define MAX_PATH_LENGTH 100
#define MAX_DATE_LENGTH 11
#define MAX_TIMESTAMP_LENGTH 9

void show_help();
void create_note();
void open_note(char *date);
void delete_note(char *date);
void list_notes();
void search_notes();

int fuzzy_match(const char *pattern, const char *str);
void lower_string(char *str);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 0;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        show_help();
    } else if (strcmp(argv[1], "-n") == 0) {
        create_note();
    } else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--date") == 0) {
        if (argc >= 3) {
            open_note(argv[2]);
        } else {
            printf("Please provide a date.\n");
        }
    } else if (strcmp(argv[1], "--delete") == 0) {
        if (argc >= 3) {
            delete_note(argv[2]);
        } else {
            printf("Please provide a date.\n");
        }
    } else if (strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0) {
        list_notes();
    } else if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--search") == 0) {
        search_notes();
    } else {
        printf("Invalid option: %s\n", argv[1]);
        show_help();
    }

    return 0;
}

void show_help() {
    printf("Noter - A program to generate and manage daily notes\n\n");
    printf("Usage: noter [OPTIONS]\n\n");
    printf("Options:\n");
    printf("-h, --help               Show help and usage information\n");
    printf("-n                       Create a new note (if none exists for today, create a new file)\n");
    printf("-d, --date DATE          Open the note for a specific date\n");
    printf("--delete DATE            Delete the note for a specific date\n");
    printf("-l, --list               List all notes\n");
    printf("-s, --search             Search for notes using fuzzy finding\n");
}

void create_note() {
    time_t raw_time;
    struct tm *timeinfo;
    char date[MAX_DATE_LENGTH];
    char timestamp[MAX_TIMESTAMP_LENGTH];
    char note_file[MAX_PATH_LENGTH];

    time(&raw_time);
    timeinfo = localtime(&raw_time);
    strftime(date, MAX_DATE_LENGTH, "%Y-%m-%d", timeinfo);
    strftime(timestamp, MAX_TIMESTAMP_LENGTH, "%H:%M:%S", timeinfo);

    snprintf(note_file, MAX_PATH_LENGTH, "./%s.txt", date);

    FILE *file = fopen(note_file, "a+");
    if (file != NULL) {
        fprintf(file, "%s\n", timestamp);
        fclose(file);
        printf("Note for today already exists. Appended timestamp to the existing note.\n");

        char command[MAX_PATH_LENGTH + 5];  // +5 for "vim " and null terminator
        snprintf(command, MAX_PATH_LENGTH + 5, "vim %s", note_file);
        system(command);
    } else {
        printf("Failed to create or open note file.\n");
    }
}

void open_note(char *date) {
    char note_file[MAX_PATH_LENGTH];
    snprintf(note_file, MAX_PATH_LENGTH, "./%s.txt", date);

    FILE *file = fopen(note_file, "r");
    if (file != NULL) {
        char c;
        while ((c = fgetc(file)) != EOF) {
            putchar(c);
        }
        fclose(file);
    } else {
        printf("Note for %s does not exist.\n", date);
    }
}

void delete_note(char *date) {
    char note_file[MAX_PATH_LENGTH];
    snprintf(note_file, MAX_PATH_LENGTH, "./%s.txt", date);

    FILE *file = fopen(note_file, "r");
    if (file != NULL) {
        char c;
        while ((c = fgetc(file)) != EOF) {
            putchar(c);
        }
        fclose(file);

        printf("Are you sure you want to delete this note? (y/n): ");
        char confirmation;
        scanf("%c", &confirmation);
        getchar(); // Consume the newline character

        if (confirmation == 'y' || confirmation == 'Y') {
            if (remove(note_file) == 0) {
                printf("Note for %s deleted successfully.\n", date);
            } else {
                printf("Failed to delete the note.\n");
            }
        } else {
            printf("Deletion canceled.\n");
        }
    } else {
        printf("Note for %s does not exist.\n", date);
    }
}

void list_notes() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("./");
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                printf("%s\n", entry->d_name);
            }
        }
        closedir(dir);
    } else {
        printf("Failed to open notes directory.\n");
    }
}

void search_notes() {
    DIR *dir;
    struct dirent *entry;
    char pattern[MAX_PATH_LENGTH];

    printf("Enter search pattern: ");
    fgets(pattern, MAX_PATH_LENGTH, stdin);
    pattern[strcspn(pattern, "\n")] = '\0';  // Remove trailing newline character

    lower_string(pattern);

    dir = opendir("./");
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                char filename[MAX_PATH_LENGTH];
                strncpy(filename, entry->d_name, MAX_PATH_LENGTH);
                lower_string(filename);

                if (fuzzy_match(pattern, filename)) {
                    printf("%s\n", entry->d_name);
                }
            }
        }
        closedir(dir);
    } else {
        printf("Failed to open notes directory.\n");
    }
}

int fuzzy_match(const char *pattern, const char *str) {
    int i = 0;
    int j = 0;

    while (pattern[i] != '\0' && str[j] != '\0') {
        if (pattern[i] == str[j]) {
            i++;
        }
        j++;
    }

    if (pattern[i] == '\0') {
        return 1;  // Fuzzy match found
    }

    return 0;  // Fuzzy match not found
}

void lower_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32;
        }
    }
}

