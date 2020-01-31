/*
Student Name: Zuhal Didem Aytaç
Student Number: 2018400045
Project Number: 1
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/

#include "Restaurant.h"

using namespace std;

void Restaurant::execute(const vector<string>& operations) {

    for (const string& op : operations){
        cout << "Operation: " << op << "\t"; 

        // if somebody enters from the south entrance
        if(op.compare("S")==0){
            //checking the tables from south to north
            for(int i=this->tables.size()-1; i>=0; i--){
                Table &current = (this->tables).at(i);
                if (!current.isOccupied()){
                    //occupy and break immediately when an empty table is found
                    current.occupy();
                    break;
                }
            }
        }


        // if somebody enters from the north entrance
         else if(op.compare("N")==0) {
             //checking the tables from north to south
            for(int i=0; i<this->tables.size(); i++){
                Table &current = (this->tables).at(i);
                if (!current.isOccupied()){
                    //occupy and break immediately when an empty table is found
                    current.occupy();
                    break;
                }

            }
        }


         // if somebody leaves
         else{
             int num;
             // getting the integer value of the operation
             num = stoi(op);
             Table &current = (this->tables).at(num-1);
             if(current.isOccupied()) {
                 //emptying that table
                 current.empty();
             }

        }



        cout << "State: " << *this << endl;
    }
}

Restaurant::Restaurant(unsigned int N)
	: tables{N}
{
    cout << "-Starting- \tState: " << *this << endl;
}

ostream &operator<<(ostream &out, Restaurant &restaurant) {
    for (Table table : restaurant.tables) {
        out << table.isOccupied();
    }
	return out;
}

