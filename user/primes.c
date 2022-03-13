#include "kernel/types.h"
#include "user/user.h"

void pipline(int rp)
{ 
    // 从上一个过滤器读取一个数
    int p, n;
    if (0 == read(rp, &p, sizeof(int))) {
        close(rp);
        exit(0);
    }
    printf("prime %d\n", p);
    
    // 创建新管道
    int ppl[2];
    pipe(ppl);

    if (fork() == 0) {
        close(ppl[1]);      // 关闭写端
        pipline(ppl[0]);    // 递归创建下一个过滤器
    } else {
        close(ppl[0]);
        for (;;) {
            // 从上一个过滤器读取一个数
            if (0 == read(rp, &n, sizeof(int))) {
                close(rp); // 已经读完上一个过滤器产生的数
                break;
            }
            // 如果不能整除发送到下一个过滤器
            if (n % p != 0) {
                write(ppl[1], &n, sizeof(int));
            }
        }
        close(ppl[1]);
        wait((int*)0); // 等待子进程返回
        exit(0);
    }
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0) {  // 开始pipline
        close(p[1]);    // 关闭写端
        pipline(p[0]);
        exit(0);
    } else {            // 主进程生成数字2-35
        close(p[0]);    // 关闭读端
        int i;
        for (i = 2; i <= 35; ++i)
            write(p[1], &i, sizeof(int));
        close(p[1]);
        wait((int*)0);  // 等待子进程返回
        exit(0);
    }
}
