#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

int main() {
  // Get the current date and time.
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // Create a file and append a banner with the current date.
  char filename[] = "log.txt";
  FILE *fp = fopen(filename, "a");
  if (fp == NULL) {
    printf("Could not open file.\n");
    exit(1);
  }
  fprintf(fp, "\n%s", asctime(t));
  fclose(fp);

  // Fork a new process.
  pid_t pid = fork();
  if (pid == 0) {
    // This is the child process.
    // Launch Vim to edit the file.
    char *args[] = {"vim", filename, NULL};
    execvp("vim", args);
  } else {
    // This is the parent process.
    // Wait for the child process to exit.
    wait(NULL);

    // Check if the current date is the same as the appended banner.
    fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("Could not open file.\n");
      exit(1);
    }
    char line[100];
    fgets(line, sizeof(line), fp);
    fclose(fp);
    if (strcmp(line, asctime(t)) == 0) {
      // The current date is the same, so just go to the end of the file.
      int size = ftell(fp);
      fseek(fp, size - 1, SEEK_SET);
    } else {
      // The current date is different, so append an EOD banner and the current date.
      fp = fopen(filename, "a");
      if (fp == NULL) {
        printf("Could not open file.\n");
        exit(1);
      }
      fprintf(fp, "\nEOD\n");
      fprintf(fp, "\n%s", asctime(t));
      fclose(fp);
    }

    // Write out the date and time to the screen using ncurses.
    initscr();
    addstr(asctime(t));
    refresh();
    sleep(5);
    endwin();
  }

  return 0;
}

