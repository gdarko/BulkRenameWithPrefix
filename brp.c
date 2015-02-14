/**
 *   Simple C program to perform bulk file rename.
 *   It can be used from command line on Windows or Unix systems
 *   Important: This program is still under early development phase
 *              and might have bugs, so, use at your own risk!
 *
 *   @author: Darko Gjorgjijoski
 *   @link http://darkog.com/
 *   @version: 0.0.1_alpha
 *   @license: http://opensource.org/licenses/MIT
 *   @compiler: GCC/Mingw
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifdef __linux__
#define OPERATING_SYSTEM 0
#else
#define OPERATING_SYSTEM 1
#endif


/**
 *   Function that removes the file extension.
 *   Used to remove the extension while generating the new file
 *   name with the merged string in generate_new_filename()
 */
char *del_ext(char* main_str, char sep)
{
    char *result_str, *lastdot, *lastsep;

    if (main_str == NULL) return NULL;
    if ((result_str = malloc(strlen(main_str) + 1)) == NULL) 
        return NULL;

    strcpy (result_str, main_str);
    lastdot = strrchr(result_str, '.');
    lastsep = (sep == 0) ? NULL : strrchr (result_str, sep);

    if (lastdot != NULL)
    {
        if (lastsep != NULL)
        {
            if (lastsep < lastdot)
            {
                *lastdot = '\0';
            }
        }
        else
        {
            *lastdot = '\0';
        }
    }
    return result_str;
}

/**
 *   Function for extracting the file name from given path
 */
char *removePath(const char *path)
{
    char *filename = strrchr(path, '\\');
    if (filename == NULL)
        filename = path;
    else
        filename++;
    return filename;
}


/**
 *   Function that returns pointer to the extension of the file
 *   Used to get file extension in generate_new_filename()
 */
char *get_ext(char *fspec)
{
    char *e = strrchr(fspec, '.');
    if (e == NULL) 
        e = "";
    return e;
}


/**
 *   Function to perform validation if the passed value is valid separator
 *   @char separator   Character that separate the filename and new prefix
 */
int is_valid_separator(char *separator)
{
    if(!strcmp(separator, "_") || !strcmp(separator, "-"))
        return 1;
    else 
        return 0;
}


/**
 *    Function to generates new file name.
 *    @char[] main_str  Character array that contains the file name
 *    @char[] new_str   Character array that needs to be merged with the file name
 *    @char[] separator Character array that separate the filename and new prefix
 */
char *generate_new_filename(char *main_str, char *new_str, char *separator)
{
    int len_main_str = strlen(main_str);
    int len_new_str = strlen(new_str);
    int len_ext_str = strlen(get_ext(main_str));

    char *ext = (char *)calloc(len_ext_str, sizeof(char));
    strcpy(ext, get_ext(main_str));

    char *generated = (char *)calloc((len_new_str+len_main_str+2), sizeof(char));
    strcat(generated, del_ext(main_str, 0));
    strcat(generated, is_valid_separator(separator) ? separator:"_");
    strcat(generated, new_str);
    strcat(generated, ext);
    return generated;
}

/**
 *
 *   Main function used to handle the renaming of the files
 *   @char location[] The location, a directory that needs to be processed.
 *   @char new_prefix[] The prefix that is used for the new names
 *   @char separator[] The separator
 */
void main_function(char location[], char new_prefix[], char separator[])
{
    DIR *DIRECTORY = opendir(location);

    int location_length = strlen(location);

    if(location[location_length-1] != '\\'){
        location[location_length++] = '\\';
    }
    
    struct dirent *dir;

    if(DIRECTORY)
    {
        while((dir=readdir(DIRECTORY)) != NULL)
        {
            if(dir->d_name[0]=='.') continue;

            int len_old_file_path = strlen(location)+strlen(dir->d_name)+2;
            char * fullpath = (char*)calloc(len_old_file_path,sizeof(char));
            strcat(fullpath, location);
            strcat(fullpath, dir->d_name);
            
            int len_new_file_path = strlen(location) + strlen(new_prefix) + 3;
            char * dynrename = (char*)calloc(len_new_file_path,sizeof(char));
            strcat(dynrename, location);
            strcat(dynrename, generate_new_filename(dir->d_name, new_prefix, separator));

            if((rename(fullpath, dynrename )==0))
            {
                printf("%s successfully renamed!\n", dir->d_name);
            }
            else
            {
                printf("%s not renamed!\n", dir->d_name);
            }
        }
        closedir(DIRECTORY);
    }
    else
    {
        printf("Can't open your directory!");
        return;
    }

}


/**
 *   C MAIN
 */
int main(int argc, char **argv)
{
    if(argc > 1)
    {
        int new_location_length = strlen(argv[1]);
        int new_prefix_length = strlen(argv[2]);
        int new_separator_length = strlen(argv[3]);

        char *location = (char*)calloc(new_location_length, sizeof(char));
        char *prefix = (char*)calloc(new_prefix_length, sizeof(char));
        char *separator = (char*)calloc(new_separator_length, sizeof(char));

        strcpy(location, argv[1]);
        strcpy(prefix, argv[2]);
        strcpy(separator, argv[3]);

        main_function(location, prefix, separator);
    }
    else
    {
        printf("Usage: ""%s"" ""C:\\mydir prefix""", removePath(argv[0]));
    }
    return 0;
}
