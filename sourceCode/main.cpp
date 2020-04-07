#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unordered_map>
#include <cstring>
#include <algorithm>
#include <stack>
#include <stdlib.h>
using namespace std;
//const int MAX_EDGE = 100000000;
struct Edge{
    int anotherID;
    int money;
    Edge(){};
    Edge(int a,int b){
        anotherID = a;
        money = b;
    }
};

struct Vertex{
    int ID;
    bool wasVisited;
    list<Edge> edgeList;
    Vertex():wasVisited(false){}
};

class Discirminant{
public:
    Discirminant():nodeNum(0),dfsDeep(0){};
    void readTestData(string path);
    void writeResult(string);
    void run();
    void dfs(int nodeID);

private:
    list<vector<int>> res;
    unordered_map<int,Vertex> graph;
    //入度哈希表，记录每个顶点的入度数量
    unordered_map<int,int> inEdgeMap;
    int nodeNum;
    int dfsDeep;
    const int maxDeep = 10;

private:
    void sortResult(list<vector<int>> &res);
    void pushResult(int *data, int size);
    //拓扑排序
    void topSort();
    //检查数据中是否套有其他圈
    bool checkDoubleCircle(int circle [], int size, int vertexID);

};

void Discirminant::readTestData(string path) {
    clock_t startTime,endTime;
    startTime = clock();

    FILE* stream = fopen("../data/test_data.txt", "rb");
    if(!stream){
        cout<<"File Open failed!"<<endl;
        exit(0);
    }

    const int MAX=60*1024*1024;
    char* buf=(char *)malloc(sizeof(char)*MAX);
    memset(buf, 0, MAX);

    int len = fread(buf, sizeof(char), MAX, stream);
    *(buf+len)='\0';
    fclose(stream);

    // 输出读取的结果到屏幕
    //printf("%s\n", buf);

    int res=0;
    int cnt=1;
    int UID,CID,money;
    for(char*p=buf;(*p)!='\0';++p) {
        //遇到数字
        if(*p>='0' && *p<='9') {
            res = res * 10 + *p - '0';
        //遇到第一个逗号
        }else if (*p ==',' && cnt==1) {
            UID = res;
            res=0;
            cnt++;
        //遇到第二个逗号
        }else if(*p ==',' && cnt==2){
            CID = res;
            res=0;
            cnt++;
        //遇到换行符
        }else if(*p=='\n'){
            money=res;
            res=0;
            cnt=1;
            //printf("UID:%d CID:%d money:%d\n",UID,CID,money);
            //入度表中有UID的key值，就不用再初始化，CID的入度+1
            if(inEdgeMap.find(UID)!=inEdgeMap.end()){
                inEdgeMap[CID]++;
            }else {//入度表中没有UID的key值，插入并初始化为0
                inEdgeMap[UID] = 0;
                inEdgeMap[CID]++;
            }

            //插入到表中
            unordered_map<int,Vertex>::iterator iter = graph.find(UID);
            if(iter != graph.end()){
                //已经存在key
                iter->second.ID = UID;
                Edge edge(CID,money);
                iter->second.edgeList.push_back(edge);
            } else{
                //不存在key
                nodeNum++;
                Vertex vertex;
                vertex.ID = UID;
                vertex.edgeList.push_back({CID,money});
                graph[UID] = vertex;
            }
        }
    }
    //拓扑排序，排除掉不可能成环的顶点
    topSort();
    endTime = clock();
    double totalTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    cout<<"ifstream running time:"<<totalTime<<"s"<<endl;
}

void Discirminant::topSort(){
    //构建临时图,保持temGraph完整性，删减graph中的顶点，
    unordered_map<int,Vertex> temGraph(graph);

    int cnt=0;
    stack<int> s;

    //遍历入度表找到所有入度为0的顶点下标，并入栈
    for(auto it:inEdgeMap){
        if(it.second==0){
            cnt++;
            graph.erase(it.first);
            s.push(it.first);
        }
    }
    cout<<"顶点总数："<<nodeNum<<endl;
    cout<<"修剪前入度为0的顶点数："<<cnt<<endl;

    //遍历栈顶顶点的所有边
    while(!s.empty()){
        int ID=s.top();
        s.pop();
        Vertex node =temGraph[ID];

        //当前顶点所有出边的顶点的入度都要-1
        for(auto it:node.edgeList){
            inEdgeMap[it.anotherID]--;
            //修剪后的邻接点如果也出现了入度为0，也要入栈
            if(inEdgeMap[it.anotherID]==0){
                cnt++;
                graph.erase(it.anotherID);
                s.push(it.anotherID);
            }
        }

    }
    cout<<"拓扑排序后不可能形成环的顶点个数："<<cnt<<endl;
    cout<<"拓扑排序后需要遍历的顶点个数："<<graph.size()<<endl;
}

void Discirminant::writeResult(string path) {
    //res 为乱序，但是都符合规则要求
    if(res.empty()){
        cout<<"Write Error:  No Circle!"<<endl;
        exit(0);
    }
    ofstream outStream(path);
    if(!outStream.is_open()){
        cout<<"Wirte Error:  Open file failed!"<<endl;
        exit(0);
    }
    outStream<<res.size()<<"\n";
    //sort
    sortResult(res);
    //write result
    for(auto &line:res){
        for(int i = 0; i < line.size()-1; i++){
            outStream << line[i]<<",";
        }
        outStream<<line.back()<<"\n";
    }
}
//有圈中圈返回true，否则false
bool Discirminant::checkDoubleCircle(int circle [],int size, int vertexID) {
    if(size < 3) return false;
    for(int i = 1; i < size;i++){
        if(circle[i] == vertexID) return true;
    }
    return false;
}


void Discirminant::dfs(int nodeID) {
    //限制深度遍历的深度
    if(dfsDeep >= maxDeep){
        return;
    }

    //当前结点
    Vertex & node = graph[nodeID];
    node.wasVisited = true;

    //遍历当前结点的边表
    for(list<Edge>::iterator iter = node.edgeList.begin();iter !=node.edgeList.end();){

        //建立环储存数据
        int *circle = new int[7];
        //memset(circle,-1, sizeof(circle));
        memset(circle,-1, sizeof(int)*7);

        //取出第1个边
        auto firstEdge = iter;

        //环头部为当前结点
        circle[0] = nodeID;

        //取出第1条边的结点：第1个邻接点
        Vertex& secondNode = graph[firstEdge->anotherID];

        //判断是否被访问过
        //if(secondNode.wasVisited) continue;
        circle[1] = secondNode.ID;

        //遍历第1个邻接点的边表
        for(auto &secondEdge:secondNode.edgeList){
            Vertex &thirdNode = graph[secondEdge.anotherID];
            circle[2] = thirdNode.ID;

            //排除小于3的点
            if(thirdNode.ID != nodeID){
                //第三列边
                for(auto &thirdEdge:thirdNode.edgeList){
                    Vertex &fourthNode = graph[thirdEdge.anotherID];

                    //出现有3个结点构成的环
                    if(fourthNode.ID == nodeID){
                        pushResult(circle, 3);
                    }else{
                        //检查是否会有圈中圈
                        if( checkDoubleCircle(circle,3,fourthNode.ID)) continue;
                        circle[3] = fourthNode.ID;

                        //遍历第4个结点的边表
                        for(auto &fourthEdge:fourthNode.edgeList){
                            Vertex &fifthNode = graph[fourthEdge.anotherID];
                            if(fifthNode.ID == nodeID){
                                pushResult(circle, 4);
                            } else{
                                //检查是否会有圈中圈
                                if(checkDoubleCircle(circle,4,fifthNode.ID)) continue;
                                circle[4] = fifthNode.ID;
                                //遍历第5个结点的边表
                                for(auto &fifthEdge:fifthNode.edgeList){
                                    Vertex &sixthNode = graph[fifthEdge.anotherID];
                                    if(sixthNode.ID == nodeID){
                                        pushResult(circle, 5);
                                    } else{
                                        //检查是否会有圈中圈
                                        if(checkDoubleCircle(circle,5,sixthNode.ID)) continue;
                                        circle[5] = sixthNode.ID;
                                        //遍历第6个结点的边表
                                        for (auto &sixEdge:sixthNode.edgeList) {
                                            Vertex &seventhNode = graph[sixEdge.anotherID];
                                            if(seventhNode.ID == nodeID){
                                                pushResult(circle, 6);
                                            } else{
                                                //检查是否会有圈中圈
                                                if(checkDoubleCircle(circle,6,seventhNode.ID)) continue;
                                                circle[6] = seventhNode.ID;
                                                //遍历第7个结点的边表
                                                for (auto &seventhEdge : seventhNode.edgeList) {
                                                    Vertex &eighthNode = graph[seventhEdge.anotherID];
                                                    if(eighthNode.ID == nodeID){
                                                        pushResult(circle, 7);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        delete [] circle;
        //剪枝
        iter = node.edgeList.erase(iter);

        //递归
        dfsDeep++;
        if(iter != node.edgeList.end() && !graph[iter->anotherID].wasVisited)
            dfs(secondNode.ID);
        dfsDeep--;
    }
}

int countRes = 0;
void Discirminant::pushResult(int data[], int size) {
    countRes++;
    //cout<<countRes<<endl;
    //取出最小ID做头部
    int head = 0;
    int minID = INT_MAX;
    for(int i = 0; i <size && data[i] != -1;i++){
        if(minID > data[i]){
            head = i;
            minID = data[i];
        }
    }
    //放入vec中
    vector<int> cir;
    for(int i = head;i < size && data[i] != -1;i++){
        cir.push_back(data[i]);
    }
    for(int i = 0; i < head;i++){
        cir.push_back(data[i]);
    }
    res.push_back(cir);
}

bool compare(vector<int> a, vector<int> b){
    if(a.size() < b.size()){
        return true;
    } else if(a.size() == b.size()){
        for(int i = 0;i < a.size();i++){
            if(a[i] < b[i]) return true;
            else if (a[i] > b[i]) return false;
        }
        return true;
    } else{
        return false;
    }
}

void Discirminant::sortResult(list<vector<int>> &res) {
    //sort(res.begin(),res.end(),compare);
    res.sort(compare);
}

void Discirminant::run() {
    for(auto &node: graph){
        //未被访问过，且入度不为0
        if(!node.second.wasVisited){
            dfs(node.second.ID);
        }
    }
}

int main() {
    //system("chcp 65001"); //解决win下打印出乱码的问题
    string testFile = "../data/myText.txt";
    string resultFile = "../projects/student/result.txt";

    clock_t startTime,endTime,endTime1;
    startTime = clock();

    Discirminant discirminant;
    discirminant.readTestData(testFile);

    //测试算法
    //discirminant.dfs(94);
    //discirminant.dfs(2112);
    //discirminant.dfs(1131);

    discirminant.run();
    endTime1 = clock();
    cout<<"running time: "<<(double(endTime1 - startTime)/CLOCKS_PER_SEC)<<"s"<<endl;
    discirminant.writeResult(resultFile);

    endTime = clock();
    cout<<"Total running time: "<<(double(endTime - startTime)/CLOCKS_PER_SEC)<<"s"<<endl;
    return 0;
}