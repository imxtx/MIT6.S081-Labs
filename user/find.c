#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(const char *path, const char *filename, const char *etname)
{
    char buf[512], *p;  // 文件路径
    int fd;             // 文件描述符
    struct dirent de;   // 目录结构
    struct stat st;     // 文件信息结构

    // 打开文件
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot %s\n", path);
        return;
    }
    // 获取文件信息
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 判断文件类型
    switch (st.type) {
        // 若是文件，判断该文件名是否和要查找的文件名相同
        case T_FILE:
            if (etname && 0 == strcmp(etname, filename))
                printf("%s\n", path);
        break;
        // 若是目录，则在目录中递归查找文件
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            // 扫描目录中的每一个文件
            while (read(fd, &de, sizeof(de)) == sizeof(de)){
                if (de.inum == 0)
                    continue;
                // 跳过 . 和 ..
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);    // 拼接路径
                p[DIRSIZ] = 0;                  // 添加字符串结束符
                // printf("%s\n", buf);
                find(buf, filename, p);        // 递归深入目录树
            }
        break;
    }
    close(fd);
}

void
findfile(const char *path, const char *filename)
{
    find(path, filename, (void*)0);
}

int 
main(int argc, char const *argv[])
{
    if (argc != 3) {
        printf("Usage: find <dir> <filename>");
        exit(1);
    }
    findfile(argv[1], argv[2]);
    exit(0);
}
