//
// Created by WU on 2018/12/12.
//

#include "optimizer.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"
//
//enum class OP {
//    PLUS=0, MINUS, MULTPLY, DIVIDE, MOD, INC, DEC,
//    BIT_AND=7, BIT_OR, BIT_XOR, SHIFT_LEFT, SHIFT_RIGHT,
//    ASSIGN=12,
//    GET_ADDR=13, GET_VALUE,
//    CALL=15, RET, PUSH, POP,
//    NEG=19, POS, NOT, INV, // NOT: !, INV: ~
//    LESS_THAN=23, GREATER_THEN, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, LOG_AND, LOG_OR,
//    FUNC=31, EFUNC, IF, EI, EL, WH, DO, EW, FOR, JIF, EFOR,
//    NOP=42, JMP, DEF_FUN, DEF_VAR, LABEL, GOTO, CONTINUE, BREAK,
//    INDEX=50 ,DEF_STRU, DEF_STRU_ELE ,EDEF_STRU ,MEMBER,PMEMBER,RETN=56, ASM=57
//};

quat_node new_quat_node(int op,int oprend_1,int oprend_2,int var){
    quat_node a;
    a.op = op;
    a.oprend_1 = oprend_1;
    a.oprend_2 = oprend_2;
    a.var =var;
    return a;
}
void show_op(OP i) {
    switch (i){
        case (OP::ASSIGN):{cout << '=' ;
            break;}
        case (OP::IF):{cout << "if" ;
            break;}
        case (OP::GOTO):{cout << "goto" ;
            break;}
        case (OP::WH):{cout << "while" ;
            break;}
        case (OP::FOR):{cout << "for" ;
            break;}
        case (OP::MULTPLY):{cout << "*" ;
            break;}
        case (OP::PLUS):{cout << "+" ;
            break;}
        case (OP::DIVIDE):{cout << "/" ;
            break;}
        case (OP::MINUS):{cout << "-" ;
            break;}
        case (OP::INDEX):{cout << "[]";
            break;}
        case (OP::LABEL):{cout << "lb";
            break;}
        case (OP::LESS_THAN):{cout << "<";
            break;}
        case (OP::EI):{cout << "ie";
            break;}
        case (OP::DEF_VAR):{cout << "define";
            break;}
        case (OP::RETN):{cout << "RETN";
            break;}
        case (OP::FUNC):{cout << "FUNC";
            break;}
        case (OP::EFUNC):{cout << "EFUNC";
            break;}
        case (OP::ASM):{cout << "ASM:";
            break;}
        case (OP::RET):{cout << "RET";
            break;}
        case (OP::PUSH):{cout << "PUSH";
            break;}
        case (OP::POP):{cout << "POP";
            break;}
        case (OP::CALL):{cout << "CALL";
            break;}
        case (OP::GREATER_THEN):{cout << ">";
            break;}
        case (OP::GREATER_EQUAL):{cout << ">=";
            break;}
        case (OP::LESS_EQUAL):{cout << "<=";
            break;}
        case (OP::BIT_AND):{cout << "&";
            break;}
        case (OP::EL):{cout << "else";
            break;}
        case (OP::JMP):{cout << "jmp";
            break;}
        case (OP::EFOR):{cout << "EFOR";
            break;}
        case (OP::EQUAL):{cout << "==";
            break;}
        case (OP::NOT_EQUAL):{cout << "!=";
            break;}
        case (OP::LOG_OR):{cout << "OR";
            break;}
        case (OP::DEC):{cout << "--";
            break;}
        case (OP::INC):{cout << "++";
            break;}
        case (OP::DO):{cout << "DO";
            break;}
        case (OP::MEMBER):{cout << ".";
            break;}
        case (OP::PMEMBER):{cout << "->";
            break;}
        case (OP::DEF_STRU_ELE):{cout << "Struct_Ele";
            break;}
        case (OP::DEF_STRU):{cout << "Struct";
            break;}
        case (OP::EDEF_STRU):{cout << "End_Struct";
            break;}
    }
}
//bool istempvar(int i){
//    switch (i){
//        case (9): {
//            return true;
//        }
//        case (11): {
//            return true;
//        }
//        case (12): {
//            return true;
//        }
//        case (13): {
//            return true;
//        }
//        case (14): {
//            return true;
//        }
//        case (15): {
//            return true;
//        }
//        default:
//            return false;
//    }
//}

dag_graph::dag_graph() {
    node_num = 0;
}
int dag_graph::find(int i) {
    if(hash_map.find(i)!=hash_map.end()){
        return hash_map[i];
    } else{
        return -1;
    }
}
int dag_graph::find(int op,int oprend_1,int oprend_2){
    int flag=0;
    for(int i=0;i<node_set.size();i++){
        if(node_set[i].op==op){
            for(int j=0;j<node_set[i].adj.size();j++){
                if(node_set[i].adj[j]==hash_map[oprend_1]){
                    flag++;
                    break;
                }
            }
            for(int j=0;j<node_set[i].adj.size();j++){
                if(node_set[i].adj[j]==hash_map[oprend_2]){
                    flag++;
                    break;
                }
            }
            if(flag==2){
                return i;
            }
        }
    }
    return -1;
}
void dag_graph::treat(quat_node a) {
    if(a.op==(int)OP::NOP ){ return;}
    else if(a.op==(int) OP::IF || a.op==(int) OP::DO || a.op==(int) OP::JMP|| a.op==(int) OP::CALL
          || a.op==(int) OP::CONTINUE || a.op==(int) OP::BREAK || a.op==(int) OP::EW ||a.op==(int) OP::GOTO
           || a.op==(int) OP::EFOR){
        dag_node node;
        node.op = a.op;
        node.main=a.var;
        insert_node(node);
        return;
    } else if(a.op==(int) OP::LABEL || a.op==(int) OP::RETN
              || a.op==(int)OP::ASM|| a.op==(int)OP::FUNC || a.op==(int)OP::PUSH|| a.op==(int)OP::POP){
        dag_node node;
        node.op = a.op;
        node.main = a.oprend_1;
        insert_node(node);
        return;
    } else if(a.op==(int) OP::DEF_VAR || a.op==(int) OP::DEF_FUN || a.op==(int) OP::FUNC
              ||a.op==(int) OP::DEF_STRU_ELE ||a.op==(int) OP::DEF_STRU   ){
        dag_node node;
        node.op = a.op;
        node.main = a.oprend_1;
        insert_node(node);
        hash_map[a.oprend_1] = node_num-1;
        return;
    } else if(a.op==(int) OP::EI || a.op==(int) OP::FOR||
              a.op==(int) OP::WH  || a.op==(int) OP::EFUNC ||a.op==(int) OP::EDEF_STRU) {
        dag_node node;
        node.op = a.op;
        node.main = 0;
        insert_node(node);
        return;
    }
    if(find(a.var)>=0){//当前赋值变量是否存在节点
        vector<int>::iterator be=node_set[find(a.var)].additional.begin();
        for(;be!=node_set[find(a.var)].additional.end();be++){//如果不是主标记则删除
            if(*be==a.var){
                node_set[find(a.var)].additional.erase(be);
                break;
            }
        }
    }
    dag_node node;
    node.op=a.op;
    node.main=a.var;
    if(a.op!=(int)OP::ASSIGN){
        if(find(a.oprend_1)>=0){
            node.adj.push_back(find(a.oprend_1));
            if(find(a.oprend_2)>=0){
                int n=find(a.op,a.oprend_1,a.oprend_2);
                if(n>=0){
                    //如果主标记是临时变量a.var不是临时变量这交换次序
                    node_set[n].additional.emplace_back(a.var);//添加到附加标记
                    if(istempvar(node_set[n].main) && isdefinevar(a.var)){
                        node_set[n].additional.erase(node_set[n].additional.begin()+
                                                     node_set[n].additional.size()-1);
                        node_set[n].additional.emplace_back(node_set[n].main);
                        node_set[n].main = a.var;
                    }
                    hash_map[a.var]=n;
                } else{
                    node.adj.push_back(find(a.oprend_2));
                    insert_node(node);
                    hash_map[a.var] = node_num-1;
                    node_set[hash_map[a.oprend_1]].pre.emplace_back(node_num-1);
                    node_set[hash_map[a.oprend_2]].pre.emplace_back(node_num-1);
                }
            } else if(a.oprend_2!=0){
                dag_node node1;
                node1.op=(int)OP::ASSIGN;
                node1.main=a.oprend_2;
                insert_node(node1);
                hash_map[a.oprend_2] = node_num-1;
                node.adj.push_back(find(a.oprend_2));
                insert_node(node);
                hash_map[a.var] = node_num-1;
                node_set[hash_map[a.oprend_1]].pre.emplace_back(node_num-1);
                node_set[hash_map[a.oprend_2]].pre.emplace_back(node_num-1);
            } else{
                if(a.var!=0) {
                    node.adj.push_back(find(a.oprend_1));
                    insert_node(node);
                    hash_map[a.var] = node_num-1;
                    node_set[hash_map[a.oprend_1]].pre.emplace_back(node_num-1);
                }
            }
        } else {
            dag_node node1;
            node1.op=(int)OP::ASSIGN;
            node1.main=a.oprend_1;
            insert_node(node1);
            hash_map[a.oprend_1] = node_num-1;
            node.adj.push_back(hash_map[a.oprend_1]);
            if(find(a.oprend_2)>=0){
                node.adj.push_back(hash_map[a.oprend_2]);
                insert_node(node);
                hash_map[a.var] = node_num-1;
            } else if(a.oprend_2!=0){
                dag_node node1;
                node1.op=(int)OP::ASSIGN;;
                node1.main=a.oprend_2;
                insert_node(node1);
                hash_map[a.oprend_2] = node_num-1;
                node.adj.push_back(hash_map[a.oprend_2]);
                insert_node(node);
                hash_map[a.var] = node_num-1;
            } else{
                insert_node(node);
                hash_map[a.var] = node_num-1;
                node.adj.emplace_back(hash_map[a.var]);
            }
            node_set[hash_map[a.oprend_1]].pre.emplace_back(node_num-1);
            node_set[hash_map[a.oprend_2]].pre.emplace_back(node_num-1);
        }
    } else{
        if(find(a.oprend_1)>=0){
            //如果主标记是临时变量a.var不是临时变量这交换次序

            node_set[find(a.oprend_1)].additional.emplace_back(a.var);//添加至附加标记
            if(istempvar(node_set[find(a.oprend_1)].main) && isdefinevar(a.var)){
                node_set[find(a.oprend_1)].additional.erase(node_set[find(a.oprend_1)].additional.begin()+
                                                            node_set[find(a.oprend_1)].additional.size()-1);
                node_set[find(a.oprend_1)].additional.emplace_back(node_set[find(a.oprend_1)].main);
                node_set[find(a.oprend_1)].main = a.var;
            }
            hash_map[a.var] = find(a.oprend_1);
        } else{
            dag_node node1;
            node1.op=(int)OP::ASSIGN;;
            node1.main=a.oprend_1;
            insert_node(node1);
            hash_map[a.oprend_1] = node_num-1;
            //如果主标记是临时变量a.var不是临时变量这交换次序
            node_set[find(a.oprend_1)].additional.emplace_back(a.var);//添加至附加标记
            hash_map[a.var] = find(a.oprend_1);
        }
    }
}//需要增加一个临时变量和用户自定义变量换位的过程
void dag_graph::insert_node(dag_node a) {
    node_set.push_back(a);
    node_num++;
}
void dag_graph::showgraph() {
    for (int i = 0; i < node_num; i++) {
        cout << "node(" << i + 1 << ")   ";
        cout << "op(";
        show_op((OP) node_set[i].op);
        cout << ")   ";
        cout << "main=";
        cout << node_set[i].main;
        if (node_set[i].additional.size() > 0) {
            cout << "|addition=";
        }
        for (int j = 0; j < node_set[i].additional.size(); j++) {
            cout << node_set[i].additional[j] << ' ';
        }
        cout << endl << endl;
    }
}

block_graph::block_graph(){
    init_quat_list();//初始化四元式活动信息
    get_block();//对于四元式进行分块
    get_adj();//对于块构造图
}//构造函数
void block_graph::get_adj(){
    vector<block_node>::iterator begin=node_set.begin(),end=node_set.end();
    vector<block_node>::iterator now=node_set.begin(),pre=now;
    int n=0;
    now++;
    n++;
    while (now!=end) {//遍历已经分块的图节点
        if(quat_list[pre->end].op!=(int)OP::GOTO){//判断当前运算符是否是goto
            pre->adjlist.push_back(n);//如果不是goto程序的顺序进行原则依然保持
            now->prelist.push_back(n-1);
        }
        if(quat_list[pre->end].op==(int) OP::ASM){

        }else if(is_jmp(quat_list[pre->end].op)){//判断是否为跳转语句
            vector<block_node>::iterator a = node_set.begin();
            int p = quat_list[pre->end].var;
            int i=0;
//            cout << p << endl;
//            cout << "p:" <<quat_list[p].op <<','
//                 << quat_list[p].oprend_1 << ','
//                 << quat_list[p].oprend_2 <<','
//                 << quat_list[p].var << endl;
            while (a->begin!=p && a!=node_set.end()){
                a++;
                i++;
            }
            pre->adjlist.push_back(i);//针对于跳转语句要将当前结点和跳转到的节点进行连接
            node_set[i].prelist.push_back(n-1);
        }//构造图的邻接表
        pre = now;
        now++;
        n++;
    }
    if(is_jmp(pre->end)){//针对于最后一个节点进行额外的一次判断
        if(quat_list[pre->end].op!=2){
            pre->adjlist.push_back(n);
            now->prelist.push_back(n-1);
        }
        vector<block_node>::iterator a = node_set.begin();
        int i=0;
        while (a->begin!=pre->end){
            a++;
            i++;
        }
        pre->adjlist.push_back(i);
        node_set[i].prelist.push_back(n-1);
    }
}//构造图的邻接表
void block_graph::get_block() {//分块函数根据入口进行分块
    vector<quat_node>::iterator entry=quat_list.begin(),exit=quat_list.end();
    vector<quat_node>::iterator now = quat_list.begin(),pre;
    get_entry();
    int i=0;
    block_node new_node;
    node_num=0;
    int flag=0;
    while (now!=exit){//遍历整个四元式节点
        if(flag==0 && now->is_entry){//根据入口进行分块
            new_node.quat_list.clear();
            new_node.prelist.clear();
            new_node.adjlist.clear();
            new_node.begin = i;
            flag = 1;
        }else if(flag==1 && now->is_entry){
            new_node.end = --i;
            new_node.quat_list.insert(new_node.quat_list.begin(),
                                      quat_list.begin()+new_node.begin,quat_list.begin()+new_node.end+1);
            --now;
            node_set.push_back(new_node);
            node_num++;
            flag = 0;
        }
        pre = now;
        now++;
        i++;
    }
    if(flag==1){//针对于结束节点特殊处理
        new_node.end = i-1;
        new_node.quat_list.insert(new_node.quat_list.begin(),
                                  quat_list.begin()+new_node.begin,quat_list.begin()+new_node.end);
        node_set.push_back(new_node);
        node_num++;
        flag = 0;
    }
}//根据入口进行分块
void block_graph::get_entry() {//根据四元式信息分出入口
    vector<quat_node>::iterator entry=quat_list.begin(),exit=quat_list.end();
    vector<quat_node>::iterator now = quat_list.begin();
    set_jmp_line();
    for(;now!=exit;now++){
        now->is_entry = false;
    }
    now = entry;
    entry->is_entry = true;
    while (now != exit){
        while(is_jmp(now->op)){//判断当前语句是否是跳转语句
            quat_list[now->var].is_entry = true;
            (++now)->is_entry = true;
        }
        now++;
    }
    now = entry;
}//根据四元式序列来判断是否是块入口
bool block_graph::is_jmp(int i) {
    if(jmp_line.find(i)!=jmp_line.end()){//查找跳转语句集合
        return true;
    } else{
        return false;
    }
}//判断是否是跳转语句
void block_graph::init_quat_list() {
    ifstream a("siyuan.txt");
    while (!a.eof()){
        int op;
        int op_1,op_2;
        int var;
        a >> op >> op_1 >> op_2 >> var ;
        quat_list.emplace_back(new_quat_node(op,op_1,op_2,var));
    }
    a.close();
//    for(int i=0;i<a.size();i++){
//        quat_list.push_back(a[i]);
//    }
}//初始化四元式序列
void block_graph::show_block(){
    for(int i=0;i<node_set.size();i++){
        cout << "block:" << i+1 << endl;
        for(int j=0;j<node_set[i].quat_list.size();j++){
            cout << "quat_number:" << j+1;
            cout << "  (" ;
            show_op(OP(node_set[i].quat_list[j].op)) ;
            cout << ',' ;
            cout << node_set[i].quat_list[j].oprend_1;
            cout << ',';
            cout << node_set[i].quat_list[j].oprend_2;
            cout << ',';
            cout << node_set[i].quat_list[j].var;
            cout << ')';
            cout << endl;
        }
    }
}
void block_graph::set_jmp_line() {
    jmp_line.insert((int)OP::IF);//if
    jmp_line.insert((int)OP::GOTO);//goto
    jmp_line.insert((int)OP::JMP);//JMP
    jmp_line.insert((int)OP::BREAK);//BREAK
    jmp_line.insert((int)OP::DO);//DO
    jmp_line.insert((int)OP::CONTINUE);//CONTINUE
    jmp_line.insert((int)OP::ASM);//ASM
//    jmp_line.insert(5);
//    jmp_line.insert(6);
}//使用一个集合来标记跳转指令


optimizer::optimizer():block_graph(){
    dag();
    show_block();
    new_quat_list.clear();
    for(int i=0;i<node_set.size();i++){
        new_quat_list.insert(new_quat_list.begin()
                ,node_set[i].quat_list.begin(),node_set[i].quat_list.end());
    }
    data_flow_analyse();
    delete_useless_var();
    gennerate_new_quat();
}
void optimizer::dag() {
    vector<block_node>::iterator first=node_set.begin(),last=node_set.end();
    new_quat_list.assign(quat_list.begin(),quat_list.end());
    while (first!=last){
        int now=0;
        dag_graph a;
        while (now<first->quat_list.size()){
            a.treat(first->quat_list[now]);
            now++;
        }
        generate_quat(a,first);
        first++;
    }
//    show_block();
}//dag图优化
void optimizer::loop_optimize() {

}//循环优化
void optimizer::generate_quat(const dag_graph &a,vector<block_node>::iterator now) {
    int begin = now->begin;
    now->quat_list.clear();
    for(int i=0;i<a.node_num;i++){
        if(a.node_set[i].op==(int)OP::ASSIGN){
            for(int j=0;j<a.node_set[i].additional.size();j++){
                if(isdefinevar(a.node_set[i].additional[j])){
                    new_quat_list[begin].op = (int)OP::ASSIGN;
                    new_quat_list[begin].var = a.node_set[i].additional[j];
                    new_quat_list[begin].oprend_1 = a.node_set[i].main;
                    new_quat_list[begin].oprend_2 = 0;
                    new_quat_list[begin].is_entry = false;
                    begin++;
                }
            }
        } else if(a.node_set[i].op==(int)OP::EI ||a.node_set[i].op==(int) OP::EDEF_STRU
                   || a.node_set[i].op==(int)OP::EFUNC) {
            new_quat_list[begin].op = a.node_set[i].op;
            new_quat_list[begin].var = 0;
            new_quat_list[begin].oprend_1 = 0;
            new_quat_list[begin].oprend_2 = 0;
            begin++;
        }else if(a.node_set[i].op==(int) OP::LABEL|| a.node_set[i].op==(int) OP::RETN
                 || a.node_set[i].op==(int)OP::DEF_VAR ||
                 a.node_set[i].op==(int)OP::DEF_FUN|| a.node_set[i].op==(int)OP::ASM||
                 a.node_set[i].op==(int)OP::FUNC||a.node_set[i].op==(int)OP::PUSH
                 ||a.node_set[i].op==(int)OP::POP
                 ||a.node_set[i].op==(int) OP::DEF_STRU_ELE ||a.node_set[i].op==(int) OP::DEF_STRU   ) {
            new_quat_list[begin].op = a.node_set[i].op;
            new_quat_list[begin].var = 0;
            new_quat_list[begin].oprend_1 = a.node_set[i].main;
            new_quat_list[begin].oprend_2 = 0;
            begin++;
        }else if(a.node_set[i].op==(int) OP::GOTO|| a.node_set[i].op==(int) OP::JMP
                 || a.node_set[i].op==(int)OP::IF|| a.node_set[i].op==(int)OP::EW || a.node_set[i].op==(int)OP::BREAK
                 || a.node_set[i].op==(int)OP::CONTINUE|| a.node_set[i].op==(int)OP::EFOR){
            new_quat_list[begin].op = a.node_set[i].op;
            new_quat_list[begin].var = a.node_set[i].main;
            new_quat_list[begin].oprend_1 = 0;
            new_quat_list[begin].oprend_2 = 0;
            begin++;
        }else{
            new_quat_list[begin].op = a.node_set[i].op;
            new_quat_list[begin].var = a.node_set[i].main;
            if(a.node_set[i].adj.size()==1){
                new_quat_list[begin].oprend_1 = a.node_set[a.node_set[i].adj[0]].main;
            } else if(a.node_set[i].adj.size()==2){
                new_quat_list[begin].oprend_2 = a.node_set[a.node_set[i].adj[1]].main;
                new_quat_list[begin].oprend_1 = a.node_set[a.node_set[i].adj[0]].main;
            }
            begin++;
            for(int j=0;j<a.node_set[i].additional.size();j++){
                if(isdefinevar(a.node_set[i].additional[j])){
                    new_quat_list[begin].op = (int)OP::ASSIGN;
                    new_quat_list[begin].var = a.node_set[i].additional[j];
                    new_quat_list[begin].oprend_1 = a.node_set[i].main;
                    new_quat_list[begin].oprend_2 = 0;
                    new_quat_list[begin].is_entry = false;
                    begin++;
                }
            }
            new_quat_list[begin].is_entry = false;
        }
    }
    now->end = begin-1;
    now->quat_list.insert(now->quat_list.begin(),
                          new_quat_list.begin()+now->begin,new_quat_list.begin()+now->end+1);
}
void optimizer::delete_useless_var() {
    for(int i=0;i<node_num;i++) {
        set<int>::iterator start = Def[i].begin(), end = Def[i].end();
        while (start != end) {
            int j;
            int m;
            unsigned int flag=0;
            for (j = node_set[i].quat_list.size()-1; j >=0 ; j--) {
                if(new_quat_list[j].var==*start){
                    break;
                }
            }
            for(m=j;m>=0;m--){
                if(new_quat_list[j].oprend_1==*start || new_quat_list[j].oprend_2==*start){
                    break;
                }
            }
            for (int k = i; k <node_num ; ++k) {
                if (LiveUse[k].find(*start)!=LiveUse[k].end()){
                    flag=1;
                }
            }
            if(flag==0){
                node_set[i].quat_list.erase(node_set[i].quat_list.begin()+m,node_set[i].quat_list.begin()+j);
            }
            start++;
        }
    }
}
void optimizer::show_block(){
    for(int i=0;i<node_set.size();i++){
        cout << "block:" << i+1 << endl;
        for(int j=0;j<node_set[i].quat_list.size();j++){
            cout << "quat_number:" << j+1;
            cout << "begin:" << node_set[i].begin << ' ';
            cout << "end:" << node_set[i].end << ' ';
            cout << "  (" ;
            show_op(OP(node_set[i].quat_list[j].op)) ;
            cout << ',' ;
            cout << node_set[i].quat_list[j].oprend_1;
            cout << ',';
            cout << node_set[i].quat_list[j].oprend_2;
            cout << ',';
            cout << node_set[i].quat_list[j].var;
            cout << ')';
            cout << endl;
        }
    }
}

void optimizer::data_flow_analyse() {
    //活跃信息流
    for(int i=0;i<node_num;i++){
        set<int> a;
        LiveUse.push_back(a);
        Def.push_back(a);
        LiveOut.push_back(a);
        LiveIn.push_back(a);
    }
    gennerate_LiveUse();
    gennerate_Def();
    gennerate_LiveIn();
}

//向后活跃信息流
void optimizer::gennerate_Def(){
    for(int i=0;i<node_set.size();i++){
        for(int j=0;j<node_set[i].quat_list.size();j++){
            if(node_set[i].quat_list[j].op==0){
                Def[i].insert(node_set[i].quat_list[j].var);
            }
        }
    }
}//构造Def集合
void optimizer::gennerate_LiveUse() {
    for(int i=0;i<node_set.size();i++){
        for(int j=0;j<node_set[i].quat_list.size();j++){
            if(node_set[i].quat_list[j].op!=(int)OP::LABEL &&
               node_set[i].quat_list[j].op!=(int)OP::EI &&
               node_set[i].quat_list[j].op!=(int)OP::WH){
                LiveUse[i].insert(node_set[i].quat_list[j].oprend_1);
                if(node_set[i].quat_list[j].oprend_2!=(int)OP::NOP){
                    LiveUse[i].insert(node_set[i].quat_list[j].oprend_2);
                }
            }
        }
    }
}//构造LiveUse集合
void optimizer::gennerate_LiveOut(int i) {
    if(i<node_num-1){
        for(int j=0;j<node_set[i].adjlist.size();j++){
            set_union(LiveOut[i].begin(),LiveOut[i].end(),
                      LiveIn[node_set[i].adjlist[j]].begin(),
                      LiveIn[node_set[i].adjlist[j]].end(),
                      inserter(LiveOut[i],LiveOut[i].begin()));
        }
    } else{
        LiveOut[i].clear();
    }
}//构造LiveOut集合
void optimizer::gennerate_LiveIn() {
    for(int i=node_set.size()-1;i>=0;i--){
        set_union(LiveIn[i].begin(),LiveIn[i].end(),
                  LiveUse[i].begin(),LiveUse[i].end(),inserter(LiveIn[i],LiveIn[i].begin()));
        gennerate_LiveOut(i);
        set_union(LiveIn[i].begin(),LiveIn[i].end(),
                  LiveOut[i].begin(),LiveOut[i].end(),inserter(LiveIn[i],LiveIn[i].begin()));
        set_difference(LiveIn[i].begin(),LiveIn[i].end(),
                       Def[i].begin(),Def[i].end(),inserter(LiveIn[i],LiveIn[i].begin()));
    }
}//构造LiveIn集合

//生成新的四元式
vector<quat_node>& optimizer::gennerate_new_quat() {
    vector<quat_node> new_list;
    map<int,vector<int>> old_new;
    map<int,int> old_new_in;
    int line=0;
    for(int i=0;i<node_set.size();i++){
        for(int j=0;j<node_set[i].quat_list.size();j++){
            if(j==0){
                old_new_in[node_set[i].begin] = line;
                if(old_new.find(node_set[i].begin)!=old_new.end()){
                    for(int m=0;m<old_new[node_set[i].begin].size();m++){
                        new_list[old_new[node_set[i].begin][m]].var = line;
                    }
                }
            } else if(j==node_set[i].quat_list.size()-1){
                if(old_new_in.find(node_set[i].quat_list[j].var)!=old_new_in.end()){
                    if(node_set[i].quat_list[j].op != (int)OP::GOTO)
                        node_set[i].quat_list[j].var = old_new_in[node_set[i].quat_list[j].var];
                } else{
                    old_new[node_set[i].quat_list[j].var].emplace_back(line);
                }
            }
            new_list.emplace_back(node_set[i].quat_list[j]);
            line++;
        }
    }
    ofstream a("youhua.txt");
    QL.clear();
    for(int i=0;i<new_list.size();i++){
        a << new_list[i].op << ' ' << new_list[i].oprend_1 << ' ' << new_list[i].oprend_2 << ' ' << new_list[i].var << endl;
        QL.emplace_back((OP)new_list[i].op, new_list[i].oprend_1, new_list[i].oprend_2, new_list[i].var);
    }

    cout << QL;
    return new_list;
}