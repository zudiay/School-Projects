/*
Student Name:   Zuhal Didem Aytaç
Student Number: 2018400045
Project Number: 2
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/

#include "RestaurantOrganizer.h"

using namespace std;

RestaurantOrganizer::RestaurantOrganizer(const vector<int>& tableCapacityInput){
    numberOfTables = tableCapacityInput.size();
    for(int i=0;i<numberOfTables;i++){
        tableCapacity[i] = tableCapacityInput[i];
        heap[i] = i;
        heapUp(i);

    }

}

void RestaurantOrganizer::addNewGroup(int groupSize, ofstream& outFile){
    // IMPLEMENT ME!
    int maxcap=tableCapacity[heap[0]];

    if(groupSize>maxcap) {
        outFile << "-1"<<endl;
    }
    else{
        int newCap= maxcap-groupSize;
        tableCapacity[heap[0]]=newCap;
        outFile<< heap[0]<< endl;
        heapDown(0);

    }


}

void RestaurantOrganizer::heapUp(int index){


    int tableno= index;
    int tablecap= tableCapacity[tableno];

    for(int i=index; i>0; i=(i-1)/2){
        int parentnum= (i-1)/2;
        if(tableCapacity[heap[parentnum]]<tablecap){
            swap(heap[i],heap[parentnum]);
        }

    }
}

void RestaurantOrganizer::heapDown(int index){
    int tableNo,leftCap,rightCap;
    for(int i=index; i<MAX_TABLES; ){
        int tableIndex= i;
        tableNo= heap[i];
        int tableCap= tableCapacity[tableNo];
        int leftChildIndex, rightChildIndex;
        if(i==0){
            leftChildIndex=1;
            rightChildIndex=2;
        }
        else {
            leftChildIndex = 2 * i +1;
            rightChildIndex = 2 * i + 2;
        }
        if(rightChildIndex > numberOfTables-1 || leftChildIndex > numberOfTables-1 ){
            if(rightChildIndex > numberOfTables-1 && leftChildIndex > numberOfTables-1 )
                break;

            else if(rightChildIndex > numberOfTables-1 ){
                leftCap= tableCapacity[heap[leftChildIndex]];
                swap(heap[i],heap[leftChildIndex]);
                break;
            }

            else if(leftChildIndex  > numberOfTables-1 ){
                rightCap= tableCapacity[heap[rightChildIndex]];
                swap(heap[i],heap[rightChildIndex]);
                break;
            }
        }
         leftCap= tableCapacity[heap[leftChildIndex]];
         rightCap= tableCapacity[heap[rightChildIndex]];



        if(tableCap<leftCap || tableCap<rightCap) {
            if(leftCap==rightCap){
                int leftNum=heap[leftChildIndex];
                int rightNum=heap[rightChildIndex];
                if(rightNum<leftNum){
                    swap(heap[i],heap[rightChildIndex]);
                    i = rightChildIndex;
                }
                else{
                    swap(heap[i],heap[leftChildIndex]);
                    i = leftChildIndex;
                }

            }
            else if (leftCap > rightCap) {
                swap(heap[i],heap[leftChildIndex]);
                i = leftChildIndex;
            } else if (leftCap < rightCap) {
                swap(heap[i],heap[rightChildIndex]);
                i = rightChildIndex;
            }
        }
        else if(tableCap==leftCap){
            int leftNum=heap[leftChildIndex];
            if(tableNo>leftNum){
                swap(heap[i],heap[leftChildIndex]);
                i = leftChildIndex;
            }
            else{
                break;
            }

        }
        else if(tableCap==rightCap){
            int rightNum=heap[rightChildIndex];
            if(tableNo>rightNum){
                swap(heap[i],heap[rightChildIndex]);
                i = rightChildIndex;
            }
            else{
                break;
            }

        }

        else{ // no child is larger than the current parent. We are done.
            break;
        }
    }
}

void RestaurantOrganizer::printSorted(ofstream& outFile){
    cout<< endl;
    for(int i=0; i<numberOfTables; i++){
        outFile << tableCapacity[heap[0]]<< " ";
        tableCapacity[heap[0]]=-1;
        heapDown(0);
    }
}


// YOU CAN ADD YOUR HELPER FUNCTIONS

