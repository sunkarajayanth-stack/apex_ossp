#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define PATH_SIZE 1024

/* Linux directory entry structure */
struct linux_dirent64
{
    unsigned long long d_ino;
    long long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

/* Directory entry types */
#define DT_UNKNOWN 0
#define DT_DIR     4
#define DT_REG     8
#define DT_LNK     10


/* ============================================================
   UTILITY FUNCTION
   ============================================================ */

void remove_newline(char *str)
{
    str[strcspn(str, "\n")] = '\0';
}


/* ============================================================
   1. SHOW CURRENT DIRECTORY
   ============================================================ */

void show_current_directory(void)
{
    char cwd[PATH_SIZE];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return;
    }

    printf("\nCurrent Directory:\n");
    printf("%s\n", cwd);
}


/* ============================================================
   2. LIST DIRECTORY
   Uses Linux getdents64 system call
   ============================================================ */

void list_directory(void)
{
    int fd;
    char buffer[BUFFER_SIZE];

    fd = open(".", O_RDONLY | O_DIRECTORY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("\n============================================\n");
    printf("              DIRECTORY CONTENTS\n");
    printf("============================================\n");

    while (1)
    {
        int nread = syscall(
            SYS_getdents64,
            fd,
            buffer,
            BUFFER_SIZE
        );

        if (nread == -1)
        {
            perror("getdents64");
            close(fd);
            return;
        }

        if (nread == 0)
        {
            break;
        }

        int position = 0;

        while (position < nread)
        {
            struct linux_dirent64 *entry =
                (struct linux_dirent64 *)(buffer + position);

            if (entry->d_type == DT_DIR)
            {
                printf("[DIR ]  %s\n", entry->d_name);
            }
            else if (entry->d_type == DT_REG)
            {
                printf("[FILE]  %s\n", entry->d_name);
            }
            else if (entry->d_type == DT_LNK)
            {
                printf("[LINK]  %s\n", entry->d_name);
            }
            else
            {
                printf("[OTHER] %s\n", entry->d_name);
            }

            position += entry->d_reclen;
        }
    }

    close(fd);
}


/* ============================================================
   3. CHANGE DIRECTORY
   ============================================================ */

void change_directory(void)
{
    char path[PATH_SIZE];

    printf("\nEnter directory path: ");

    fgets(path, sizeof(path), stdin);
    remove_newline(path);

    if (chdir(path) == -1)
    {
        perror("chdir");
        return;
    }

    printf("Directory changed successfully.\n");
}


/* ============================================================
   4. GO TO PARENT DIRECTORY
   ============================================================ */

void parent_directory(void)
{
    if (chdir("..") == -1)
    {
        perror("chdir");
        return;
    }

    printf("Moved to parent directory.\n");
}


/* ============================================================
   5. FILE INFORMATION
   ============================================================ */

void file_information(void)
{
    char filename[PATH_SIZE];
    struct stat file_stat;

    printf("\nEnter file/directory name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    if (stat(filename, &file_stat) == -1)
    {
        perror("stat");
        return;
    }

    printf("\n============================================\n");
    printf("              FILE INFORMATION\n");
    printf("============================================\n");

    printf("Name          : %s\n", filename);
    printf("Size          : %ld bytes\n", (long)file_stat.st_size);
    printf("Inode         : %ld\n", (long)file_stat.st_ino);
    printf("User ID       : %d\n", file_stat.st_uid);
    printf("Group ID      : %d\n", file_stat.st_gid);

    if (S_ISREG(file_stat.st_mode))
    {
        printf("Type          : Regular File\n");
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        printf("Type          : Directory\n");
    }
    else if (S_ISLNK(file_stat.st_mode))
    {
        printf("Type          : Symbolic Link\n");
    }
    else
    {
        printf("Type          : Other\n");
    }

    printf("Permissions   : ");

    printf("%c", (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXUSR) ? 'x' : '-');

    printf("%c", (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXGRP) ? 'x' : '-');

    printf("%c", (file_stat.st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

    printf("\n");

    printf("Last Modified : %s", ctime(&file_stat.st_mtime));
}


/* ============================================================
   6. READ FILE
   Uses open(), read(), write(), close()
   ============================================================ */

void read_file(void)
{
    char filename[PATH_SIZE];
    char buffer[BUFFER_SIZE];

    printf("\nEnter file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    int fd = open(filename, O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("\n============================================\n");
    printf("                FILE CONTENTS\n");
    printf("============================================\n");

    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        ssize_t total_written = 0;

        while (total_written < bytes_read)
        {
            ssize_t bytes_written = write(
                STDOUT_FILENO,
                buffer + total_written,
                bytes_read - total_written
            );

            if (bytes_written == -1)
            {
                perror("write");
                close(fd);
                return;
            }

            total_written += bytes_written;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
    }

    printf("\n");

    close(fd);
}


/* ============================================================
   7. CREATE FILE
   ============================================================ */

void create_file(void)
{
    char filename[PATH_SIZE];

    printf("\nEnter new file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    int fd = open(
        filename,
        O_CREAT | O_WRONLY | O_EXCL,
        0644
    );

    if (fd == -1)
    {
        perror("open");
        return;
    }

    close(fd);

    printf("File created successfully.\n");
}


/* ============================================================
   8. CREATE DIRECTORY
   ============================================================ */

void create_directory(void)
{
    char dirname[PATH_SIZE];

    printf("\nEnter new directory name: ");

    fgets(dirname, sizeof(dirname), stdin);
    remove_newline(dirname);

    if (mkdir(dirname, 0755) == -1)
    {
        perror("mkdir");
        return;
    }

    printf("Directory created successfully.\n");
}


/* ============================================================
   9. DELETE FILE
   ============================================================ */

void delete_file(void)
{
    char filename[PATH_SIZE];

    printf("\nEnter file name to delete: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    if (unlink(filename) == -1)
    {
        perror("unlink");
        return;
    }

    printf("File deleted successfully.\n");
}


/* ============================================================
   10. DELETE DIRECTORY
   ============================================================ */

void delete_directory(void)
{
    char dirname[PATH_SIZE];

    printf("\nEnter directory name to delete: ");

    fgets(dirname, sizeof(dirname), stdin);
    remove_newline(dirname);

    if (rmdir(dirname) == -1)
    {
        perror("rmdir");
        return;
    }

    printf("Directory deleted successfully.\n");
}


/* ============================================================
   11. RENAME FILE / DIRECTORY
   ============================================================ */

void rename_item(void)
{
    char oldname[PATH_SIZE];
    char newname[PATH_SIZE];

    printf("\nEnter current name: ");

    fgets(oldname, sizeof(oldname), stdin);
    remove_newline(oldname);

    printf("Enter new name: ");

    fgets(newname, sizeof(newname), stdin);
    remove_newline(newname);

    if (rename(oldname, newname) == -1)
    {
        perror("rename");
        return;
    }

    printf("Renamed successfully.\n");
}


/* ============================================================
   12. SEARCH FILE
   Recursive directory search using getdents64
   ============================================================ */

void search_file(const char *directory, const char *target)
{
    int fd;
    char buffer[BUFFER_SIZE];

    fd = open(directory, O_RDONLY | O_DIRECTORY);

    if (fd == -1)
    {
        return;
    }

    while (1)
    {
        int nread = syscall(
            SYS_getdents64,
            fd,
            buffer,
            BUFFER_SIZE
        );

        if (nread == -1)
        {
            close(fd);
            return;
        }

        if (nread == 0)
        {
            break;
        }

        int position = 0;

        while (position < nread)
        {
            struct linux_dirent64 *entry =
                (struct linux_dirent64 *)(buffer + position);

            if (strcmp(entry->d_name, ".") != 0 &&
                strcmp(entry->d_name, "..") != 0)
            {
                char fullpath[PATH_SIZE];

                snprintf(
                    fullpath,
                    sizeof(fullpath),
                    "%s/%s",
                    directory,
                    entry->d_name
                );

                if (strcmp(entry->d_name, target) == 0)
                {
                    printf("Found: %s\n", fullpath);
                }

                if (entry->d_type == DT_DIR)
                {
                    search_file(fullpath, target);
                }
            }

            position += entry->d_reclen;
        }
    }

    close(fd);
}


void search(void)
{
    char filename[PATH_SIZE];

    printf("\nEnter file/directory name to search: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    printf("\n============================================\n");
    printf("                SEARCH RESULTS\n");
    printf("============================================\n");

    search_file(".", filename);
}


/* ============================================================
   13. WRITE TO FILE
   Uses O_TRUNC to replace existing contents
   ============================================================ */

void write_to_file(void)
{
    char filename[PATH_SIZE];
    char content[BUFFER_SIZE];

    printf("\nEnter file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    printf("Enter content: ");

    fgets(content, sizeof(content), stdin);

    int fd = open(
        filename,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1)
    {
        perror("open");
        return;
    }

    ssize_t bytes = write(
        fd,
        content,
        strlen(content)
    );

    if (bytes == -1)
    {
        perror("write");
    }
    else
    {
        printf("Content written successfully.\n");
    }

    close(fd);
}


/* ============================================================
   14. APPEND TO FILE
   Uses O_APPEND
   ============================================================ */

void append_to_file(void)
{
    char filename[PATH_SIZE];
    char content[BUFFER_SIZE];

    printf("\nEnter file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    printf("Enter content to append: ");

    fgets(content, sizeof(content), stdin);

    int fd = open(
        filename,
        O_WRONLY | O_CREAT | O_APPEND,
        0644
    );

    if (fd == -1)
    {
        perror("open");
        return;
    }

    ssize_t bytes = write(
        fd,
        content,
        strlen(content)
    );

    if (bytes == -1)
    {
        perror("write");
    }
    else
    {
        printf("Content appended successfully.\n");
    }

    close(fd);
}


/* ============================================================
   15. COPY FILE
   Uses open(), read(), write(), close()
   ============================================================ */

void copy_file(void)
{
    char source[PATH_SIZE];
    char destination[PATH_SIZE];
    char buffer[BUFFER_SIZE];

    printf("\nEnter source file: ");

    fgets(source, sizeof(source), stdin);
    remove_newline(source);

    printf("Enter destination file: ");

    fgets(destination, sizeof(destination), stdin);
    remove_newline(destination);

    int src = open(source, O_RDONLY);

    if (src == -1)
    {
        perror("open source");
        return;
    }

    int dest = open(
        destination,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (dest == -1)
    {
        perror("open destination");
        close(src);
        return;
    }

    ssize_t bytes_read;

    while ((bytes_read = read(
        src,
        buffer,
        sizeof(buffer)
    )) > 0)
    {
        ssize_t total_written = 0;

        while (total_written < bytes_read)
        {
            ssize_t bytes_written = write(
                dest,
                buffer + total_written,
                bytes_read - total_written
            );

            if (bytes_written == -1)
            {
                perror("write");
                close(src);
                close(dest);
                return;
            }

            total_written += bytes_written;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
    }
    else
    {
        printf("File copied successfully.\n");
    }

    close(src);
    close(dest);
}


/* ============================================================
   16. CHANGE FILE PERMISSIONS
   Uses chmod()
   ============================================================ */

void change_permissions(void)
{
    char filename[PATH_SIZE];
    char permission[20];

    printf("\nEnter file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    printf("Enter permission in octal (example 755): ");

    fgets(permission, sizeof(permission), stdin);
    remove_newline(permission);

    mode_t mode = (mode_t)strtol(
        permission,
        NULL,
        8
    );

    if (chmod(filename, mode) == -1)
    {
        perror("chmod");
        return;
    }

    printf("Permissions changed successfully.\n");
}


/* ============================================================
   17. CREATE SYMBOLIC LINK
   Uses symlink()
   ============================================================ */

void create_symbolic_link(void)
{
    char target[PATH_SIZE];
    char linkname[PATH_SIZE];

    printf("\nEnter target file/path: ");

    fgets(target, sizeof(target), stdin);
    remove_newline(target);

    printf("Enter symbolic link name: ");

    fgets(linkname, sizeof(linkname), stdin);
    remove_newline(linkname);

    if (symlink(target, linkname) == -1)
    {
        perror("symlink");
        return;
    }

    printf("Symbolic link created successfully.\n");
}


/* ============================================================
   18. READ SYMBOLIC LINK
   Uses readlink()
   ============================================================ */

void read_symbolic_link(void)
{
    char linkname[PATH_SIZE];
    char target[PATH_SIZE];

    printf("\nEnter symbolic link name: ");

    fgets(linkname, sizeof(linkname), stdin);
    remove_newline(linkname);

    ssize_t length = readlink(
        linkname,
        target,
        sizeof(target) - 1
    );

    if (length == -1)
    {
        perror("readlink");
        return;
    }

    target[length] = '\0';

    printf("Symbolic link points to: %s\n", target);
}


/* ============================================================
   19. CLEAR FILE CONTENTS
   Uses O_TRUNC
   ============================================================ */

void clear_file(void)
{
    char filename[PATH_SIZE];

    printf("\nEnter file name: ");

    fgets(filename, sizeof(filename), stdin);
    remove_newline(filename);

    int fd = open(
        filename,
        O_WRONLY | O_TRUNC
    );

    if (fd == -1)
    {
        perror("open");
        return;
    }

    close(fd);

    printf("File contents cleared successfully.\n");
}


/* ============================================================
   MENU
   ============================================================ */

void display_menu(void)
{
    printf("\n");
    printf("============================================\n");
    printf("       LINUX FILE SYSTEM EXPLORER\n");
    printf("============================================\n");

    show_current_directory();

    printf("\n");

    printf("1.  List Directory\n");
    printf("2.  Change Directory\n");
    printf("3.  Go to Parent Directory\n");
    printf("4.  File Information\n");
    printf("5.  Read File\n");
    printf("6.  Create File\n");
    printf("7.  Create Directory\n");
    printf("8.  Delete File\n");
    printf("9.  Delete Directory\n");
    printf("10. Rename File/Directory\n");
    printf("11. Search File\n");
    printf("12. Write to File\n");
    printf("13. Append to File\n");
    printf("14. Copy File\n");
    printf("15. Change File Permissions\n");
    printf("16. Create Symbolic Link\n");
    printf("17. Read Symbolic Link\n");
    printf("18. Clear File Contents\n");
    printf("19. Exit\n");
}


/* ============================================================
   MAIN FUNCTION
   ============================================================ */

int main(void)
{
    char input[20];
    int choice;

    while (1)
    {
        display_menu();

        printf("\nEnter choice: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        choice = atoi(input);

        switch (choice)
        {
            case 1:
                list_directory();
                break;

            case 2:
                change_directory();
                break;

            case 3:
                parent_directory();
                break;

            case 4:
                file_information();
                break;

            case 5:
                read_file();
                break;

            case 6:
                create_file();
                break;

            case 7:
                create_directory();
                break;

            case 8:
                delete_file();
                break;

            case 9:
                delete_directory();
                break;

            case 10:
                rename_item();
                break;

            case 11:
                search();
                break;

            case 12:
                write_to_file();
                break;

            case 13:
                append_to_file();
                break;

            case 14:
                copy_file();
                break;

            case 15:
                change_permissions();
                break;

            case 16:
                create_symbolic_link();
                break;

            case 17:
                read_symbolic_link();
                break;

            case 18:
                clear_file();
                break;

            case 19:
                printf("\nExiting Linux File System Explorer...\n");
                return 0;

            default:
                printf("\nInvalid choice. Please enter 1-19.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}
