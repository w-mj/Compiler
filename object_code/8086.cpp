//
// Created by wmj on 18-12-20.
//

#include "8086.h"

// #include"siyuan.h"
#include <cstdio>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <iostream>
#include <cstring>


#define arr 10


using namespace std;

void insertaddr(int a,int b){}///将符号表中a号变量的首地址赋为b
void setoffset(int a,int b){}///将符号表中a号变量的首地址赋为b
int getaddr(int a){return 0;}///返回符号表中a号变量的首地址
int getoffset(int a){return 15;}///返回符号表a号变量的偏移量
int getparanum(int a){return 0;}///返回符号表中a号函数的参数个数
std::string getvalue(int a){return "int";}///返回符号表中a号函数的返回值类型或者a号变量的类型
bool istem(int a){return false;}///返回符号表a号变量是否是临时变量
std::string getname(int a){if(a==2)return "MAIN";else return "gcd";}///返回符号表a号变量的名字


string s[10010];
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

map<string,int> typesize;


int cnt=0;///标号


vector<int> addrset;///地址集合
int huoyue[100010];///存储每个地址的活跃信息
int block[100010];///第i条四元式是不是基本块入口
int temp[100010];///在ES段分配临时变量的地址
int temp_off=0;
int index_1[100010];///临时变量存储的数组元素

map<pair<string,int>,int> global;///全局变量
int num;///全局变量数量

string label[100010];///四元式标号
int paranum=0;///当前函数参数个数

void preout(){///在生成主函数之前的前置输出
    printf("SSEG\tSEGMENT\tSTACK\n");
    printf("STK	DB\t100 DUP(0)\n");
    printf("SSEG\tENDS\n");
    printf("DSEG\tSEGMENT\n");
    for(map<pair<string,int>,int>::iterator it=global.begin();it!=global.end();it++){
        cout << (*it).first.first;
        printf("\t");
        if((*it).first.second==2)
            printf("DW");
        else if((*it).first.second==4)
            printf("DD");
        else if((*it).first.second>4){
            printf("DW %dDUP(0)\n",(*it).first.second/2);
            continue;
        }
        if((*it).second!=-1)
            printf("%dDUP(0)\n",(*it).second);
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
    for(int i=1;i<=n;i++){
        if(s[i]=="FUNC")
        {
            if(getname(a[i][0])=="MAIN"){
                preout();
                return ;
            }
            f1=1;
            continue;
        }
        else if(s[i]=="FUNCE"){
            f1=0;
            continue;
        }
        if(f1)
            continue;
        if(typesize.count(s[i])){///如果定义了s【i】类型的,即int，float之类
            string name=getname(a[i][0]);
            if(a[i][1]==arr){///为数组

                if(flag==0){
                    global.insert(make_pair(make_pair(name,typesize[s[i]]),a[i][2]));
                    num++;
                }
            }
            else {///为单个变量
                if(flag==0){
                    global.insert(make_pair(make_pair(name,typesize[s[i]]),-1));
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
        if(s[i]=="FUNC"||s[i]=="IF"||s[i-1]=="IE"||s[i-1]=="WE"||s[i-1]=="DO"||s[i-1]=="EL")
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
        if(s[i]=="ADD"||s[i]=="SUB"||s[i]=="MUL"||s[i]=="DIV"||s[i]=="AND"||s[i]=="OR"||s[i]=="NOT"||s[i]=="XOR"){

            addrset.push_back(a[i][0]);
            addrset.push_back(a[i][1]);
            addrset.push_back(a[i][2]);

        }
        else if(s[i]=="*"||s[i]=="&"){
            addrset.push_back(a[i][0]);
            addrset.push_back(a[i][2]);
        }

    }
    alive_init();
    for(int i=n;i>=1;i--){
        if(block[i+1]==1){
            alive_init();
        }
        else if(s[i]=="ADD"||s[i]=="SUB"||s[i]=="MUL"||s[i]=="DIV"||s[i]=="AND"||s[i]=="OR"||s[i]=="NOT"||s[i]=="XOR"){
            huoyue[a[i][2]]^=1;
            c[i][2]=huoyue[a[i][2]];

            huoyue[a[i][0]]=1;
            c[i][0]=huoyue[a[i][0]];
            huoyue[a[i][1]]=1;
            c[i][1]=huoyue[a[i][1]];

        }
        else if(s[i]=="*"||s[i]=="&"){
            huoyue[a[i][2]]^=1;
            c[i][2]=huoyue[a[i][2]];

            huoyue[a[i][0]]=1;
            c[i][0]=huoyue[a[i][0]];
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
        str.push_back(static_cast<char>('0' + tem[i]));
    return str;

}



void init()
{
    flag=0;
    memset(huoyue,0,sizeof huoyue);
    memset(block,0,sizeof(block));
    num=0;
    for(int i=0;i<=100000;i++)
        temp[i]=-1, index_1[i]=-1;
    temp_off=0;
    typesize.insert(make_pair("SHORT",1));
    typesize.insert(make_pair("INT",2));
    typesize.insert(make_pair("LONG",4));
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
    if(ss.top()>=10)
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

    cout << s;
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
            printf("DS:["),from10to16(index_1[x]),printf("]\n");
            cout << s;
            printf("\tDX,");
            printf("DS:["),from10to16(index_1[x]+1),printf("]\n");
        }
        return ;
    }
    x=getoffset(x);
    if(x<0){
        printf("\tAX,");
        printf("SS:[SI+2+"),from10to16(-x),printf("]\n");
        cout << s;
        printf("\tDX,");
        printf("SS:[SI+1+"),from10to16(-x),printf("]\n");
    } else {

        printf("\tAX,");
        printf("[DI+"),from10to16(x),printf("]\n");
        cout << s;
        printf("\tDX,");
        printf("[DI+1"),from10to16(x),printf("]\n");
    }
}

void long_sto(int x)///存储long型数据
{
    if(istem(x)){
        if(temp[x]==-1){///不是数组指针
            printf("MOV\t");
            printf("ES:["),from10to16(temp[x]),printf("],AX\n");
            printf("MOV\t");
            printf("ES:["),from10to16(temp[x]+1),printf("],DX\n");

        }
        else{///是数组指针
            printf("MOV\t");
            printf("DS:["),from10to16(index_1[x]),printf("],AX\n");
            printf("MOV\t");
            printf("DS:["),from10to16(index_1[x]+1),printf("],DX\n");
        }
        return ;
    }
    x=getoffset(x);
    if(x<0){
        printf("MOV\t");
        printf("SS:[SI+2+"),from10to16(-x),printf("],AX\n");
        printf("MOV\t");
        printf("SS:[SI+1+"),from10to16(-x),printf("],DX\n");
    } else {

        printf("MOV\t");
        printf("[DI+"),from10to16(x),printf("],AX\n");
        printf("MOV\t");
        printf("[DI+1"),from10to16(x),printf("],DX\n");
    }

}

void long_mov(int x)///存储int型数据
{
    if(istem(x)){
        if(temp[x]==-1){///不是数组指针
            printf("MOV\tAX,");
            printf("ES:["),from10to16(temp[x]),printf("]\n");
            printf("MOV\tDX,");
            printf("ES:["),from10to16(temp[x]+1),printf("]\n");

        }
        else{///是数组指针
            printf("MOV\tAX,");
            printf("DS:["),from10to16(index_1[x]),printf("]\n");
            printf("MOV\tDX,");
            printf("DS:["),from10to16(index_1[x]+1),printf("]\n");
        }
        return ;
    }
    x=getoffset(x);
    if(x<0){
        printf("MOV\tAX,");
        printf("SS:[SI+2+"),from10to16(-x),printf("]\n");
        printf("MOV\tDX,");
        printf("SS:[SI+1+"),from10to16(-x),printf("]\n");
    } else {

        printf("MOV\tAX,");
        printf("[DI+"),from10to16(x),printf("]\n");
        printf("MOV\tDX,");
        printf("[DI+1"),from10to16(x),printf("]\n");
    }
}

void getmark()
{
    for(int i=1;i<=n;i++){
        if(s[i]=="JMP"||s[i]=="DO"||s[i]=="EF"||s[i]=="WH"||s[i]=="WE"||s[i]=="IF"||s[i]=="EL"||s[i]=="EI"){///有跳转语句的直接加label
            label[a[i][2]]=pou();
        }
    }
}

void pin(int x)
{
    if(istem(x)){
        if(temp[x]==-1)///不是数组指针
            printf("ES:["),from10to16(temp[x]),printf("]");
        else///是数组指针
            printf("DS:["),from10to16(index_1[x]),printf("]");
        return ;
    }
    x=getoffset(x);
    if(x<0){
        printf("SS:[SI+2+"),from10to16(-x),printf("]");
    }
    else {
        printf("[DI+"),from10to16(x),printf("]");
    }
}

void makeasm()
{

    init();
    freopen("4yuan.txt","r",stdin);
//    freopen("huibian.txt","w",stdout);
    n=0;
    string ch;
    while(cin>>ch)
    {
        s[++n]=ch;
        for(int i=0;i<=2;i++)
            scanf("%d",&a[n][i]);
    }
    globalinit();
    getblock();///分配代码块
    getalive();///分配活跃信息
    getmark();///label信息

    for(int i=1;i<=n;i++){
//        printf("i=%d\n",i);
        if(label[i]!="")
            cout <<  label[i];
//            printf("    ");

        if(block[i]==1){
            if(alive==1){
                if(ax!=-1){
                    long_sto(ax);
                    ax=-1,al=-1;
                }
                else {
                    printf("MOV\t");
                    pin(al);
                    printf(",AL\n");
                }

            }
        }

        string name=getname(a[i][0]);
        if(typesize.count(s[i])){///如果定义了s【i】类型的,即int，float之类
            if(a[i][1]==arr){///为数组
                if(flag==3){
                    soff+=typesize[s[i]]*a[i][2];
                    continue;
                }
                if(flag==1)
                    insertaddr(a[i][0],seg+off);
                off+=typesize[s[i]]*a[i][2];
                if(flag==0){
                    global.insert(make_pair(make_pair(name,typesize[s[i]]),a[i][2]));
                    num++;
                }
            }
            else {///为单个变量
                if(flag==3){
                    soff+=typesize[s[i]];
                    continue;
                }
                if(flag==1)
                    insertaddr(a[i][0],seg+off);
                off+=typesize[s[i]];
                if(flag==0){
                    global.insert(make_pair(make_pair(name,typesize[s[i]]),-1));
                    num++;
                }
            }
        }
        else if(s[i]=="FUNC"){///函数定义

            paranum=getparanum(a[i][0]);
            if(name=="MAIN"){///是主函数
                flag=1;
                cout << "MAIN"  << endl;
                printf("\tXOR\tDI,DI\n");///DI初始化
            }
            else {///不是主函数
                flag=2;
                cout << name << "\t"  << "PROC\tNEAR"<< endl;
                seg_s.push(seg);
                off_s.push(off);
                seg+=off;
                printf("\tPUSH\tDI\n");
                printf("\tADD\tDI,");
                from10to16(off);
                printf("\n");
                off=0;
                printf("\tMOV\tSI,SP\n");
            }
        }
        else if(s[i]=="FUNCE"){///函数定义结束
            if(name=="MAIN"){///是主函数
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
        else if(s[i]=="RET"){///函数返回
            if(flag==2){///函数返回值
                int type=typesize[getvalue(a[i][0])];
                if(type==4){

                    long_mov(a[0][0]);
                    continue;
                }
                else{
                    printf("\tMOV\tAX,");
                    pin(a[0][0]);
                    printf("\n");
                }
                printf("\tPOP\tDI\n");

                printf("\tRET\n");
            }

        }
        else if(s[i]=="ADD"||s[i]=="SUB"||s[i]=="MUL"||s[i]=="DIV"||s[i]=="AND"||s[i]=="OR"||s[i]=="NOT"||s[i]=="XOR"||s[i]=="*"||s[i]=="&"||s[i]=="INC"||s[i]=="DEC"||s[i]=="MOD"){///操作符还没写全
            if((ax!=-1||al!=-1||aa!=-1)&&alive){///若a中变量活跃，先存起来
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
            if(s[i]=="INC"||s[i]=="DEC"){
                if(typesize[getvalue(a[i][0])]==4){
                    long_mov(a[i][0]);
                    printf("\t");
                    //////////////////////////////////long_cal(s[i],a[i][1]);
                }
                else {
                    char s1[10];s1[0]='A',s1[2]='\0';
                    if(typesize[getvalue(a[i][0])]==1)s1[1]='L';
                    else s1[1]='L';
                    printf("\tMOV\t%s,",s1);
                    pin(a[i][0]);
                    printf("\n");

                    printf("\t");

                    cout << s[i];
                    printf("\t%s\n",s1);

                    printf("\tMOV\t");
                    pin(a[i][0]);
                    printf(",%s\n",s1);
                }
                continue;

            }

            if(istem(a[i][2])){///如果结果是临时变量，为其分配地址
                if(temp[a[i][2]]==-1)
                    temp[a[i][2]]=temp_off,temp_off+=typesize[getvalue(a[i][0])];///将临时变量存起来并将ES段偏移量加上size
            }

            if(s[i]=="&"){///a[i][2]=&a[i][0]
                printf("\tMOV\t");
                pin(a[i][2]);
                printf(",");
                from10to16(getoffset(a[i][0]));
                printf("\n");
                continue;
            }
            if(s[i]=="*"){///*a[i][2]=a[i][0]
                setoffset(a[i][2],getoffset(a[i][0]));
                continue;
            }
            if(s[i]=="ADD"||s[i]=="SUB"||s[i]=="AND"||s[i]=="OR"||s[i]=="NOT"||s[i]=="XOR"){///加减法
                if(typesize[getvalue(a[i][0])]==4){///如果是long型就特殊判断
                    long_mov(a[i][0]);
                    long_cal(s[i],a[i][1]);
                    alive=c[i][2];
                    al=-1;
                    aa=-1;
                    ax=a[i][2];
                }
                else {
                    char s1[10];s1[1]='\0';
                    if(typesize[getvalue(a[i][0])]==1)s1[0]='L';
                    else s1[0]='X';
                    printf("\tMOV\tA%s,",s1);
                    pin(a[i][0]);
                    printf("\n");

                    printf("\tMOV\tB%s,",s1);
                    pin(a[i][1]);
                    printf("\n");

                    cout << s[i];
                    printf("\tA%s,B%s\n",s1,s1);

                    alive=c[i][2];
                    if(s1[0]=='L')
                        aa=a[i][2],al=-1,ax=-1;
                    else if(s1[0]=='X')
                        al=a[i][2],aa=-1,ax=-1;
                }
            }
            if(s[i]=="MUL"&&s[i]=="DIV"){


                if(s[i]=="MUL"){
                    char s1[10];s1[1]='\0';
                    if(typesize[getvalue(a[i][0])]==1)s1[0]='L';
                    else s1[0]='X';
                    printf("MOV\t");
                    pin(a[i][0]);
                    printf("A%s\n",s1);


                    printf("MOV\t");
                    pin(a[i][1]);
                    printf("B%s\n",s1);

                    printf("MUL\tB%s\n",s1);

                    alive=c[i][2];
                    if(s1[0]=='L')
                        al=a[i][2],aa=-1,ax=-1;
                    else if(s1[0]=='X')
                        ax=a[i][2],aa=-1,al=-1;
                }
                else if(s[i]=="DIV"||s[i]=="MOD"){

                    if(typesize[getvalue(a[i][0])]==4){
                        long_mov(a[i][0]);
                        printf("MOV\tBX,");
                        pin(a[i][1]);
                        printf("\n");
                        printf("DIV\tBX\n");
                        if(s[i]=="MOD")
                            printf("MOV\tAX,DX");
                        alive=c[i][2];
                        al=a[i][2];
                        ax=-1,aa=-1;
                    }else if(typesize[getvalue(a[i][0])]==2){
                        printf("MOV\tAX,");
                        pin(a[i][0]);
                        printf("\n\tMOV\tBL,");
                        pin(a[i][1]);
                        printf("\n");
                        printf("DIV\tBL\n");
                        if(s[i]=="MOD")
                            printf("MOV\tAL,DL");
                        alive=c[i][2];
                        aa=a[i][2];
                        ax=-1,al=-1;
                    }
                }


            }


        }
        else if(s[i]=="=="){///赋值操作符

            if(typesize[getvalue(a[i][0])]==4){
                long_mov(a[i][0]);
                long_sto(a[i][2]);
                continue;
            }
            printf("\tMOV\tBX,");
            pin(a[i][0]);
            printf("\n");

            printf("\tMOV\t");
            pin(a[i][2]);
            printf(",BX\n");
        }
        else if(s[i]=="PUSH"){///参数入栈
            printf("\tPUSH\t");
            pin(a[i][0]);
            printf("\n");
        }
        else if(s[i]=="CALL"){///调用函数
            printf("\tCALL\t");
            cout << name;
            printf("\n");

            for(int i=1;i<=getparanum(a[i][0]);i++)
                printf("POP\t DX\n");

        }
        else if(s[i]=="IF"){///if开始
            if(s[i-1]=="EQU")printf("\tJNE\t");
            if(s[i-1]=="NEQ")printf("\tJE\t");
            if(s[i-1]=="GT")printf("\tJBE\t");
            if(s[i-1]=="GE")printf("\tJB\t");
            if(s[i-1]=="LT")printf("\tJAE\t");
            if(s[i-1]=="LE")printf("\tJA\t");
            cout << label[a[i][2]] << endl;
        }
        else if(s[i]=="EL"){///else
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;

        }
        else if(s[i]=="IE"){///if结束

        }
        else if(s[i]=="WH"){///while开始

        }
        else if(s[i]=="DO"){///while判断
            if(s[i-1]=="EQU")printf("\tJNE\t");
            if(s[i-1]=="NEQ")printf("\tJE\t");
            if(s[i-1]=="GT")printf("\tJBE\t");
            if(s[i-1]=="GE")printf("\tJB\t");
            if(s[i-1]=="LT")printf("\tJAE\t");
            if(s[i-1]=="LE")printf("\tJA\t");
            cout << label[a[i][2]] << endl;


        }
        else if(s[i]=="WE"){///while结束
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;
        }
        else if(s[i]=="EQU"||s[i]=="NEQ"||s[i]=="LT"||s[i]=="LE"||s[i]=="GT"||s[i]=="GE"){///关系算符
            printf("\tMOV\tAX,");
            pin(a[i][0]);
            printf("\n");

            printf("\tCMP\tAX,");
            pin(a[i][1]);
            printf("\n");
        }
        else if(s[i]=="[]"){
            if(istem(a[i][2])){
                if(temp[a[i][2]]==-1)
                    temp[a[i][2]]=temp_off,temp_off+=2;///将临时变量存起来并将ES段偏移量加上size
            }
            index_1[a[i][2]]=getaddr(a[i][0])+(a[i][1]*typesize[getvalue(a[i][0])]);///存储数组某个索引的地址
        }
        else if(s[i]=="STRUCT"){
            flag=3;
            sname=name;
            soff=0;

        }
        else if(s[i]=="STRUCTE"){
            flag=0;
            typesize.insert(make_pair("sname",soff));
            sname="";
            soff=0;
        }
        else if(s[i]=="JMP"){
            printf("\tJMP\t");
            cout << label[a[i][2]] << endl;
        }
    }
}

int make_8086(){
    makeasm();
    printf("\tMOV\tAH,4CH\n\tINT\t21H\nCSEG\tENDS\nEND MAIN\n");
#ifdef __linux__
    freopen("/dev/tty", "r", stdin);
    freopen("/dev/tty", "w", stdout);
#endif
#ifdef __WIN32
    freopen("CON", "r", stdin);
    freopen("CON", "w", stdout);
#endif
    return 0;
}

