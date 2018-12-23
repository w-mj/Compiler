#include<bits/stdc++.h>
#include "../symbol_table/SymbolTable.h"

#define arr 10


using namespace std;

//void insertaddr(int a,int b){}///将符号表中a号变量的首地址赋为b
//void setoffset(int a,int b){}///将符号表中a号变量的首地址赋为b
//int getaddr(int a){return 0;}///返回符号表中a号变量的首地址
//int getoffset(int a){return 15;}///返回符号表a号变量的偏移量
//int getparanum(int a){return 0;}///返回符号表中a号函数的参数个数
//
//int getvalue(int a){return 2;}///返回符号表中a号函数的返回值类型或者a号变量的类型
//
//bool istem(int a){return false;}///返回符号表a号变量是否是临时变量
//std::string getname(int a){if(a==29)return "main";else return "gcd";}///返回符号表a号变量的名字
//bool isconst(int a){return 1;}///符号表中a号变量是不是常数
//int getnumber(int a){return 1000;}///返回符号表中a号变量的常数
//int getsize(int a){return 1;}///返回符号表中a号数组的总空间大小
//int getlength(int a){return 1;}///返回符号表中a号数组的总长度
//bool isarr(int a){return 1; }///返回符号表中a是不是数组
//string getasm(int a){return "asm";}///得到手写的汇编代码
//int getarrsize(int a,int b){}///获得数组a第一维索引为b的首地址

int s[10010];
int a[10010][3];///四元式的后三个数
int c[10010][3];///活跃信息

int seg,off;///分配地址的偏移量
stack<int> off_s;///函数开始时将off入栈
stack<int> seg_s;///函数开始时将seg入栈
int flag;///标志是全局0，主函数1，还是函数2，还是结构体3
string sname;///当前结构体名字
int soff;///结构体大小

int ax,al,aa;///标志ax和al存储的变量在字符表中的位置
int alive;///活跃信息，0：n，1：y
int n;///四元式个数

//map<string,string> caluate;

//map<string,int> typesize;


int cnt=0;///标号


vector<int> addrset;///地址集合
int huoyue[100010];///存储每个地址的活跃信息
int block[100010];///第i条四元式是不是基本块入口
int temp[100010];///在ES段分配临时变量的地址
int temp_off=0;///纪录临时变量的偏移量
//int index[100010][3];///临时变量存储的数组元素

map<pair<string,int>,int> global;///全局变量
int num;///全局变量数量

string label[100010];///四元式标号
int paranum=0;///当前函数参数个数
string op[100];

bool quanju[100010];///数组指针指示的数组是不是全局的


void preout(){///在生成主函数之前的前置输出
    printf("SSEG\tSEGMENT\tSTACK\n");
    printf("STK	DB\t100 DUP(0)\n");
    printf("SSEG\tENDS\n");
    printf("DSEG\tSEGMENT\n");
    for(map<pair<string,int>,int>::iterator it=global.begin();it!=global.end();it++){
        cout << (*it).first.first;
        printf("\t");
        if((*it).first.second==1)
            printf("DB");
        if((*it).first.second==2)
            printf("DW");
        else if((*it).first.second==4)
            printf("DD");
        if((*it).second!=-1)
            printf(" %d DUP(0)\n",(*it).second);
        else
            printf("\n");

    }
    printf("DSEG\tENDS\n");
    printf("CSEG\tSEGMENT\n");
    printf("\tASSUME\tCS:CSEG,DS:DSEG\n");
    printf("\tASSUME\tSS:SSEG\n");
}


void globalinit(){
    int f1=0;
    off=0;
    for(int i=0;i<=n;i++){
//        cout << "s" << i << "= " << s[i] << endl;
        if(s[i]==31)
        {
            if(getname(a[i][0])=="main"){
                preout();
                return ;
            }
            f1=1;
            continue;
        }
        else if(s[i]==32){
            f1=0;
            continue;
        }
        if(f1)
            continue;
        if(s[i]==45){///如果定义了s【i】类型的,即int，float之类
            string name=getname(a[i][0]);
            int type=getvalue(a[i][0]);
            int size1=getsize(a[i][0]);///返回数组字节数
            cout << "name=" << name << endl;
            if(isarr(a[i][0])){///为数组

                if(flag==0){
                    global.insert(make_pair(make_pair(name,type),getlength(a[i][0])));
                    insertaddr(a[i][0],off);
                    off+=getsize(a[i][0]);
                    num++;
                }
            }
            else {///为单个变量
                if(flag==0){
                    global.insert(make_pair(make_pair(name,type),-1));
                    insertaddr(a[i][0],off);
                    off+=getsize(a[i][0]);
                    num++;
                }
            }
        }
    }
}


void getblock(){///分配代码块

    memset(block,0,sizeof(block));
    block[1]=1;
    for(int i=2;i<=n;i++){
        if(s[i]==31||s[i]==33||s[i-1]==34||s[i-1]==38||s[i-1]==37||s[i-1]==35||s[i]==15)
            block[i]=1;
    }
}

void alive_init(){///初始化活跃信息
    for(int i=0;i<addrset.size();i++)
    {

        if(istem(addrset[i]))
            huoyue[addrset[i]]=0;
        else
            huoyue[addrset[i]]=1;


    }
}


void getalive(){///配置变量活跃信息
    addrset.clear();
    for(int i=1;i<=n;i++){
        if(s[i]==0||s[i]==1||s[i]==2||s[i]==3||s[i]==7||s[i]==8||s[i]==22||s[i]==9){

            addrset.push_back(a[i][0]);
            addrset.push_back(a[i][1]);
            addrset.push_back(a[i][2]);

        }
        else if(s[i]==14||s[i]==13){
            addrset.push_back(a[i][0]);
            addrset.push_back(a[i][2]);
        }

    }
    alive_init();
    for(int i=n;i>=1;i--){
        if(block[i+1]==1){
            alive_init();
        }
        else if(s[i]==0||s[i]==1||s[i]==2||s[i]==3||s[i]==7||s[i]==8||s[i]==22||s[i]==9||s[i]==12){
            c[i][2]=huoyue[a[i][2]];
            huoyue[a[i][2]]^=1;

            c[i][0]=huoyue[a[i][0]];
            huoyue[a[i][1]]=1;
            c[i][1]=huoyue[a[i][1]];
            huoyue[a[i][0]]=1;
//            printf("c[%d][%d]=%d\n",i,0,c[i][0]);
//            printf("c[%d][%d]=%d\n",i,1,c[i][1]);
//            printf("c[%d][%d]=%d\n",i,2,c[i][2]);

        }
        else if(s[i]==14||s[i]==13){
            c[i][2]=huoyue[a[i][2]];
            huoyue[a[i][2]]^=1;

            c[i][0]=huoyue[a[i][0]];
            huoyue[a[i][0]]=1;
//            printf("c[%d][%d]=%d\n",i,0,c[i][0]);
//            printf("c[%d][%d]=%d\n",i,2,c[i][2]);
        }
        else if(s[i]==56||s[i]==17){
            c[i][0]=huoyue[a[i][0]];
            huoyue[a[i][0]]=1;
        }
    }
}

string pou()
{
    string str="mark";
    cnt++;
    int tem[100];
    int n=0;
    int x=cnt;
    while(x)
    {
        tem[++n]=x%10;
        x/=10;
    }
    for(int i=n;i>0;i--)
        str.push_back('0'+tem[i]);
    return str;

}



void init()
{
    memset(quanju,0,sizeof quanju);
    op[0]="ADD";
    op[1]="SUB";
    op[7]="AND";
    op[8]="OR";
    op[22]="NOT";
    op[9]="XOR";
    op[5]="INC";
    op[6]="DEC";
    flag=0;
//    memset(index,0,sizeof(index));
    memset(huoyue,0,sizeof huoyue);
    memset(block,0,sizeof(block));
    num=0;
    for(int i=0;i<=100000;i++)
        temp[i]=-2;
    temp_off=0;
    seg=0;
    off=0;
    al=-1;
    ax=-1;
    cnt=0;
    paranum=0;
}


void from10to16(int x)
{
    stack<int> ss;
    if(x<0)
        printf("-"),x=-x;
    char s1[20]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    if(x==0)
        printf("0");
    while(x)
    {
        int tem=x%16;
        ss.push(tem);
        x/=16;
    }
    if(!ss.empty()&&ss.top()>=10)
        printf("0");
    while(!ss.empty()){
        int tem=ss.top();
        printf("%c",s1[tem]);
        ss.pop();
    }
    printf("H");
}

void long_cal(string s,int x)
{

    if(s=="ADD")
        s="ADC";
    else if(s=="SUB")
        s="SBB";
    printf("CLC\n");
    printf("\t");
    cout << s;
    if(isconst(x)){
        printf("\tAX,");
        from10to16(getnumber(x)%65535),printf("\n");
        cout << s;
        printf("\tDX,");
        from10to16(getnumber(x)>>16),printf("\n");
    }
    if(istem(x)){
        if(temp[x]==-1){///不是数组指针
            printf("\tAX,");
            printf("ES:["),from10to16(temp[x]),printf("]\n");
            cout << s;
            printf("\tDX,");
            printf("ES:["),from10to16(temp[x]+1),printf("]\n");

        }
        else{///是数组指针
            printf("\tAX,");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)),printf("]\n");
            cout << s;
            printf("\tDX,");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)+1),printf("]\n");
        }
        return ;
    }

    if(isparam(x)){
        x=getoffset(x);
        printf("\tAX,");
        printf("SS:[BP+6+"),from10to16(-x),printf("]\n");
        cout << s;
        printf("\tDX,");
        printf("SS:[BP+4+"),from10to16(-x),printf("]\n");
    } else {
        int p=x;
        x=getoffset(x);
        printf("\tAX,");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+"),from10to16(x),printf("]\n");
        cout << s;
        printf("\tDX,");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+2"),from10to16(x),printf("]\n");
    }
}

void long_sto(int x)///存储long型数据
{
    if(istem(x)){
        if(temp[x]==-1){///不是数组指针
            printf("\tMOV\t");
            printf("ES:["),from10to16(temp[x]),printf("],AX\n");
            printf("\tMOV\t");
            printf("ES:["),from10to16(temp[x]+1),printf("],DX\n");

        }
        else{///是数组指针
            printf("\tMOV\t");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)),printf("],AX\n");
            printf("\tMOV\t");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)+1),printf("],DX\n");
        }
        return ;
    }

    if(isparam(x)){
        x=getoffset(x);
        printf("\tMOV\t");
        printf("SS:[BP+6+"),from10to16(-x),printf("],AX\n");
        printf("\tMOV\t");
        printf("SS:[BP+4+"),from10to16(-x),printf("],DX\n");
    } else {
        int p=x;
        x=getoffset(x);
        printf("\tMOV\t");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+"),from10to16(x),printf("],AX\n");
        printf("\tMOV\t");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+2"),from10to16(x),printf("],DX\n");
    }

}

void long_mov(int x)///存储int型数据
{
    if(isconst(x)){
        printf("MOV\tAX,");
        from10to16(getnumber(x)%65535);
        printf("\n");

        printf("MOV\tDX,");
        from10to16(getnumber(x)>>16);
        printf("\n");

        return;
    }
    if(istem(x)){
        if(temp[x]==-1){///不是数组指针
            printf("MOV\tAX,");
            printf("ES:["),from10to16(temp[x]),printf("]\n");
            printf("MOV\tDX,");
            printf("ES:["),from10to16(temp[x]+1),printf("]\n");

        }
        else{///是数组指针
            printf("MOV\tAX,");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)),printf("]\n");
            printf("MOV\tDX,");
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)+1),printf("]\n");
        }
        return ;
    }

    if(isparam(x)){
        x=getoffset(x);
        printf("MOV\tAX,");
        printf("SS:[BP+6+"),from10to16(-x),printf("]\n");
        printf("MOV\tDX,");
        printf("SS:[BP+4+"),from10to16(-x),printf("]\n");
    } else {
        int p=x;
        x=getoffset(x);
        printf("MOV\tAX,");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+"),from10to16(x),printf("]\n");
        printf("MOV\tDX,");
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+2"),from10to16(x),printf("]\n");
    }
}

void getmark()
{
    int tem[100010];
    for(int i=1;i<=n;i++){
        if(s[i]==46)
            tem[a[i][0]]=i;
    }

    for(int i=1;i<=n;i++){
        if(s[i]==43||s[i]==37||s[i]==41||s[i]==36||s[i]==38||s[i]==33||s[i]==35||s[i]==34||s[i]==49||s[i]==48){///有跳转语句的直接加label
            label[a[i][2]]=pou();
        }
        else if(s[i]==47){
            a[i][2]=tem[a[i][2]];
            label[a[i][2]]=pou();
        }
    }
}

void pin(int x)
{
//    printf("istem(%d)=%d\n",x,istem(x));
    if(isconst(x)){
        from10to16(getnumber(x));
        return ;
    }
    if(istem(x)){
        if(!isaddr(x)||temp)///不是数组指针
            printf("ES:["),from10to16(temp[x]),printf("]");
        else///是数组指针
            printf("DS:[%s",quanju[x]==0?"DI+":""),from10to16(getaddr(x)),printf("]");
        return ;
    }
    if(isparam(x)){
        x=getoffset(x);
        printf("SS:[BP+4+"),from10to16(-x),printf("]");
    }
    else {
        int p=x;
        x=getoffset(x);
        printf("[%s",global.count(make_pair(getname(p),getvalue(p)))?"":"DI+"),from10to16(x),printf("]");
    }
}
void pok(int x){
    printf("\tMOV\tSI,");
    printf("ES:["),from10to16(temp[x]),printf("]\n");
    printf("\tMOV\tAX,[SI]\n");



}
void makeasm()
{
    init();
    freopen("siyuan.txt","r",stdin);
//    freopen("huibian.txt","w",stdout);
    n=-1;
    int tem;
    while(~scanf("%d",&tem))
    {
        ++n;
        s[n]=tem;
        for(int i=0;i<=2;i++)
            scanf("%d",&a[n][i]);
    }
//    n--;

//    printf("n=%d\n",n);
    globalinit();
    getblock();///分配代码块
    getalive();///分配活跃信息
    getmark();///label信息
    al=-1;aa=-1;ax=-1;
    alive=0;
    for(int i=0;i<=n;i++){
//        printf("s[%d]=%d\n",i,s[i]);


        if(s[i]==57){
            string s1=getasm(a[i][0]);
            cout << s1 << endl;
            continue;
        }
        if(s[i]==42)
            continue;

        if((ax!=-1||al!=-1||aa!=-1)){///若a中变量活跃，先存起来
            if(ax!=-1){
                long_sto(ax);
            }
            else if(aa!=-1){
                printf("\tMOV\t");
                pin(aa);
                printf(",AL\n");
            }
            else {
                printf("\tMOV\t");
                pin(al);
                printf(",AX\n");
            }
            alive=0,
            al=-1,ax=-1,aa=-1;

        }
        if(label[i]!="")
            cout <<  label[i],cout << ":";
//            printf("    ");

        if(block[i]==1){
            if(alive==1){
                if(ax!=-1){
                    long_sto(ax);
                    ax=-1;
                }
                else if(al!=-1){
                    printf("MOV\t");
                    pin(al);
                    printf(",AX\n");
                    al=-1;
                }
                else if(aa!=-1){
                    printf("MOV\t");
                    pin(aa);
                    printf(",AX\n");
                    al=-1;
                }

            }
        }
        string name;
        if(s[i]!=57)
            name=getname(a[i][0]);
        if(s[i]==45||s[i]==52){///如果定义了s【i】类型的,即int，float之类
            int type=getvalue(a[i][0]);
            int size1=getsize(a[i][0]);///返回数组字节数
            if(isarr(a[i][0])){///为数组
                if(flag==3&&s[i]==52){
                    soff+=size1;
                    continue;
                }
                if(flag==1)
                    insertaddr(a[i][0],off);
                if(flag==2)
                    insertaddr(a[i][0],off);
                if(flag==0){
                    global.insert(make_pair(make_pair(name,type),getlength(a[i][0])));
                    num++;
                    continue;
                }
                off+=size1;
            }
            else {///为单个变量
                if(flag==3&&s[i]==52){
                    soff+=type;
                    continue;
                }
                if(flag==1)
                    insertaddr(a[i][0],off);
                if(flag==2)
                    insertaddr(a[i][0],off);
                if(flag==0){
                    global.insert(make_pair(make_pair(name,type),-1));
                    num++;
                    continue;
                }
                off+=size1;
            }
        }
        else if(s[i]==31){///函数定义

            paranum=getparanum(a[i][0]);
            if(name=="main"){///是主函数
                flag=1;
                cout << "main:"  << endl;
                printf("\tMOV\tAX,CSEG\n");
                printf("\tADD\tAX,"),from10to16(2000),printf("\n");
                printf("\tMOV\tDS,AX\n");
                printf("\tADD\tAX,"),from10to16(2000),printf("\n");
                printf("\tMOV\tES,AX\n");

                printf("\tADD\tAX,"),from10to16(2000),printf("\n");
                printf("\tMOV\tSS,AX\n");

                printf("\tMOV\tDI,");///DI初始化
                from10to16(off);
                printf("\n");
                off=0;
            }
            else {///不是主函数
                flag=2;
                cout << name << "\t"  << "PROC\tNEAR"<< endl;
                seg_s.push(seg);
                off_s.push(off);
                seg+=off;
                off=0;
                printf("\tMOV\tBP,SP\n");
            }
        }
        else if(s[i]==32){///函数定义结束
            if(name=="main"){///是主函数
                flag=0;

            }
            else {///不是主函数
                flag=0;
                seg=seg_s.top();
                seg_s.pop();
                off=off_s.top();
                off_s.pop();
                cout << name;
                printf("\tENDP\n");
            }
        }
        else if(s[i]==16||s[i]==56){///函数返回
            if(flag==2){///函数返回值
                if(s[i]==56){
                    int type=getvalue(a[i][0]);
                    if(type==4){

                        long_mov(a[i][0]);
                        continue;
                    }
                    else{

                        printf("\tMOV\tA%c,",type==2?'X':'L');
                        pin(a[i][0]);
                        printf("\n");
                    }
                }

                printf("\tMOV\tDI,SS:[BP+2]\n");

                printf("\tRET\n");
            }

        }
        else if(s[i]==0||s[i]==1||s[i]==2||s[i]==3||s[i]==7||s[i]==8||s[i]==22||s[i]==9||s[i]==14||s[i]==13||s[i]==5||s[i]==6||s[i]==4||s[i]==10||s[i]==11){///操作符还没写全

            if(istem(a[i][2])){///如果结果是临时变量，为其分配地址
                if(temp[a[i][2]]==-2)
                    temp[a[i][2]]=temp_off,temp_off+=getvalue(a[i][2]);///将临时变量存起来并将ES段偏移量加上size
            }


            if(s[i]==22){
                if(getvalue(a[i][0])==4){
                    long_mov(a[i][0]);
                    printf("\tXOR\tAX,0H\nXOR\tDX,0H\n");
                    long_sto(a[i][2]);
                }else {
                    char s1[10];s1[0]='A',s1[2]='\0';
                    if(getvalue(a[i][0])==1)s1[1]='L';
                    else s1[1]='X';
                    printf("\tMOV\t%s,",s1);
                    pin(a[i][0]);
                    printf("\n");

                    printf("\tXOR\t%s,0H\n",s1);

                    printf("\tMOV\t");
                    pin(a[i][2]);
                    printf(",%s\n",s1);
                }

            }


            if(s[i]==5||s[i]==6){
                if(getvalue(a[i][0])==4){
                    long_mov(a[i][0]);
                    if(s[i]==5)
                        printf("\tINC\tAX\n\tADC\t DX,0H\n");
                    else if(s[i]==6)
                        printf("\tDEC\tAX\n\tABB\t DX,0H\n");
                    alive=c[i][2];
                    ax=a[i][2];
                    al=-1,aa=-1;
                }
                else {
                    char s1[10];s1[0]='A',s1[2]='\0';
                    if(getvalue(a[i][0])==1)s1[1]='L';
                    else s1[1]='X';
                    printf("\tMOV\t%s,",s1);
                    pin(a[i][0]);
                    printf("\n");


                    printf("\t");
                    cout << op[s[i]];
                    printf("\t%s\n",s1);

                    printf("\tMOV\t");
                    pin(a[i][2]);
                    printf(",%s\n",s1);


                }
                continue;

            }



            if(s[i]==13){///a[i][2]=&a[i][0]
                printf("\tMOV\t");
                pin(a[i][2]);
                printf(",");
                from10to16(getaddr(a[i][0]));
                printf("\n");
                continue;
            }
            if(s[i]==14){///a[i][2]=*a[i][0]
                printf("\tXOR\tAH,AH\n");
                char s1[10];s1[1]='\0';
                if(getvalue(a[i][0])==1)s1[0]='L';
                else s1[0]='X';
                printf("\tMOV\tA%s",s1);
                pin(a[i][0]);
                printf("\n");
                printf("\tMOV\tAX,[AX]\n");
                continue;
            }
            if(s[i]==0||s[i]==1||s[i]==7||s[i]==8||s[i]==22||s[i]==9){///加减法和逻辑运算
                if(getvalue(a[i][0])==4){///如果是long型就特殊判断
                    long_mov(a[i][0]);
                    long_cal(op[s[i]],a[i][1]);
                    alive=c[i][2];
                    al=-1;
                    aa=-1;
                    ax=a[i][2];
                }
                else {
                    char s1[10];s1[1]='\0';
                    if(getvalue(a[i][0])==1)s1[0]='L';
                    else s1[0]='X';
                    printf("\tMOV\tA%s,",s1);
                    pin(a[i][0]);
                    printf("\n");

                    printf("\tMOV\tB%s,",s1);
                    pin(a[i][1]);
                    printf("\n");

                    printf("\t");
                    cout << op[s[i]];
                    printf("\tA%s,B%s\n",s1,s1);

                    alive=c[i][2];
//                    cout << "alive=" << alive << endl;
                    if(s1[0]=='L')
                        aa=a[i][2],al=-1,ax=-1;
                    else if(s1[0]=='X')
                        al=a[i][2],aa=-1,ax=-1;
                }
            }


            if(s[i]==2){
                char s1[10];s1[1]='\0';
                if(getvalue(a[i][0])==1)s1[0]='L';
                else s1[0]='X';
                printf("\tMOV\t");
                pin(a[i][0]);
                printf("A%s\n",s1);


                printf("\tMOV\t");
                pin(a[i][1]);
                printf("B%s\n",s1);

                printf("\tMUL\tB%s\n",s1);

                alive=c[i][2];
                if(s1[0]=='L')
                    al=a[i][2],aa=-1,ax=-1;
                else if(s1[0]=='X')
                    ax=a[i][2],aa=-1,al=-1;
            }
            else if(s[i]==3||s[i]==4){

                if(getvalue(a[i][0])==4){
                    long_mov(a[i][0]);
                    printf("\tMOV\tBX,");
                    pin(a[i][1]);
                    printf("\n");
                    printf("\tDIV\tBX\n");
                    if(s[i]==4)
                        printf("\tMOV\tAX,DX");
                    alive=c[i][2];
                    al=a[i][2];
                    ax=-1,aa=-1;
                }else if(getvalue(a[i][0])==2){
                    printf("\tMOV\tAX,");
                    pin(a[i][0]);
                    printf("\n\tMOV\tBL,");
                    pin(a[i][1]);
                    printf("\n");
                    printf("\tDIV\tBL\n");
                    if(s[i]==4)
                        printf("\tMOV\tAL,DL");
                    alive=c[i][2];
                    al=a[i][2];
                    ax=-1,aa=-1;
                }
            }
            else if(s[i]==10||s[i]==11){

                if(getvalue(a[i][0])==2){
                    printf("\tMOV\tAX,");
                    pin(a[i][0]);
                    printf("\n\tMOV\tCX,");
                    pin(a[i][1]);
                    printf("\n");

                    if(s[i]==10)
                        printf("\tSAL\tAX,CX\n");
                    else if(s[i]==11)
                        printf("\tSHR\tAX,CX\n");
                    alive=c[i][2];
                    al=a[i][2];
                    ax=-1,aa=-1;
                }else if(getvalue(a[i][0])==1){
                    printf("\tMOV\tAL,");
                    pin(a[i][0]);
                    printf("\n\tMOV\tCX,");
                    pin(a[i][1]);
                    printf("\n");
                    if(s[i]==10)
                        printf("\tSAL\tAL,CX\n");
                    else if(s[i]==11)
                        printf("\tSHR\tAL,CX\n");
                    alive=c[i][2];
                    aa=a[i][2];
                    ax=-1,al=-1;
                }
            }
        }
        else if(s[i]==12||s[i]==20||s[i]==19){///赋值操作符

            if(isaddr(a[i][2])){
                printf("\tMOV\tSI,");
                pin(a[i][2]);
                printf("\n");

                char s1[10];s1[0]='B',s1[2]='\0';
                if(getvalue(a[i][0])==1)s1[1]='L';
                else s1[1]='X';

                printf("\tMOV\t%s,",s1);
                pin(a[i][0]);
                printf("\n");

                if(s[i]==19)
                    printf("\tNEG\t%s\n",s1);


                printf("\tMOV\t[SI]");
                printf(",%s\n",s1);
                continue;
            }

            if(getvalue(a[i][2])==4){
                long_mov(a[i][0]);
                if(s[i]==19)
                    printf("\tNEG\tAX\n\tNEG\tDX\n");
                long_sto(a[i][2]);
                continue;
            }


            char s1[10];s1[0]='B',s1[2]='\0';
            if(getvalue(a[i][2])==1)s1[1]='L';
            else s1[1]='X';

            printf("\tMOV\t%s,",s1);
            pin(a[i][0]);
            printf("\n");

            if(s[i]==19)
                printf("\tNEG\t%s\n",s1);


            printf("\tMOV\t");
            pin(a[i][2]);
            printf(",%s\n",s1);
        }
        else if(s[i]==17){///参数入栈
            if(getvalue(a[i][0])==4){
                long_mov(a[i][0]);
                printf("\tPUSH\tAX\n");
                printf("\tPUSH\tDX\n");
            }
            else if(getvalue(a[i][0])==1){
                printf("\tMOV\tAL,");
                pin(a[i][0]);
                printf("\n");
                printf("\tXOR\tAH,AH\n");
                printf("\tPUSH\tAX\n");
            }
            else if(getvalue(a[i][0])==2){
                if(!isaddr(a[i][0])) {
                    printf("\tMOV\tAX,");
                    pin(a[i][0]);
                    printf("\n");
                }else {
                    pok(a[i][0]);
                }

                printf("\tPUSH\tAX\n");
            }
        }
        else if(s[i]==15){///调用函数


            printf("\tPUSH\tDI\n");
            printf("\tADD\tDI,");

            from10to16(off);
            printf("\n");


            printf("\tCALL\t");
            cout << name;
            printf("\n");
            if(istem(a[i][2])){
                if(temp[a[i][2]]==-2)
                    temp[a[i][2]]=temp_off,temp_off+=getvalue(a[i][2]);
            }

            printf("\tPOP\t DX\n");
            for(int i=1;i<=getparanum(a[i][0]);i++)
                printf("\tPOP\t DX\n");
            printf("\tMOV\tBP,SP\n");

            if(getvalue(a[i][2])==4)
                long_sto(a[i][2]);
            else if(getvalue(a[i][2])==2)
                printf("\tMOV\t"),pin(a[i][2]),printf(",AX\n");
            else if(getvalue(a[i][2])==1)
                printf("\tMOV\t"),pin(a[i][2]),printf(",AL\n");


        }
        else if(s[i]==33){///if开始
            printf("\tMOV\tAX,");
            pin(a[i][0]);
            printf("\n");
            printf("\tAND\tAX,AX\n");

            printf("\tJZ\t");
            cout << label[a[i][2]] << endl;
        }
        else if(s[i]==35){///else
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;

        }
        else if(s[i]==34){///if结束

        }
        else if(s[i]==36){///while开始

        }
        else if(s[i]==37){///while判断


            printf("\tMOV\tAX,");
            pin(a[i][0]);
            printf("\n");
            printf("\tAND\tAX,AX\n");

            printf("\tJZ\t");
            cout << label[a[i][2]] << endl;
        }
        else if(s[i]==38){///while结束
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;
        }
        else if(s[i]==27||s[i]==28||s[i]==23||s[i]==25||s[i]==24||s[i]==26||s[i]==29||s[i]==30||s[i]==21){///关系算符

            if(istem(a[i][2])){///如果结果是临时变量，为其分配地址
                if(temp[a[i][2]]==-2)
                    temp[a[i][2]]=temp_off,temp_off+=getvalue(a[i][2]);///将临时变量存起来并将ES段偏移量加上size
            }
            if((ax!=-1||al!=-1||aa!=-1)){///若a中变量活跃，先存起来
                if(ax!=-1){
                    long_sto(ax);
                }
                else if(aa!=-1){
                    printf("\tMOV\t");
                    pin(aa);
                    printf(",AL\n");
                }
                else {
                    printf("\tMOV\t");
                    pin(al);
                    printf(",AX\n");
                }
            }
            if(s[i]==27||s[i]==28||s[i]==23||s[i]==25||s[i]==24||s[i]==26){
                if(getvalue(a[i][0])==4){

                    long_mov(a[i][0]);
                    long_cal("SUB",a[i][1]);
                    string s1=pou();
                    if(label[i+1]=="")label[i+1]=pou();
                    string s2=label[i+1];
                    if(s[i]==27)printf("\tJNZ\t");
                    if(s[i]==28)printf("\tJZ\t");
                    if(s[i]==23)printf("\tJAE\t");
                    if(s[i]==25)printf("\tJA\t");
                    if(s[i]==24)printf("\tJBE\t");
                    if(s[i]==26)printf("\tJB\t");
                    cout << s1 << endl;
                    printf("\tMOV\t");
                    pin(a[i][2]);
                    printf(",01H\n\tJMP\t");
                    cout << s2 << endl;
                    cout << s1 ;
                    printf(":\tMOV\t");
                    pin(a[i][2]);
                    printf(",00H\n");

                }
                else {
                    int tem=getvalue(a[i][0]);
                    printf("\tMOV\tA%c,",tem==2?'X':'L');
                    pin(a[i][0]);
                    printf("\n");

                    printf("\tMOV\tB%c,",tem==2?'X':'L');
                    pin(a[i][1]);
                    printf("\n");

                    printf("\tCMP\tA%c,B%c\n",tem==2?'X':'L',tem==2?'X':'L');

                    string s1=pou();
                    if(label[i+1]=="")label[i+1]=pou();
                    string s2=label[i+1];
                    if(s[i]==27)printf("\tJNZ\t");
                    if(s[i]==28)printf("\tJZ\t");
                    if(s[i]==23)printf("\tJAE\t");
                    if(s[i]==25)printf("\tJA\t");
                    if(s[i]==24)printf("\tJBE\t");
                    if(s[i]==26)printf("\tJB\t");
                    cout << s1 << endl;
                    printf("\tMOV\t");
                    pin(a[i][2]);
                    printf(",01H\n\tJMP\t");
                    cout << s2 << endl;
                    cout << s1 ;
                    printf(":\tMOV\t");
                    pin(a[i][2]);
                    printf(",00H\n");
                }

            }
            else if(s[i]==29||s[i]==30){///不支持long型数据
                printf("\tMOV\tAX,");
                pin(a[i][0]);
                printf("\n");

                if(s[i]==29)printf("\tAND\tAX,");
                if(s[i]==30)printf("\tOR\tAX,");
                pin(a[i][1]);
                printf("\n");
                string s1=pou();
                if(label[i+1]=="")label[i+1]=pou();
                string s2=label[i+1];
                printf("\tJZ\t");
                cout << s1 << endl;
                printf("\tMOV\t");
                pin(a[i][2]);
                printf(",01H\n\tJMP\t");
                cout << s2 << endl;
                cout << s1 ;
                printf(":\tMOV\t");
                pin(a[i][2]);
                printf(",00H\n");


            }
            else if(s[i]==21){
                printf("\tMOV\tAX,");
                pin(a[i][0]);
                printf("\n");
                printf("\tAND\tAX,AX");
                string s1=pou();
                if(label[i+1]=="")label[i+1]=pou();
                string s2=label[i+1];
                printf("\tJZ\t");
                cout << s1 << endl;
                printf("\tMOV\t");
                pin(a[i][2]);
                printf(",00H\n\tJMP\t");
                cout << s2 << endl;
                cout << s1 ;
                printf(":\tMOV\t");
                pin(a[i][2]);
                printf(",01H\n");

            }
        }
        else if(s[i]==50||s[i]==54){

            if(istem(a[i][2])){
                if(temp[a[i][2]]==-2){
                    temp[a[i][2]]=temp_off;
                    temp_off+=2;///将临时变量存起来并将ES段偏移量加上size
                }

            }

            int to=a[i][2];
            if(s[i]==50){
                if(isarr(a[i][0])){
                    if(!global.count(make_pair(getname(a[i][0]),getvalue(a[i][0]))))
                        printf("\tMOV\tAX,DI\n"),quanju[a[i][2]]=0;
                    else
                        quanju[a[i][2]]=1;
                    //printf("%d %d %d\n",a[i][0],getoffset(a[i][0]),getaddr(a[i][0]))
                    printf("\tADD\tAX,");
                    from10to16(getaddr(a[i][0]));
                    printf("\n");
                }else if(isaddr(a[i][0])){
                    quanju[a[i][2]]=quanju[a[i][0]];
                    printf("\tADD\tAX,");
                    pin(a[i][0]);
                    printf("\n");
                }
                printf("\tMOV\tBX,AX\n");
                printf("\tMOV\tAX,");
                pin(a[i][1]);
                printf("\n");

                printf("\tMOV\tCX,"),from10to16(getarrsize(a[i][0])),printf("\n");

                printf("\tMUL\tCX\n");
                printf("\tADD\tAX,BX\n");

                printf("\tMOV\t");
                pin(a[i][2]);
                printf(",AX\n");
            }
            else if(s[i]==54){
                int pp=getaddr(a[i][0])+getnumber(a[i][1]);///存储结构体某个成员的地址
                insertaddr(a[i][2],pp);
            }



        }
        else if(s[i]==51){
            flag=3;
            sname=name;
            soff=0;
        }
        else if(s[i]==53){
            flag=0;
//            typesize.insert(make_pair(sname,soff));
            sname="";
            soff=0;
        }
        else if(s[i]==43||s[i]==49||s[i]==48||s[i]==47||s[i]==41){
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;
        }

    }
    printf("\tMOV\tAH,4CH\n\tINT\t21H\nCSEG\tENDS\nEND main\n");

}

//int main(){
//    makeasm();
//    printf("\tMOV\tAH,4CH\n\tINT\t21H\nCSEG\tENDS\nEND MAIN\n");
//}
