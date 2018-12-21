//
// Created by WU on 2018/12/12.
//

#ifndef COMPILER_OPTIMIZER_H
#define COMPILER_OPTIMIZER_H

#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include<algorithm>

using namespace std;

struct quat_node{//四元式
    int op;//运算符
    int oprend_1,oprend_2;//操作数
    int var;//变量
    bool is_entry;//判断是否是入口地址
};
struct block_node{//块节点
    int begin,end;//指向块开始四元式和结束四元式
    vector<quat_node> quat_list;
    vector<int> adjlist;//邻接表
    vector<int> prelist;//前驱表
};
struct dag_node{
    int op;//运算符
    int main;//主标记
    vector<int> additional;//附属标记
    vector<int> adj;//邻接表
    vector<int> pre;//前驱表
};
class dag_graph{
public:
    vector<dag_node> node_set;//dag图节点集合
    map<int,int> hash_map;//哈希表根据输入的变量找到相应节点
    int node_num;//节点数量
    dag_graph();
    void insert_node(dag_node a);//插入节点
    void treat(quat_node a);//根据四元式边建立边生成dag图
    void showgraph();//输出图的信息
    int find(int i);//根据变量查找在表中的位置
    int find(int op,int oprend_1,int oprend_2);//根据变量查找在表中的位置
};
class block_graph{
protected:
    vector<block_node> node_set;//图节点集合
    int node_num;//节点数量
    set<int> jmp_line;//跳转语句集合
    vector<quat_node> quat_list;//四元式列表
public:
    block_graph();//构造函数
    void set_jmp_line();//设置跳转语句标号
    void init_quat_list();//初始化四元式列表
    void get_entry();//根据跳转语句对程序块进行入口信息赋值
    void get_block();//对四元式进行分块
    void get_adj();//构造块之间的邻接关系
    void show_block();
    bool is_jmp(int i);//判断是否是跳转语句
};
class optimizer :public block_graph{
private:
    vector<quat_node> new_quat_list;
    vector<set<int>> LiveIn,LiveOut,Def,LiveUse;
    vector<vector<int>> loop_set;
public:
    void show_block();
    optimizer();
    void dag();//构造DAG图对块内进行局部优化
    void identifyloop();//识别自然循环
    void loop_optimize();//循环优化定值代码外提
    void data_flow_analyse();//数据流分析
    void generate_quat(const dag_graph &a,vector<block_node>::iterator now);//根据DAG图的结构构造四元式

    //向后数据流分析
    void gennerate_LiveIn();//构造LiveIn集合
    void gennerate_LiveOut(int i);//构造LiveOut集合
    void gennerate_Def();//构造Def集合
    void gennerate_LiveUse();//构造LiveUse集合

    //删除无用变量
    void delete_useless_var();
    //生成新的四元式
    vector<quat_node>& gennerate_new_quat();
};


#endif //COMPILER_OPTIMIZER_H
