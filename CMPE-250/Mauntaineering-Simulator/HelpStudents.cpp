
/*
Student Name: ZUHAL DIDEM AYTAC
Student Number: 2018400045
Project Number: 4
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/
#include "HelpStudents.h"

using namespace std;

HelpStudents::HelpStudents(int  N, int  M, int K, vector < pair< pair <int,int> , int > > ways) {
    //N nodes, M undirected edges with ime value, K:summit
    //nodes:1,,, n. k:summit
    (*this).N=N;
    (*this).M=M;
    (*this).K=K-1;
    (*this).adj= vector<vector<pair<int,lli>>>(N);
    (*this).distances= vector<lli> (N,-1);
    (*this).visited=vector<bool> (N,false);


    for(int i=0; i<ways.size(); i++){
        int firstEdge= (ways[i].first.first)-1;
        int secondEdge= (ways[i].first.second)-1;
        lli weight= ways[i].second;
        adj[firstEdge].push_back(make_pair(secondEdge,weight));
        adj[secondEdge].push_back(make_pair(firstEdge,weight));

    }

}

long long int HelpStudents::firstStudent() { // fast, shortest path: DIJKSTRA

    int startNode=0;
    distances[startNode]=0;
    priority_queue<pair<lli,int>, vector<pair<lli,int>>,greater<pair<lli,int>> > ourQueue;
    ourQueue.push(make_pair(0,startNode));

    while(!ourQueue.empty()){
        pair<lli,int> current= ourQueue.top();
        ourQueue.pop();

        lli currentLength=current.first;
        int currentNode=current.second;

        if(visited[currentNode])
            continue;

        visited[currentNode]=true;

        for(auto it: adj[currentNode]){
            lli adjLength= it.second;
            int adjNode= it.first;

            if((distances[adjNode]!=-1 &&distances[currentNode]+adjLength<distances[adjNode])||(distances[adjNode]==-1)){
                distances[adjNode]=distances[currentNode]+adjLength;
                ourQueue.push(make_pair(distances[adjNode], adjNode));
            }

        }

    }

    return (distances[(*this).K]);

}

long long int HelpStudents::secondStudent() { // through shorter edges,  minimize the duration of the maximum edge: MINIMUM SPANNING TREE: PRIMS
    priority_queue<pair<lli,int>, vector<pair<lli,int>>,greater<pair<lli,int>> > ourQueue;
    vector<lli> costs= vector<lli> (N,LLONG_MAX/2);
    vector<int> parents= vector<int> (N,-1);
    vector<vector<pair<int,lli>>>validEdges= vector<vector<pair<int,lli>>>(N);

    int startNode=0;
    costs[startNode]=0;
    ourQueue.push(make_pair(0,startNode));
    while(!ourQueue.empty()){
        pair<lli,int> current= ourQueue.top();
        ourQueue.pop();

        lli currentLength=current.first;
        int currentNode=current.second;
        if(!visited[currentNode]){
            visited[currentNode]=true; //it's in MST now
            if(currentNode!=0){
                validEdges[parents[currentNode]].push_back(make_pair(currentNode,costs[currentNode]));
                validEdges[currentNode].push_back(make_pair(parents[currentNode],costs[currentNode]));
            }

            for(auto it: adj[currentNode]){
                lli adjLength= it.second;
                int adjNode= it.first;
                if(adjLength<costs[adjNode]){
                    costs[adjNode]=adjLength;
                    ourQueue.push(make_pair(adjLength,adjNode));
                    parents[adjNode]=currentNode;


                }
            }
        }
    }

    lli maximum=-1;

    int currentNode=0;
    queue <int > q;
    distances[currentNode]=0;
    q.push(currentNode);
    vector<bool> mark(N,false);

    while(!q.empty()){
        currentNode=q.front();
        q.pop();
        mark[currentNode]=1;
        for(auto it: validEdges[currentNode]) {
            int adjNode = it.first;
            lli adjLength= it.second;
            if(mark[adjNode])
                continue;
            distances[adjNode]=max(distances[currentNode],adjLength);
            if(!mark[adjNode])
                q.push(adjNode);

        }
    }

    return distances[K];



}

long long int HelpStudents::thirdStudent() { // minimum # of nodes: DIJKSTRA ALL 1
    int startNode=0;
    distances[startNode]=0;
    priority_queue<pair<lli,int>, vector<pair<lli,int>>,greater<pair<lli,int>> > ourQueue;
    ourQueue.push(make_pair(0,startNode));

    while(!ourQueue.empty()){
        pair<lli,int> current= ourQueue.top();
        ourQueue.pop();

        lli currentLength=current.first;
        int currentNode=current.second;

        if(visited[currentNode])
            continue;

        visited[currentNode]=true;

        for(auto it: adj[currentNode]){
            lli adjLength= 1;
            int adjNode= it.first;

            if((distances[adjNode]!=-1 &&distances[currentNode]+adjLength<distances[adjNode])||(distances[adjNode]==-1)){
                distances[adjNode]=distances[currentNode]+adjLength;
                ourQueue.push(make_pair(distances[adjNode], adjNode));
            }

        }

    }

    return (distances[(*this).K]);


}

long long int HelpStudents::fourthStudent() { // greedy: shortest edge out of current node. use each edge only once. if same, smaller edge num: GREEDY
    int currentNode=0;
    lli distance=0;
    lli iterations=0;


    while(currentNode!=K && iterations<M+5){
        visited[currentNode]=true;
        lli smallerDist= LLONG_MAX/2;
        lli smallerNode=LLONG_MAX/2;
        for(auto it: adj[currentNode]) {
            lli adjLength = it.second;
            int adjNode = it.first;
            if(adjLength!=-1) {
                if (adjLength == smallerDist && adjNode < smallerNode) {
                    smallerDist = adjLength;
                    smallerNode = adjNode;
                } else if (adjLength < smallerDist) {
                    smallerDist = adjLength;
                    smallerNode = adjNode;
                }
            }
        }

        //kendi adjlerim arasında en kücüğü buldum
        //currentNode'dan smaller Node'a gideceğim
        //tekrar kullanmamalıyım

        for(int i=0; i<(adj[currentNode]).size(); i++){
            if((adj[currentNode]).at(i).first==smallerNode) {
                adj[currentNode].at(i).second = -1;
                continue;
            }
        }
        for(int i=0; i<(adj[smallerNode]).size(); i++){
            if((adj[smallerNode]).at(i).first==currentNode) {
                adj[smallerNode].at(i).second =-1;
                continue;
            }
        }


        iterations++; //I used an edge
        distance +=smallerDist;
        currentNode=smallerNode;
    }

    if(currentNode!=K)
        return -1;
    return distance;

}

long long int HelpStudents::fifthStudent() { // teleport in 0 time, no power in beginning. pass 2 edges to recharge, can use an edge multiple times: DIJK. WITH DISTANCE 3 ARRAY
    vector<long long int> distances1= vector<long long int> (N,LONG_LONG_MAX/2);//ışınlanarak geldiklerim
    vector<long long int> distances2= vector<long long int> (N,LONG_LONG_MAX/2);//1 adımda geldiklerim
    vector<long long int> distances3= vector<long long int> (N,LONG_LONG_MAX/2);//2 adımda geldiklerim

    int startNode=0;
    distances1[startNode]=0;


    priority_queue<pair<lli,int>, vector<pair<lli,int>>,greater<pair<lli,int>> > ourQueue;
    ourQueue.push(make_pair(0,startNode));

    while(!ourQueue.empty()){
        pair<lli,int> current= ourQueue.top();
        ourQueue.pop();

        lli currentLength=current.first;
        int currentNode=current.second;




        for(auto it: adj[currentNode]){
            lli adjLength= it.second;
            int adjNode= it.first;

            if(distances1[currentNode]+adjLength<distances2[adjNode]){
                distances2[adjNode]=distances1[currentNode]+adjLength;
                ourQueue.push(make_pair(distances2[adjNode], adjNode));
            }
            if(distances2[currentNode]+adjLength<distances3[adjNode]){
                distances3[adjNode]=distances2[currentNode]+adjLength;
                ourQueue.push(make_pair(distances3[adjNode], adjNode));
            }
            if(distances3[currentNode]<distances1[adjNode]){ //ısınlanıyorum
                distances1[adjNode]=distances3[currentNode];
                ourQueue.push(make_pair(distances1[adjNode], adjNode));
            }
        }

    }

    return (min(min(distances1[(*this).K],distances2[(*this).K]),distances3[(*this).K]));

}

// YOU CAN ADD YOUR HELPER FUNCTIONS

