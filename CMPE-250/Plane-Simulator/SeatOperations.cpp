/*
Student Name: Zuhal Didem Aytaç
Student Number: 2018400045
Project Number: 3
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/
#include "SeatOperations.h"

using namespace std;

SeatOperations::SeatOperations(int N, int M){
    Person *emptyP= new Person;
    emptyP->seatNumber=emptyP->line=emptyP->type=emptyP->type3LastOperation-1;
    for(int i=0;i<N; i++)
        lines[0].push_back(*emptyP);
    for(int i=0;i<M; i++)
        lines[1].push_back(*emptyP);
}

void SeatOperations::addNewPerson(int personType, const string& ticketInfo){
    Person *current= new Person();
    (*current).type=personType;
    (*current).type3LastOperation=0;
    string ticket=ticketInfo;
    int lineNo= ticket.at(0)-'A';
    (*current).line= lineNo;
    int seatNo= stoi(ticket.substr(1,ticket.length()));
    (*current).seatNumber=seatNo;
    int length=lines[lineNo].size();
    int seatIndex= (seatNo-1)%length;
    Person sittingThere=lines[lineNo].at(seatIndex);
    lines[lineNo].at(seatIndex)=*current;
    if(sittingThere.type!=-1)
        moveSittingPerson(sittingThere,lineNo,seatIndex);


}

void SeatOperations::moveSittingPerson(Person pass, int lastLineNo, int lastSeatIndex){
    if(pass.type==1){
        int newLineNo=abs(lastLineNo-1);
        int length=lines[newLineNo].size();
        int newSeatIndex= (pass.seatNumber-1)%length;
        Person sittingThere=lines[newLineNo].at(newSeatIndex);
        lines[newLineNo].at(newSeatIndex)=pass;
        if(sittingThere.type!=-1)
            moveSittingPerson(sittingThere,newLineNo,newSeatIndex);
    }
    else if(pass.type==2){
        int length=lines[lastLineNo].size();
        int newSeatIndex=lastSeatIndex+1;
        int newLineNo=lastLineNo;
        if(newSeatIndex==length){
            newLineNo=abs(lastLineNo-1);
            length=lines[newLineNo].size();
            newSeatIndex=0;
        }
        Person sittingThere=lines[newLineNo].at(newSeatIndex);
        lines[newLineNo].at(newSeatIndex)=pass;
        if(sittingThere.type!=-1)
            moveSittingPerson(sittingThere,newLineNo,newSeatIndex);

    }
    else{
        int old= pass.type3LastOperation;
        pass.type3LastOperation+=1;
        int leftMoves=pass.type3LastOperation*pass.type3LastOperation - old*old;
        int newLineNo=lastLineNo;
        int newSeatIndex=lastSeatIndex;
        int currentLength= lines[newLineNo].size();
        if(currentLength>newSeatIndex+leftMoves){ //there is no problem
            newSeatIndex=lastSeatIndex+leftMoves;
        }
        else{
            int movedSeats=currentLength-1-lastSeatIndex;
            leftMoves-=movedSeats;
            while(leftMoves>0){
                newLineNo=abs(newLineNo-1);
                currentLength= lines[newLineNo].size();
                if(leftMoves>currentLength)
                    leftMoves-=currentLength;
                else{
                    newSeatIndex=leftMoves-1;
                    leftMoves=0;
                }

            }



        }

        Person sittingThere=lines[newLineNo].at(newSeatIndex);
        lines[newLineNo].at(newSeatIndex)=pass;
        if(sittingThere.type!=-1)
            moveSittingPerson(sittingThere,newLineNo,newSeatIndex);

    }

}

void SeatOperations::printAllSeats(ofstream& outFile){
    for(int i=0; i<lines[0].size(); i++){
        if(lines[0].at(i).type==-1)
            outFile<<"0"<<endl;
        else {
            outFile << lines[0].at(i).type << " ";
            lines[0].at(i).line==0 ? outFile<<'A' :outFile<<'B';
            outFile << lines[0].at(i).seatNumber << endl;
        }
    }
    for(int i=0; i<lines[1].size(); i++){
        if(lines[1].at(i).type==-1)
            outFile<<"0"<<endl;
        else {
            outFile << lines[1].at(i).type << " ";
            lines[1].at(i).line==0 ? outFile<<'A' :outFile<<'B';
            outFile << lines[1].at(i).seatNumber << endl;
        }
    }
}

// YOU CAN ADD YOUR HELPER FUNCTIONS