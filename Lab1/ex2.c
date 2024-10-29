#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "error arg", argv[0]);
        return 1;
    }
    
    int start_index=0;
    int count_only = 0;
    int show_filename = 1;
    int list_filenames = 0;
    int line_number = 0;
    int invert_match = 0;

 int count=0;
    p = strtok (arg," -"");
  while (p != NULL)
  {
       if(p[0]=='-'){
        if (strcmp(p, "-c") == 0) {
            count_only = 1;
            start_index=++;}
        } else if (strcmp(p, "-h") == 0) {
            show_filename = 0;
start_index=++;}
        } else if (strcmp(p, "-i") == 0) {
            ignore_case = 1;
start_index=++;}
        } else if (strcmp(p, "-l") == 0) {
            list_filenames = 1;
start_index=++;}
        } else if (strcmp(p, "-n") == 0) {
            line_number = 1;
start_index=++;}
        } else if (strcmp(p, "-v") == 0) {
            invert_match = 1;
        start_index=++;}
        
    p = strtok (NULL, " -"");
  }

    const char *filename = argv[start_index + 1];
    const char *pattern = argv[start_index];


    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Err deschidere);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int match_count = 0;

    while (fgets(line, sizeof(line), file)) {
        bool match_found = (strstr(line, pattern) != NULL);
        if (invert_match) {
            match_found = !match_found;
        }

        if (match_found) {
            match_count++;
            if (!list_filenames) {
                if (line_number) {
                    printf("%d: ", ++line_number);
                }
                if (show_filename) {
                    printf("%s: ", filename);
                }
                printf("%s", line);
            }
        }
    }

    fclose(file);

    if (count_only) {
        printf("%d\n", match_count);
    } else if (list_filenames) {
        if (match_count > 0) {
            printf("%s\n", filename);
        }
    }

    return 0;
}

