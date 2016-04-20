/**银行家算法的实现
 可利用资源向量Avaiable[i] 表示资源i的可利用数量
 最大需求矩阵Max[i][j] 表示进程i对资源j的最大需求
 需求矩阵Need[i][j] 表示进程i对资源j还需要的数量
 分配矩阵Allocation[i][j] 表示进程i当前已被分配资源j的数量
 资源请求矩阵Request[i][j] 表示进程i对资源j的请求数量
*/
#include<stdlib.h>
#include<time.h>
#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<cctype>
#define resource int
#define process  int
#define safe true
#define finish true
#define unsafe false
#define unfinish false
using namespace std;
/**------------------------------------------------*/
const int maxn = 11;///这里规定最大进程数和最大资源种类
resource Avaiable[maxn];///可利用资源向量Avaiable[i] 表示资源i的可利用数量
resource Work[maxn];///Work[i]表示安全检测时每种资源的可用数量
resource Max[maxn][maxn];///最大需求矩阵Max[i][j] 表示进程i对资源j的最大需求
resource Need[maxn][maxn];///需求矩阵Need[i][j] 表示进程i对资源j还需要的数量
resource Allocation[maxn][maxn];///分配矩阵Allocation[i][j] 表示进程i当前已被分配资源j的数量
resource Request[maxn][maxn];/// 资源请求矩阵Request[i][j] 表示进程i对资源j的请求数量
resource process_num,sources_num;///进程数 和 资源种类
bool Finish[maxn];///进程是否执行完成
int safeSeq[maxn],seq_num;///安全序列
char button[1000];///字符串输入
void print_table();///打印资源表
void Try_allocate(process P);///试探分配
void Rollback(process P);///试探分配失败，回滚
bool Enough(process P);///资源是否足够分配给进程P
void Collect(process P);///回收进程P所用的资源
bool Req_is_nobig_than_Need(process P);///判断进程P请求的资源是否大于需求矩阵
bool Req_is_nobig_than_Ava(process P);///判断进程P请求的资源是否大于可利用资源的数量
bool Safecheck();///安全检查
bool Request_(process P);///进程P发出资源请求
void read();///读入手动初始化的数据
double random();///随机函数
int random(int m);///生成0到m以内的任意整数
int check_random(int m,int l,int r);///检查生成数据是否合法
void auto_init();///自动初始化
void System_init();///系统初始化
/**--------------------------------------------------*/
void print_table()
{
    printf("\t\t\t*********资源分配表*********\n");
    printf("Avaiable\n");
    for(int i = 1; i <= sources_num; i++) printf("%4d",Avaiable[i]);
    printf("\nMax\n");
    for(int i = 1; i <= process_num ; i++)
    {
        printf("P%d ",i);
        for(int j = 1; j <= sources_num; j++) printf("%4d",Max[i][j]);
        printf("\n");
    }
    printf("Need\n");
    for(int i = 1; i <= process_num ; i++)
    {
        printf("P%d ",i);
        for(int j = 1; j <= sources_num; j++) printf("%4d",Need[i][j]);
        printf("\n");
    }
    printf("Allocation\n");
    for(int i = 1; i <= process_num ; i++)
    {
        printf("P%d ",i);
        for(int j = 1; j <= sources_num; j++) printf("%4d",Allocation[i][j]);
        printf("\n");
    }
}
/**试探分配----------------------------------------------*/
void Try_allocate(process P)
{
    for(resource i = 1; i <= sources_num ; i++)
    {
        Avaiable[i] -= Request[P][i];
        Allocation[P][i] += Request[P][i];
        Need[P][i] -= Request[P][i];
    }
}
/**-----------------------------------------------------*/
/**试探分配将进入不安全状态，分配回滚-------------------*/
void Rollback(process P)
{
    for(resource i = 1; i <= sources_num ; i++)
    {
        Avaiable[i] += Request[P][i];
        Allocation[P][i] -= Request[P][i];
        Need[P][i] += Request[P][i];
    }
}
bool Enough(process P)///判断是否有足够的资源分配
{
    for(resource i = 1; i <= sources_num ; i++)
    {
        if(Need[P][i] > Work[i]) return false;
    }
    return true;
}

void Collect(process P)///回收资源
{
    for(resource i = 1; i <= sources_num ; i ++)
    {
        Work[i] += Allocation[P][i];
    }
}
bool Req_is_nobig_than_Need(process P)
{
    for(int i = 1; i <= sources_num ; i ++)
    {
        if(Request[P][i] > Need[P][i]) return false;
    }
    return true;
}

bool Req_is_nobig_than_Ava(process P)
{
    for(int i = 1; i <= sources_num ; i++)
    {
        if(Request[P][i] > Avaiable[i]) return false;
    }
    return true;
}
/**---------------------------------------------------------*/
bool Safecheck()
{
    for(int i = 1; i <= sources_num ; i++)
        Work[i] = Avaiable[i];
    memset(Finish,unfinish,sizeof(Finish));
    process i;
    seq_num = 0;
    for( i = 1 ; i <= process_num ; i ++)
    {
        if(Finish[i] == unfinish)
        {
            if(Enough(i)) ///有足够的资源分配给该进程
            {
                Collect(i); ///回收该进程的资源
                Finish[i] = finish;
                safeSeq[ ++seq_num ] = i;
                i = -1;
            }
        }
    }
    for(int i = 1; i <= process_num ; i++)
    {
        if(Finish[i] == unfinish) return unsafe;
    }
    return safe;
}

bool Request_(process P)
{
    if(Req_is_nobig_than_Need(P))
    {
        if(Req_is_nobig_than_Ava(P))
        {
            Try_allocate(P);
            if(Safecheck())
            {
                return true;
            }
            else
            {
                printf("安全性检查失败。原因：系统将进入不安全状态，有可能引起死锁。\n");
                printf("正在回滚...\n");
                Rollback(P);
            }
        }
        else
        {
            printf("安全性检查失败。原因：请求向量大于可利用资源向量。\n");
        }
    }
    else
    {
        printf("安全性检查失败。原因：请求向量大于需求向量。\n");
    }
    return false;
}
void read()
{

    printf("\n请输入初始进程数\n:");
    while(scanf("%s",button))
    {
        int flag = 1;
        for(int i = 0; i < strlen(button) &&flag; i++)
            if(!isdigit(button[i]))
            {
                flag = 0;
            }
        if(!flag)
        {
            printf("输入不合法，请输入大于0小于100的整数\n");
        }
        else
        {
            process_num = 0;
            int len = strlen(button);
            for(int i = 0; i < len; i++) process_num = process_num * 10 + button[i] - '0';
            break;
        }
    }
    printf("\n请输入初始资源种类\n:");
    while(scanf("%s",button))
    {
        int flag = 1;
        for(int i = 0; i < strlen(button) &&flag; i++)
            if(!isdigit(button[i]))
            {
                flag = 0;
            }
        if(!flag)
        {
            printf("输入不合法，请输入大于0小于100的整数\n");
        }
        else
        {
            sources_num= 0;
            int len  = strlen(button);
            for(int i = 0; i < len; i++) sources_num = sources_num * 10 + button[i] - '0';
            break;
        }
    }
    printf("\n请输入初始%d个可利用资源数量-----------\n",sources_num);
    for(int i = 1; i <= sources_num; i++) scanf("%d",&Avaiable[i]);
    printf("\n请输入最大需求矩阵-------------\n");
    for(int i = 1; i <= process_num; i++)
        for(int j = 1; j <= sources_num; j++) scanf("%d",&Max[i][j]);
    printf("\n请输入需求矩阵-----\n");
    for(int i = 1; i<=process_num; i++)
        for(int j = 1; j<=sources_num; j++) scanf("%d",&Need[i][j]);
    printf("\n请输入资源分配矩阵----\n");
    for(int i = 1; i <= process_num; i++)
        for(int j = 1; j <= sources_num; j++) scanf("%d",&Allocation[i][j]);
}
/**随机数据生成-----------------*/
double random()
{
    return (double)rand()/RAND_MAX;
}
int random(int m)
{
    return (int)(random()*(m-1)+0.5);
}
int check_random(int m,int l,int r)
{
    while(1)
    {
        int x = random(m);
        if(x>=l&&x<=r) return x;
    }
}
void auto_init()
{
    srand(time(NULL));
    process_num = check_random(maxn,1,maxn-1);
    sources_num = check_random(maxn,1,maxn-1);
    for(int i = 1; i<=sources_num; i++)
        Avaiable[i] = check_random(maxn,1,maxn);
    for(int i = 1; i <= process_num; i++)
        for(int j = 1; j<=sources_num; j++)
        {
            Max[i][j] = check_random(maxn,1,maxn);
            Allocation[i][j] = check_random(maxn,1,maxn);
            Need[i][j] = check_random(maxn,1,maxn);
        }
}
/**--------------------------------*/
void System_init()
{
    printf("请求自动初始化 或者 手动输入数据初始化：\n");
    printf("自动初始化请按1:\n");
    printf("手动初始化请按2:\n");
    while(scanf("%s",button)&&button[0]<'1'||button[0]>'2'||strlen(button)>1)
    {
        printf("输入不合法，请输入1 或者 2\n");
    }
    if(button[0]=='1')
    {   
        do
        {   
            auto_init();
        }
        while(!Safecheck());
        printf("系统处于安全状态\n");
        printf("安全序列是:");
        for(int i = 1; i<=seq_num; i++) printf("P%d ",safeSeq[i]);
        printf("\n");
    }
    else
    {   int f  = 0;
        do
        {   if(f++) printf("输入的数据会造成死锁，请求重新输入");
            read();
        }
        while(!Safecheck());
        printf("系统处于安全状态\n");
        printf("安全序列是:");
        for(int i = 1; i<=seq_num; i++) printf("P%d ",safeSeq[i]);
        printf("\n");
    }
}
int main()
{
    System_init();
    print_table();
    char ch;
    do
    {
        process P;
        printf("请输入请求分配的进程号:\n");
        scanf("%d",&P);
        printf("请输入进程%d对%d类资源的请求数量:\n",P,sources_num);
        for(int i = 1; i <= sources_num; i++) scanf("%d",&Request[P][i]);
        if(Request_(P))
        {
            printf("分配成功\n");
            printf("安全序列是:");
            for(int i = 1; i<=seq_num; i++) printf("P%d ",safeSeq[i]);
            printf("\n");
        }
        else
        {
            printf("分配失败\n");
        }
        printf("是否继续分配Y|N?\n");
        fflush(stdin);
        ch = getchar();
    }
    while(ch =='Y' || ch == 'y');
    printf("执行完毕.\n");
    return 0;
}
