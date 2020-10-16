#include <iostream>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <grp.h>
#include <pwd.h>
#include <vector>
#include <algorithm>

struct File {
    std::string fpath;
    struct stat sb;
    int tflag;
    struct FTW ftwbuf;

    bool operator<(File const& rhs) const {
        return (std::string)(fpath.c_str() + ftwbuf.base) < (std::string)(rhs.fpath.c_str() + rhs.ftwbuf.base);
    }
};

std::vector<File> files;

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
display_info(std::vector<File> &fv)
{
    std::sort(files.begin(), files.end());

    for (auto f : fv) {
        char buf[200];
        struct tm *tm;
        tm = localtime(&(f.sb.st_mtime));
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);

        char filetype = '\0';
        if (S_ISDIR(f.sb.st_mode)) {
            filetype = '/';
        } else if (S_ISLNK(f.sb.st_mode)) {
            filetype = '@';
        } else if (S_ISFIFO(f.sb.st_mode)) {
            filetype = '|';
        } else if (S_ISSOCK(f.sb.st_mode)) {
            filetype = '=';
        } else if (!S_ISREG(f.sb.st_mode)) {
            filetype = '?';
        } else if (S_IXUSR & f.sb.st_mode) {
            filetype = '*';
        }


        printf("%s %s %7jd %s %-40s %c%s\n",
               permissions(&f.sb),
               getpwuid(f.sb.st_uid)->pw_name, (intmax_t) f.sb.st_size,
               buf, f.fpath.c_str(), filetype, f.fpath.c_str() + f.ftwbuf.base);
    }
    return 0;
}

int files_to_vector(const char *fpath, const struct stat *sb,
                    int tflag, struct FTW *ftwbuf)
{
    if (!strcmp(fpath, ".")) {
        return 0;
    }

    files.push_back((File){fpath, *sb, tflag, *ftwbuf});
    return 0;
}

int main(int argc, char **argv) {
    int flags = FTW_MOUNT || FTW_PHYS || FTW_DEPTH;
    if (nftw((argc < 2) ? "." : argv[1], files_to_vector, 20, flags) == -1)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    display_info(files);

    exit(EXIT_SUCCESS);
}
