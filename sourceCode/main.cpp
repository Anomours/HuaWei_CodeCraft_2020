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
    //��ȹ�ϣ����¼ÿ��������������
    unordered_map<int,int> inEdgeMap;
    int nodeNum;
    int dfsDeep;
    const int maxDeep = 10;

private:
    void sortResult(list<vector<int>> &res);
    void pushResult(int *data, int size);
    //��������
    void topSort();
    //����������Ƿ���������Ȧ
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

    // �����ȡ�Ľ������Ļ
    //printf("%s\n", buf);

    int res=0;
    int cnt=1;
    int UID,CID,money;
    for(char*p=buf;(*p)!='\0';++p) {
        //��������
        if(*p>='0' && *p<='9') {
            res = res * 10 + *p - '0';
        //������һ������
        }else if (*p ==',' && cnt==1) {
            UID = res;
            res=0;
            cnt++;
        //�����ڶ�������
        }else if(*p ==',' && cnt==2){
            CID = res;
            res=0;
            cnt++;
        //�������з�
        }else if(*p=='\n'){
            money=res;
            res=0;
            cnt=1;
            //printf("UID:%d CID:%d money:%d\n",UID,CID,money);
            //��ȱ�����UID��keyֵ���Ͳ����ٳ�ʼ����CID�����+1
            if(inEdgeMap.find(UID)!=inEdgeMap.end()){
                inEdgeMap[CID]++;
            }else {//��ȱ���û��UID��keyֵ�����벢��ʼ��Ϊ0
                inEdgeMap[UID] = 0;
                inEdgeMap[CID]++;
            }

            //���뵽����
            unordered_map<int,Vertex>::iterator iter = graph.find(UID);
            if(iter != graph.end()){
                //�Ѿ�����key
                iter->second.ID = UID;
                Edge edge(CID,money);
                iter->second.edgeList.push_back(edge);
            } else{
                //������key
                nodeNum++;
                Vertex vertex;
                vertex.ID = UID;
                vertex.edgeList.push_back({CID,money});
                graph[UID] = vertex;
            }
        }
    }
    //���������ų��������ܳɻ��Ķ���
    topSort();
    endTime = clock();
    double totalTime = double(endTime - startTime)/CLOCKS_PER_SEC;
    cout<<"ifstream running time:"<<totalTime<<"s"<<endl;
}

void Discirminant::topSort(){
    //������ʱͼ,����temGraph�����ԣ�ɾ��graph�еĶ��㣬
    unordered_map<int,Vertex> temGraph(graph);

    int cnt=0;
    stack<int> s;

    //������ȱ��ҵ��������Ϊ0�Ķ����±꣬����ջ
    for(auto it:inEdgeMap){
        if(it.second==0){
            cnt++;
            graph.erase(it.first);
            s.push(it.first);
        }
    }
    cout<<"����������"<<nodeNum<<endl;
    cout<<"�޼�ǰ���Ϊ0�Ķ�������"<<cnt<<endl;

    //����ջ����������б�
    while(!s.empty()){
        int ID=s.top();
        s.pop();
        Vertex node =temGraph[ID];

        //��ǰ�������г��ߵĶ������ȶ�Ҫ-1
        for(auto it:node.edgeList){
            inEdgeMap[it.anotherID]--;
            //�޼�����ڽӵ����Ҳ���������Ϊ0��ҲҪ��ջ
            if(inEdgeMap[it.anotherID]==0){
                cnt++;
                graph.erase(it.anotherID);
                s.push(it.anotherID);
            }
        }

    }
    cout<<"��������󲻿����γɻ��Ķ��������"<<cnt<<endl;
    cout<<"�����������Ҫ�����Ķ��������"<<graph.size()<<endl;
}

void Discirminant::writeResult(string path) {
    //res Ϊ���򣬵��Ƕ����Ϲ���Ҫ��
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
//��Ȧ��Ȧ����true������false
bool Discirminant::checkDoubleCircle(int circle [],int size, int vertexID) {
    if(size < 3) return false;
    for(int i = 1; i < size;i++){
        if(circle[i] == vertexID) return true;
    }
    return false;
}


void Discirminant::dfs(int nodeID) {
    //������ȱ��������
    if(dfsDeep >= maxDeep){
        return;
    }

    //��ǰ���
    Vertex & node = graph[nodeID];
    node.wasVisited = true;

    //������ǰ���ı߱�
    for(list<Edge>::iterator iter = node.edgeList.begin();iter !=node.edgeList.end();){

        //��������������
        int *circle = new int[7];
        //memset(circle,-1, sizeof(circle));
        memset(circle,-1, sizeof(int)*7);

        //ȡ����1����
        auto firstEdge = iter;

        //��ͷ��Ϊ��ǰ���
        circle[0] = nodeID;

        //ȡ����1���ߵĽ�㣺��1���ڽӵ�
        Vertex& secondNode = graph[firstEdge->anotherID];

        //�ж��Ƿ񱻷��ʹ�
        //if(secondNode.wasVisited) continue;
        circle[1] = secondNode.ID;

        //������1���ڽӵ�ı߱�
        for(auto &secondEdge:secondNode.edgeList){
            Vertex &thirdNode = graph[secondEdge.anotherID];
            circle[2] = thirdNode.ID;

            //�ų�С��3�ĵ�
            if(thirdNode.ID != nodeID){
                //�����б�
                for(auto &thirdEdge:thirdNode.edgeList){
                    Vertex &fourthNode = graph[thirdEdge.anotherID];

                    //������3����㹹�ɵĻ�
                    if(fourthNode.ID == nodeID){
                        pushResult(circle, 3);
                    }else{
                        //����Ƿ����Ȧ��Ȧ
                        if( checkDoubleCircle(circle,3,fourthNode.ID)) continue;
                        circle[3] = fourthNode.ID;

                        //������4�����ı߱�
                        for(auto &fourthEdge:fourthNode.edgeList){
                            Vertex &fifthNode = graph[fourthEdge.anotherID];
                            if(fifthNode.ID == nodeID){
                                pushResult(circle, 4);
                            } else{
                                //����Ƿ����Ȧ��Ȧ
                                if(checkDoubleCircle(circle,4,fifthNode.ID)) continue;
                                circle[4] = fifthNode.ID;
                                //������5�����ı߱�
                                for(auto &fifthEdge:fifthNode.edgeList){
                                    Vertex &sixthNode = graph[fifthEdge.anotherID];
                                    if(sixthNode.ID == nodeID){
                                        pushResult(circle, 5);
                                    } else{
                                        //����Ƿ����Ȧ��Ȧ
                                        if(checkDoubleCircle(circle,5,sixthNode.ID)) continue;
                                        circle[5] = sixthNode.ID;
                                        //������6�����ı߱�
                                        for (auto &sixEdge:sixthNode.edgeList) {
                                            Vertex &seventhNode = graph[sixEdge.anotherID];
                                            if(seventhNode.ID == nodeID){
                                                pushResult(circle, 6);
                                            } else{
                                                //����Ƿ����Ȧ��Ȧ
                                                if(checkDoubleCircle(circle,6,seventhNode.ID)) continue;
                                                circle[6] = seventhNode.ID;
                                                //������7�����ı߱�
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
        //��֦
        iter = node.edgeList.erase(iter);

        //�ݹ�
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
    //ȡ����СID��ͷ��
    int head = 0;
    int minID = INT_MAX;
    for(int i = 0; i <size && data[i] != -1;i++){
        if(minID > data[i]){
            head = i;
            minID = data[i];
        }
    }
    //����vec��
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
        //δ�����ʹ�������Ȳ�Ϊ0
        if(!node.second.wasVisited){
            dfs(node.second.ID);
        }
    }
}

int main() {
    //system("chcp 65001"); //���win�´�ӡ�����������
    string testFile = "../data/myText.txt";
    string resultFile = "../projects/student/result.txt";

    clock_t startTime,endTime,endTime1;
    startTime = clock();

    Discirminant discirminant;
    discirminant.readTestData(testFile);

    //�����㷨
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