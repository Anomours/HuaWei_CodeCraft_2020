#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unordered_map>
#include <cstring>
#include <algorithm>
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
    Discirminant():nodeNum(0){};
    void readTestData(string path);
    void writeResult(string);
    void run();
    void dfs(int nodeID);

private:
    vector<vector<int>> res;
    unordered_map<int,Vertex> graph;
    int nodeNum;
private:
    void sortResult(vector<vector<int>> &res);
    void pushResult(int *data, int size);
    //检查数据中是否套有其他圈
    bool checkDoubleCircle(int circle [], int size, int vertexID);

};
void Discirminant::readTestData(string path) {
    clock_t startTime,endTime;
    startTime = clock();

    ifstream inStream(path.c_str());
    string line;

    if(!inStream){
        cout<<"File Open failed!"<<endl;
        exit(0);
    }
    int e=0;
    while(inStream){
        getline(inStream,line);
        e++;
        if(line.empty()) continue;

        stringstream sin(line);
        string s;

        int UID,CID,money;
        char ch;
        sin >> UID;
        sin >> ch;
        sin >> CID;
        sin >> ch;
        sin >>money;
        sin.clear();
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
    inStream.close();
    endTime = clock();
    double totalTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    cout<<"ifstream running time:"<<totalTime<<"s"<<endl;
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
    Vertex & node = graph[nodeID];
    node.wasVisited = true;
    //第一列边
    for(list<Edge>::iterator iter = node.edgeList.begin();iter !=node.edgeList.end();){
        auto firstEdge = iter;
        //存储环的数据
        int *circle = new int[7];
        memset(circle,-1, sizeof(circle));
        circle[0] = nodeID;
        //取出第一条边的另一个点
        Vertex& secondNode = graph[firstEdge->anotherID];
        //判断是否被访问过
        //if(secondNode.wasVisited) continue;
        circle[1] = secondNode.ID;
        //第二列边
        for(auto &secondEdge:secondNode.edgeList){
            Vertex &thirdNode = graph[secondEdge.anotherID];
            circle[2] = thirdNode.ID;
            //排除小于3的点
            if(thirdNode.ID != nodeID){
                //第三列边
                for(auto &thirdEdge:thirdNode.edgeList){
                    Vertex &fourthNode = graph[thirdEdge.anotherID];
                    if(fourthNode.ID == nodeID){
                        //出现环
                        pushResult(circle, 3);
                    } else{
                        //检查是否会有圈中圈
                        if( checkDoubleCircle(circle,3,fourthNode.ID)) continue;
                        circle[3] = fourthNode.ID;
                        //第四列边
                        for(auto &fourthEdge:fourthNode.edgeList){
                            Vertex &fifthNode = graph[fourthEdge.anotherID];
                            if(fifthNode.ID == nodeID){
                                pushResult(circle, 4);
                            } else{
                                //检查是否会有圈中圈
                                if(checkDoubleCircle(circle,4,fifthNode.ID)) continue;
                                circle[4] = fifthNode.ID;
                                //第五列边
                                for(auto &fifthEdge:fifthNode.edgeList){
                                    Vertex &sixthNode = graph[fifthEdge.anotherID];
                                    if(sixthNode.ID == nodeID){
                                        pushResult(circle, 5);
                                    } else{
                                        //检查是否会有圈中圈
                                        if(checkDoubleCircle(circle,5,sixthNode.ID)) continue;
                                        circle[5] = sixthNode.ID;
                                        //第六条边
                                        for (auto &sixEdge:sixthNode.edgeList) {
                                            Vertex &seventhNode = graph[sixEdge.anotherID];
                                            if(seventhNode.ID == nodeID){
                                                pushResult(circle, 6);
                                            } else{
                                                //检查是否会有圈中圈
                                                if(checkDoubleCircle(circle,6,seventhNode.ID)) continue;
                                                circle[6] = seventhNode.ID;
                                                //第七条边
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
        if(iter != node.edgeList.end())
            dfs(secondNode.ID);
    }
}
void Discirminant::pushResult(int data[], int size) {
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
void Discirminant::sortResult(vector<vector<int>> &res) {
    sort(res.begin(),res.end(),compare);
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
    string testFile = "../data/test_data.txt";
    string resultFile = "../projects/student/result.txt";

    clock_t startTime,endTime;
    startTime = clock();

    Discirminant discirminant;
    discirminant.readTestData(testFile);

    //测试算法
    //discirminant.dfs(94);
    //discirminant.dfs(2112);
    //discirminant.dfs(1131);

    discirminant.run();
    discirminant.writeResult(resultFile);

    endTime = clock();
    cout<<"Total running time: "<<(double(endTime - startTime)/CLOCKS_PER_SEC)<<"s"<<endl;
    return 0;
}