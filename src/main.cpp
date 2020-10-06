#include <iostream>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <grp.h>
#include <pwd.h>

char* permissions(const struct stat *sb){
    char *access_modes = static_cast<char *>(malloc(10));
    access_modes[0] = (sb->st_mode & S_IRUSR) ? 'r' : '-';
    access_modes[1] = (sb->st_mode & S_IWUSR) ? 'w' : '-';
    access_modes[2] = (sb->st_mode & S_IXUSR) ? 'x' : '-';
    access_modes[3] = (sb->st_mode & S_IRGRP) ? 'r' : '-';
    access_modes[4] = (sb->st_mode & S_IWGRP) ? 'w' : '-';
    access_modes[5] = (sb->st_mode & S_IXGRP) ? 'x' : '-';
    access_modes[6] = (sb->st_mode & S_IROTH) ? 'r' : '-';
    access_modes[7] = (sb->st_mode & S_IWOTH) ? 'w' : '-';
    access_modes[8] = (sb->st_mode & S_IXOTH) ? 'x' : '-';
    access_modes[9] = '\0';
    return access_modes;

}

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    char buf[200];
    struct tm *tm;
    tm = localtime(&(sb->st_mtime));
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    printf("%s %s %7jd %s %-40s %s\n",
           permissions(sb),
           getpwuid(sb->st_uid)->pw_name, (intmax_t) sb->st_size,
           buf,
           fpath, fpath + ftwbuf->base);
    return 0;
}

int main(int argc, char **argv) {
    int flags = FTW_MOUNT;
    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
