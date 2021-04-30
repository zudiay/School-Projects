/* Z. Didem Aytac - Ezgi A. Batı
 * 2018400045 - 2018400207
 * CMPE 230 Systems Programming Project
 * An interpreter for an assembly language of a hypothetical 8086-like CPU called HYP86
 */

/*
<reg16>	Any 16-bit register (AX,BX, CX, DX, DI, SP, SI, BP)
<reg8>	Any 8-bit register (AH, AL, BH, BL, CH, CL, DH, DL)
<reg>	Any register
<mem>	A memory address (e.g., [eax], [var + 4], or dword ptr [eax+ebx])
<con16>	Any 16-bit constant
<con8>	Any 8-bit constant
<con>	Any 8-, 16-, or 32-bit constant
 */

using namespace std;
#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include "math.h"
#include <fstream>
#include <string>
#include <vector>
#include <string.h>

// prototypes
template <class datatype> void print_bits(datatype x) ;
template <class datatype> void print_hex(datatype x) ;
void print_16bitRegs() ;
void print_8bitRegs();



// Global Variables ( memory, registers and flags )
unsigned char memory[2<<15] ;    // 64K memory
unsigned short PC = 0 ; //todo use???
class reg;
map<string, reg*> eightBitRegs;
map<string, reg*> sixteenBitRegs;
vector<string> lines;   // instructions read from file
map<string,int> labels; //string is the label name, int is the line number the label starts with.
map<string,pair<int,unsigned short> > vars; //string is variable name, pair is variable address
int linecount;


unsigned concat(unsigned char a, unsigned char b);


class reg {
public:
    int regType;
    unsigned char eightValue;
    unsigned short sixteenValue;
    unsigned char low;
    unsigned char high;
    string name;

    reg(int type, string name){  //default constructor

        regType=type;
        eightValue=0;
        sixteenValue=0;
        low=0;
        high=0;
        this->name=name;
    }
    void update_8(unsigned char val){
        this->eightValue=val;
        for (auto itr = sixteenBitRegs.begin(); itr != sixteenBitRegs.end(); itr++) {
            string nn= itr->second->name;
            if((itr->second->name)[1]=='x'){
                if((itr->second->name)[0]==(this->name)[0]){
                    if((this->name)[1]=='h'){
                        itr->second->high=val;
                        itr->second->sixteenValue=val*256+itr->second->low;
                    }
                    else{
                        itr->second->low=val;
                        itr->second->sixteenValue=(itr->second->high)*256+val;
                    }
                    break;
                }
            }

        }

    }
    void update_16(unsigned short val){
        this->sixteenValue=val;
        this->high = (unsigned char) ((unsigned char)(val >>8)) ; //8 most significant bits
        this->low = (unsigned char) val& ((1<<8)-1) ; // 8 least significant bits
        if(this->name[1]=='x'){
            int count=0;
            for (auto itr = eightBitRegs.begin(); itr != eightBitRegs.end(); itr++) {
                if(count==2)
                    break;
                if((itr->second->name)[0]==(this->name)[0]){
                    count++;
                    if((itr->first)[1]=='h')
                        itr->second->eightValue=high;
                    else
                        itr->second->eightValue=low;

                }
            }
        }

    }

};



//FUNCTIONS FOR INSTRUCTIONS
void and_opr(reg* reg1, reg* reg2);
void and_opr(reg* reg1, pair<int, unsigned short> realAddress);
void and_opr(pair<int, unsigned short> realAddress, reg* reg1);
void and_opr(reg* reg1, unsigned constant, char constType);
void and_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType);

void or_opr(reg* reg1, reg* reg2);
void or_opr(reg* reg1, pair<int, unsigned short> realAddress);
void or_opr(pair<int, unsigned short> realAddress, reg* reg1);
void or_opr(reg* reg1, unsigned constant, char constType);
void or_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType);

void xor_opr(reg* reg1, reg* reg2);
void xor_opr(reg* reg1, pair<int, unsigned short> realAddress);
void xor_opr(pair<int, unsigned short> realAddress, reg* reg1);
void xor_opr(reg* reg1, unsigned constant, char constType);
void xor_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType);

void not_opr(reg* reg1);
void not_opr(pair<int, unsigned short> realAddress);

void shift_right_opr(reg* reg1, unsigned  constant);
void shift_right_opr(pair<int, unsigned short> realAddress, unsigned  constant);
void shift_right_opr(pair<int, unsigned short> realAddress, reg* reg2);
void shift_right_opr(reg* reg1, reg* reg2);

void shift_left_opr(reg* reg1, unsigned  constant);
void shift_left_opr(pair<int, unsigned short> realAddress, unsigned  constant);
void shift_left_opr(pair<int, unsigned short> realAddress, reg* reg2);
void shift_left_opr(reg* reg1, reg* reg2);

void rotate_right_opr(reg* reg1, unsigned  constant);
void rotate_right_opr(pair<int, unsigned short> realAddress, unsigned  constant);
void rotate_right_opr(pair<int, unsigned short> realAddress, reg* reg2);
void rotate_right_opr(reg* reg1, reg* reg2);

void rotate_left_opr(reg* reg1, unsigned  constant);
void rotate_left_opr(pair<int, unsigned short> realAddress, unsigned  constant);
void rotate_left_opr(pair<int, unsigned short> realAddress, reg* reg2);
void rotate_left_opr(reg* reg1, reg* reg2);

void add_opr(reg* reg1, reg* reg2);
void add_opr(reg* reg, pair<int,unsigned short> address);
void add_opr(pair<int,unsigned short> address,reg* reg);
void add_opr(reg* reg, unsigned short constant);
void add_opr(pair<int,unsigned short> address, unsigned short constant);

void sub_opr(reg* reg1, reg* reg2);
void sub_opr(reg* reg, pair<int,unsigned short> mem);
void sub_opr(pair<int,unsigned short>mem, reg* reg);
void sub_opr(reg* reg, unsigned short con);
void sub_opr(pair<int,unsigned short>mem, unsigned short con);

void inc_opr(reg* reg);
void inc_opr(pair<int,unsigned short> address);
void dec_opr(reg* reg);
void dec_opr(pair<int,unsigned short> address);

void mul_opr(reg* source);
void mul_opr(pair<int,unsigned short>address);
void div_opr(reg* operand);
void div_opr(pair<int,unsigned short>address);

void mov_opr(reg* reg1, reg* reg2);
void mov_opr(reg* reg1, pair<int, unsigned short> realAddress);
void mov_opr(pair<int, unsigned short> realAddress, reg* reg1);
void mov_opr(reg* reg1, unsigned constant, char constType);
void mov_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType);

void push_opr(reg* reg);
void push_opr(pair<int, unsigned short> realAddress);
void push_opr(unsigned constant,char constType);
void pop_opr(reg* reg);
void pop_opr(pair<int, unsigned short> realAddress);


void cmp_opr(reg* reg1, reg* reg2);
void cmp_opr(reg* reg, pair<int,unsigned short> mem);
void cmp_opr(pair<int,unsigned short>mem, reg* reg);
void cmp_opr(reg* reg, unsigned short con, char constType);


//HELPER FUNCTIONS
unsigned long hex2dec(string hex);
pair<int,unsigned int> realAddressFinder(string address);
char findOperandType(string op);
unsigned long long int decToBinary(unsigned long long int n);
unsigned char MSB_16(unsigned short temp);
unsigned char MSB_8(unsigned char temp);
unsigned char LSB_16(unsigned short temp);
unsigned char LSB_8(unsigned char temp);


//DECLARING REGISTERS, FLAGS AND POINTERS
//16-bit registers: AX, BX, CX, DX, DI, SP, SI, BP.
reg ax(16,"ax"),bx(16,"bx"),cx(16,"cx"),dx(16,"dx"),di(16,"di"),si(16,"si"),bp(16,"bp"),sp(16,"sp");
//8-bit registers: AH, AL, BH, BL, CH, CL, DH, DL.
reg ah(8,"ah"), al(8,"al"), bh(8,"bh"), bl(8,"bl"), ch(8,"ch"), cl(8,"cl"), dh(8,"dh"), dl(8,"dl");
// It has the following flags: ZF zero flag, CF carry flag, AF auxiliary flag, SF sign flag , OF overflow flag.
//The zero flag (ZF) is set when the result of an arithmetic or logical operation generates a result of zero.
bool zf ; // zero flag
//The carry flag (CF) is set when the result of an unsigned arithmetic operation is too large to fit into the destination
bool cf ; // carry flag
bool af ; // auxiliary flag
//The sign flag (SF) is set when the result of an arithmetic or logical operation generates a negative result.
bool sf ; // sign flag
//The overflow flag (OF) is set when the result of a signed arithmetic operation is too large or too small to fit into the destination.
bool of ; // overflow flag

// Initializing pointers
reg* pax = &ax ;
reg* pbx = &bx ;
reg* pcx = &cx ;
reg* pdx = &dx ;
reg* psp = &sp ;
reg* psi = &si ;
reg* pbp = &bp ;
// x86 uses little endian, that is, least significant byte is stored in lowest byte
reg* pah = &ah;
reg* pal = &al;
reg* pbh = &bh;
reg* pbl = &bl;
reg* pch = &ch;
reg* pcl = &cl;
reg* pdh = &dh;
reg* pdl = &dl;




int main(int argc, char* argv[]) {

    //ESSENTIAL STUFF
    sp.update_16((2<<15)-2);  //  SP initially contains FFFE
    sixteenBitRegs.insert(pair<string, reg*>("ax",&ax));
    sixteenBitRegs.insert(pair<string, reg*>("bx",&bx));
    sixteenBitRegs.insert(pair<string, reg*>("cx",&cx));
    sixteenBitRegs.insert(pair<string, reg*>("dx",&dx));
    sixteenBitRegs.insert(pair<string, reg*>("di",&di));
    sixteenBitRegs.insert(pair<string, reg*>("si",&si));
    sixteenBitRegs.insert(pair<string, reg*>("sp",&sp));
    sixteenBitRegs.insert(pair<string, reg*>("bp",&bp));
    eightBitRegs.insert(pair<string, reg*>("ah",&ah));
    eightBitRegs.insert(pair<string, reg*>("al",&al));
    eightBitRegs.insert(pair<string, reg*>("bh",&bh));
    eightBitRegs.insert(pair<string, reg*>("bl",&bl));
    eightBitRegs.insert(pair<string, reg*>("ch",&ch));
    eightBitRegs.insert(pair<string, reg*>("cl",&cl));
    eightBitRegs.insert(pair<string, reg*>("dh",&dh));
    eightBitRegs.insert(pair<string, reg*>("dl",&dl));
    // ESSENTIAL STUFF ENDS

    string line;
    ifstream infile(argv[1]);
    if (!infile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }
    if (infile.is_open()){
        //First reading, takes the input lines and declares variables
        while ( getline(infile,line)){
            line = line.substr(line.find_first_not_of(" \t"),line.length()); //trims whitespaces
            line= line.substr(0,line.length()-1);
            if(line.find("code segment")!=string::npos) continue;
            if(line.compare("int 20h")==0) {
                linecount++;
                lines.push_back(line);
                break;
            }
            lines.push_back(line);
            if(line.find(':')==string::npos) linecount++; //counts non-label instructions to fill memory space accordingly
            else{
                labels.insert(pair<string,int>(line.substr(0,line.length()-1),labels.size()+linecount));
            }
        }
        int pos = linecount*6;//pos corresponds to direct memory[] index, so it is used without calling realAddressFinder method.
        while(getline(infile,line)){
            line= line.substr(0,line.length()-1);

            if(line.compare("code ends")==0) break;
            if(line.find("db")!=string::npos ||line.find("dw")!=string::npos){//declaring variable
                char linr[line.size()+5];
                strcpy(linr,line.c_str());
                string varname=strtok(linr," ");
                string vartype=strtok(NULL," ");
                if(vartype.compare("db")==0){
                    vars.insert(pair<string,pair<int,unsigned short> >(varname,make_pair('c',pos)));
                    string val=strtok(NULL," ");
                    if(val.compare("?")!=0){
                        if(val.find('h')!=string::npos || val[0]=='0'){
                            memory[pos] = hex2dec(val);
                        }else{
                            if(val[0]=='\'' && val[2]=='\''){
                                memory[pos]= val[1];
                            }
                            else if(stoi(val)>255){
                                cout<<"ERROR"<<endl;
                                exit(0);
                            }
                            else memory[pos]= stoi(val);
                        }
                    }
                    pos++;
                }
                else{
                    vars.insert(pair<string,pair<int,unsigned short> >(varname,make_pair('s',pos)));
                    string val=strtok(NULL," ");
                    if(val.compare("?")!=0){
                        unsigned short valv;
                        if(val.find('h')!=string::npos){
                            valv = hex2dec(val);
                        }else{
                            if(val[0]=='\'' && val[2]=='\''){
                                valv = val[1];
                            }
                            else valv= stoi(val);
                        }
                        if(valv<256){
                            memory[pos]=valv;
                            memory[pos+1]=0;
                        }
                        else if(valv>65535){
                            cout<<"ERROR"<<endl;
                            exit(0);
                        }
                        else{
                            unsigned char hi = valv >> 8;
                            unsigned char lo = valv - (hi << 8);
                            memory[pos] = lo;
                            memory[pos+1] = hi;
                        }
                    }
                    pos+=2;
                }
            }
        }
        infile.close();
    }
    //Second reading, executes the instructions
    for(int i=0; i<lines.size(); i++){
        string currentLine= lines[i];
        char str[lines[i].size()+5];
        strcpy(str, currentLine.c_str());
        string curs;
        char* pch;
        curs = strtok(str," ,");
        if(curs.find(':')!=string::npos || curs.compare("nop")==0){//this is a label or no operation
            continue;
        }
            //Interrupt Instructions
        else if(curs.compare("int")==0){
            string op = strtok(NULL, " ");
            if(op.compare("20h")==0){ //Exit to OS.
                return 0;
            }else{//int 21h
                if(ah.eightValue==1){//Read character from stdin with Echo
                    unsigned char ne;
                    cin >>ne;
                    pal->update_8(ne);
                    continue;
                    //cout <<ne;
                }
                else if(ah.eightValue==2){//Write character to stdout
                    cout <<dl.eightValue;
                    pal->update_8(dl.eightValue);
                    continue;
                }
            }
        }
            //Jump and conditional jump instructions
        else if(curs.compare("jmp")==0){
            string label = strtok(NULL, " ,");
            i = labels[label];
            continue;
        }
        else if(curs.compare("jnz")==0 ||curs.compare("jne")==0){
            string label = strtok(NULL, " ,");
            if(!zf){
                i = labels[label];
            }
            continue;
        }
        else if(curs.compare("jz")==0 ||curs.compare("je")==0){
            string label = strtok(NULL, " ,");
            if(zf){
                i = labels[label];
            }
            continue;
        }
        else if(curs.compare("ja")==0 ||curs.compare("jnbe")==0){
            string label = strtok(NULL, " ,");
            if(!zf && !cf){
                i = labels[label];
            }
            continue;
        }
        else if(curs.compare("jae")==0 ||curs.compare("jnb")==0 ||curs.compare("jnc")==0){
            string label = strtok(NULL, " ,");
            if(!cf){
                i = labels[label];
            }
            continue;
        }
        else if(curs.compare("jb")==0 ||curs.compare("jnae")==0 ||curs.compare("jc")==0){
            string label = strtok(NULL, " ,");
            if(cf){
                i = labels[label];
            }
            continue;
        }
        else if(curs.compare("jbe")==0){
            string label = strtok(NULL, " ,");
            if(cf || zf ){
                i = labels[label];
            }
            continue;
        }

        string ccurr = currentLine.substr(currentLine.find(curs)+curs.length(),currentLine.length());
        ccurr.erase(remove(ccurr.begin(), ccurr.end(), ' '), ccurr.end());
        char str2[ccurr.size()+5];
        strcpy(str2, ccurr.c_str());


        //operation instructions
        if(curs.compare("not")==0 || curs.compare("mul")==0 || curs.compare("div")==0 ||
           curs.compare("push")==0 || curs.compare("pop")==0){ //takes one operand
            string op = strtok(str2, " ,");
            char optype = findOperandType(op);
            if(op.find("offset")!=string::npos){//If the operand is given with offset, then the address of given var is taken as constant
                optype = 'c';
                string vargiven = op.substr(6,op.length());
                op = to_string(vars[vargiven].second);
            }
            if(optype == 'm'){
                if(curs.compare("not") == 0) not_opr(realAddressFinder(op));
                else if(curs.compare("mul") == 0) mul_opr(realAddressFinder(op));
                else if(curs.compare("div") == 0) div_opr(realAddressFinder(op));
                else if(curs.compare("push") == 0) push_opr(realAddressFinder(op));
                else if(curs.compare("pop") == 0) pop_opr(realAddressFinder(op));
            }else if(optype == 'r'){
                reg* curr;
                if(eightBitRegs.find(op) != eightBitRegs.end()) curr = eightBitRegs[op];
                else curr = sixteenBitRegs[op];

                if(curs.compare("not")==0) not_opr(curr);
                else if(curs.compare("mul")==0) mul_opr(curr);
                else if(curs.compare("div")==0) div_opr(curr);
                else if(curs.compare("push")==0) push_opr(curr);
                else if(curs.compare("pop")==0) pop_opr(curr);

            }else if(optype == 'v'){
                if(curs.compare("not") == 0) not_opr(vars[op]);
                else if(curs.compare("mul") == 0) mul_opr(vars[op]);
                else if(curs.compare("div") == 0) div_opr(vars[op]);
                else if(curs.compare("push") == 0) push_opr(vars[op]);
            }else {//call with constant
                // if(curs.compare("push") == 0 && stoi(op)<256) push_opr(stoi(op));
            }
        }
        else if(curs.compare("add")==0 || curs.compare("sub")==0 || curs.compare("and")==0 || curs.compare("or")==0 || curs.compare("xor")==0 || curs.compare("mov")==0
                || curs.compare("cmp")==0|| curs.compare("shr")==0|| curs.compare("shl")==0|| curs.compare("rcr")==0|| curs.compare("rcl")==0){ //takes two operands

            string op1 = strtok (str2, " ,");
            string op2 = strtok (NULL, " ,");
            if(op2=="''"){
                op2="' '";
            }

            // r for reg, m for memory, c for constant
            char op1type=findOperandType(op1);
            char op2type;
            if(op2.find("offset")!=string::npos){//If the operand is given with offset, then the address of given var is taken as constant
                string vargiven = op2.substr(6,op2.length());
                op2type = 'c';
                op2 = to_string(vars[vargiven].second);
            }else{
                op2type=findOperandType(op2);
            };
            if(op1type == 'm'){
                if(op2type == 'r'){//<mem><reg>
                    reg* curr;
                    if(eightBitRegs.find(op2) != eightBitRegs.end()) curr = eightBitRegs[op2];
                    else curr = sixteenBitRegs[op2];
                    if(curs.compare("add") == 0) add_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("sub") == 0) sub_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("and") == 0) and_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("or") == 0) or_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("xor") == 0) xor_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("mov") == 0) mov_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("cmp") == 0) cmp_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("shr") == 0) shift_right_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("shl") == 0) shift_left_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(realAddressFinder(op1),curr);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(realAddressFinder(op1),curr);
                }
                    //todo mov<mem><var>
                else { //<mem><con>
                    char temp;
                    if(realAddressFinder(op1).first==8) temp = 'c';
                    else temp = 's';
                    unsigned short opp;
                    if(op2.find('h')!=string::npos){
                        opp = hex2dec(op2);
                    }else{
                        if(op2[0]=='\'' && op2[2]=='\''){
                            opp = op2[1];
                        }
                        else opp = stoi(op2);
                    }
                    if(curs.compare("add") == 0) add_opr(realAddressFinder(op1),opp);
                    else if(curs.compare("sub") == 0) sub_opr(realAddressFinder(op1),opp);
                    else if(curs.compare("and") == 0) and_opr(realAddressFinder(op1),opp,temp);
                    else if(curs.compare("or") == 0) or_opr(realAddressFinder(op1),opp,temp);
                    else if(curs.compare("xor") == 0) xor_opr(realAddressFinder(op1),opp,temp);
                    else if(curs.compare("mov") == 0) mov_opr(realAddressFinder(op1),opp,temp);
                    else if(curs.compare("shr") == 0) shift_right_opr(realAddressFinder(op1),opp);
                    else if(curs.compare("shl") == 0) shift_left_opr(realAddressFinder(op1),opp);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(realAddressFinder(op1),opp);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(realAddressFinder(op1),opp);
                    else{
                        cerr <<"invalid syntax";
                        return 1;
                    }
                }
            }else if(op1type=='v'){
                pair<int,unsigned short> address;

                for (auto itr = vars.begin(); itr != vars.end(); itr++) {
                    if(itr->first==op1)
                        address=itr->second;
                }
                int type=address.first;
                int addressVal=address.second;
                if(type==99)
                    type=8;
                if(type==115)
                    type=16;
                address=make_pair(type,addressVal);

                if(op2type == 'r'){//<mem><reg> with var
                    reg* curr;
                    if(eightBitRegs.find(op2) != eightBitRegs.end()) curr = eightBitRegs[op2];
                    else curr = sixteenBitRegs[op2];
                    if(curs.compare("add") == 0) add_opr(address,curr);
                    else if(curs.compare("sub") == 0) sub_opr(address,curr);
                    else if(curs.compare("and") == 0) and_opr(address,curr);
                    else if(curs.compare("or") == 0) or_opr(address,curr);
                    else if(curs.compare("xor") == 0) xor_opr(address,curr);
                    else if(curs.compare("mov") == 0) mov_opr(address,curr);
                    else if(curs.compare("cmp") == 0) cmp_opr(address,curr);
                    else if(curs.compare("shr") == 0) shift_right_opr(address,curr);
                    else if(curs.compare("shl") == 0) shift_left_opr(address,curr);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(address,curr);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(address,curr);
                }
                else { //<mem><con> with var
                    char temp;
                    if(address.first==8) temp = 'c';
                    else temp = 's';
                    unsigned short opp;
                    if(op2.find('h')!=string::npos){
                        opp = hex2dec(op2);
                    }else{
                        if(op2[0]=='\'' && op2[2]=='\''){
                            opp = op2[1];
                        }
                        else opp = stoi(op2);
                    }
                    if(curs.compare("add") == 0) add_opr(address,opp);
                    else if(curs.compare("sub") == 0) sub_opr(address,opp);
                    else if(curs.compare("and") == 0) and_opr(address,opp,temp);
                    else if(curs.compare("or") == 0) or_opr(address,opp,temp);
                    else if(curs.compare("xor") == 0) xor_opr(address,opp,temp);
                    else if(curs.compare("mov") == 0) mov_opr(address,opp,temp);
                    else if(curs.compare("shr") == 0) shift_right_opr(address,opp);
                    else if(curs.compare("shl") == 0) shift_left_opr(address,opp);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(address,opp);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(address,opp);
                    else{
                        cerr <<"invalid syntax";
                        return 1;
                    }
                }
            }
            else{
                if(op2type == 'r') {//<reg><reg>
                    reg* curr;
                    if(eightBitRegs.find(op1) != eightBitRegs.end()) curr = eightBitRegs[op1];
                    else curr = sixteenBitRegs[op1];
                    reg* currt;
                    if(eightBitRegs.find(op2) != eightBitRegs.end()) currt = eightBitRegs[op2];
                    else currt = sixteenBitRegs[op2];
                    if(curs.compare("add") == 0) add_opr(curr,currt);
                    else if(curs.compare("sub") == 0) sub_opr(curr,currt);
                    else if(curs.compare("and") == 0) and_opr(curr,currt);
                    else if(curs.compare("or") == 0) or_opr(curr,currt);
                    else if(curs.compare("xor") == 0) xor_opr(curr,currt);
                    else if(curs.compare("mov") == 0) mov_opr(curr,currt);
                    else if(curs.compare("cmp") == 0) cmp_opr(curr,currt);
                    else if(curs.compare("shr") == 0) shift_right_opr(curr,currt);
                    else if(curs.compare("shl") == 0) shift_left_opr(curr,currt);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(curr,currt);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(curr,currt);
                }
                else  if(op2type == 'm') {//<reg><mem>
                    reg* curr;
                    if(eightBitRegs.find(op1) != eightBitRegs.end()) curr = eightBitRegs[op1];
                    else curr = sixteenBitRegs[op1];
                    if(curs.compare("add") == 0) add_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("sub") == 0) sub_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("and") == 0) and_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("or") == 0) or_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("xor") == 0) xor_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("mov") == 0) mov_opr(curr,realAddressFinder(op2));
                    else if(curs.compare("cmp") == 0) cmp_opr(curr,realAddressFinder(op2));
                }
                else if(op2type == 'v') {//<reg><mem> with var
                    reg* curr;
                    if(eightBitRegs.find(op1) != eightBitRegs.end()) curr = eightBitRegs[op1];
                    else curr = sixteenBitRegs[op1];
                    pair<int, unsigned short> address=vars[op2];
                    int type=address.first;
                    int addressVal=address.second;
                    if(type==99)
                        type=8;
                    if(type==115)
                        type=16;
                    address=make_pair(type,addressVal);

                    if (curs.compare("add") == 0) add_opr(curr, address);
                    else if (curs.compare("sub") == 0) sub_opr(curr, address);
                    else if (curs.compare("and") == 0) and_opr(curr,address);
                    else if (curs.compare("or") == 0) or_opr(curr, address);
                    else if (curs.compare("xor") == 0) xor_opr(curr, address);
                    else if (curs.compare("mov") == 0) mov_opr(curr, address);
                    else if (curs.compare("cmp") == 0) cmp_opr(curr,address);
                    else {
                        cout <<"ERROR";
                        exit(0);
                    }
                }
                else{//<reg><con>
                    reg* curr;
                    if(eightBitRegs.find(op1) != eightBitRegs.end()) curr = eightBitRegs[op1];
                    else curr = sixteenBitRegs[op1];
                    char temp;
                    if(curr->regType==8) temp = 'c';
                    else temp = 's';
                    unsigned short opp;
                    if(op2.find('h')!=string::npos){
                        opp = hex2dec(op2);
                    }else{
                        if(op2[0]=='\'' && op2[2]=='\''){
                            opp = op2[1];
                        }
                        else opp = stoi(op2);
                    }
                    if(curs.compare("add") == 0) add_opr(curr,opp);
                    else if(curs.compare("sub") == 0) sub_opr(curr,opp);
                    else if(curs.compare("and") == 0) and_opr(curr,opp,temp);
                    else if(curs.compare("or") == 0) or_opr(curr,opp,temp);
                    else if(curs.compare("xor") == 0) xor_opr(curr,opp,temp);
                    else if(curs.compare("mov") == 0) mov_opr(curr,opp,temp);
                    else if(curs.compare("cmp") == 0) cmp_opr(curr, opp,temp);
                    else if(curs.compare("shr") == 0) shift_right_opr(curr, opp);
                    else if(curs.compare("shl") == 0) shift_left_opr(curr, opp);
                    else if(curs.compare("rcr") == 0) rotate_right_opr(curr, opp);
                    else if(curs.compare("rcl") == 0) rotate_left_opr(curr, opp);
                    else{
                        cout <<"ERROR";
                        exit(0);
                    }
                }


            }

        }
        else if(curs.compare("inc")==0 || curs.compare("dec")==0){//can take many operands at once, calls each individually
            string temp;
            vector<string> operands;
            pch = strtok (str2, " ,");
            while(pch != NULL){
                temp = pch;
                operands.push_back(temp);
                pch = strtok (NULL, " ,");
            }
            for(int a=0; a<operands.size(); a++){
                char op1type=findOperandType(operands[a]);
                if(op1type == 'r'){
                    reg* curr;
                    if(eightBitRegs.find(operands[a]) != eightBitRegs.end()){
                        curr = eightBitRegs[operands[a]];
                    }else{
                        curr = sixteenBitRegs[operands[a]];
                    }
                    if(curs.compare("inc")==0){
                        inc_opr(curr);
                    }else{
                        dec_opr(curr);
                    }
                }else{
                    if(curs.compare("inc")==0){
                        inc_opr(realAddressFinder(operands[a]));
                    }else{
                        dec_opr(realAddressFinder(operands[a]));
                    }
                }
            }
        }
        else{
            cout<<"ERROR"<<endl;
            exit(0);
        }
    }

}

//----------------------------------------INSTRUCTIONS



//----------------------------------------MOV

//The mov instruction copies the data item referred to by its second operand (i.e. register contents, memory contents, or a constant value)
// into the location referred to by its first operand (i.e. a register or memory).
// While register-to-register moves are possible, direct memory-to-memory moves are not.
//Copies the second operand (source operand) to the first operand (destination operand).
// The source operand can be an immediate value, general-purpose register, segment register, or memory location
// the destination register can be a general-purpose register, segment register, or memory location.
// Both operands must be the same size, which can be a byte, a word, or a doubleword.
// No flags are affected

/*SYNTAX
mov <reg>,<reg>
mov <reg>,<mem>
mov <mem>,<reg>
mov <reg>,<const>
mov <mem>,<const>
 */

void mov_opr(reg* reg1, reg* reg2) { //mov <reg>,<reg>

    if (reg1->regType == reg2->regType) {
        if(reg1->name=="cs"){ //The MOV instruction cannot be used to load the CS register.
            throw "The MOV instruction cannot be used to load the CS register.";
            return;
        }
        if (reg1->regType == 16) { //both 16 bit registers
            unsigned short temp = reg2->sixteenValue;
            reg1->update_16(temp);
        }
        else { //both 8 bit registers
            unsigned char temp = reg2->eightValue;
            reg1->update_8(temp);
        }
    } else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void mov_opr(reg* reg1, pair<int, unsigned short> realAddress){ // mov <reg>,<mem>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (8 or 16)

    if(reg1->regType==type || (reg1->regType == 16 && type == 's') || (reg1->regType == 8 && type == 'c')){
        if(reg1->regType==16){ //both 16 bit
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = memVal;
            reg1->update_16(temp);
        }
        else{ // both 8 bit
            unsigned char memVal= memory[addressValue];
            unsigned char temp =memVal;
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void mov_opr(pair<int, unsigned short> realAddress, reg* reg1){ // mov <mem>, <reg>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type){
        if(reg1->regType==16){ //both 16 bit
            unsigned short temp = reg1->sixteenValue;
            memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
        }
        else{ // both 8 bit
            unsigned char temp = reg1->eightValue;
            memory[addressValue]= temp;
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void mov_opr(reg* reg1, unsigned constant, char constType){ // mov <reg>,<const>

    if(reg1->regType==16 && constType=='s'){ // both 16 bit
        unsigned short temp = ((unsigned short) constant);
        reg1->update_16(temp);
    }
    else if(reg1->regType==8 && constType=='c'){ // both 8 bit
        unsigned char temp =(unsigned char) constant;
        reg1->update_8(temp);
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void mov_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType){ // mov <mem>, <const>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(type==16 && constType=='s'){ // both 16 bit
        unsigned short temp = (unsigned short) constant;
        memory[addressValue+1]=temp>>8; //8 most significant bits
        memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else if(type==8 && constType=='c'){ // both 8 bit
        unsigned char temp = (unsigned char) constant;
        memory[addressValue]= temp;
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}


//----------------------------------------CMP OPERATION
// The CF, OF, SF, ZF, AF, and PF flags are set according to the result.
// The comparison is performed by subtracting the second operand from the first operand and then setting the status flags in the same manner as the SUB instruction.
// When an immediate value is used as an operand, it is sign-extended to the length of the first operand.
// cmp destination, source
// If dst=src zf=1 cf=0
// If dst<src zf=0 cf=1
// If dst>src zf=0 cf=0
// SF ≔ MostSignificantBit(difference), so a unset SF means the difference is non-negative
/*SYNTAX
cmp <reg>,<reg>
cmp <reg>,<mem>
cmp <mem>,<reg>
cmp <reg>,<con>
*/
void cmp_opr(reg* reg1, reg* reg2){ // cmp<reg><reg>
    if(reg1->regType==reg2->regType){
        cf=false;
        af=false;
        of=false;
        zf=false;
        sf=false;
        if(reg1->regType==16){ //both 16 bit registers
            unsigned short res;
            int rres= reg1->sixteenValue - reg2->sixteenValue;
            if(rres> 65535 || rres<-65535){//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if(rres<0){//result needs a borrow -> carry flag is set
                cf=true;
                res = rres + 65536;
            } else res = rres;
            if((reg1->low - ((reg1->low>>4)<<4 ) - reg2->low + ((reg2->low>>4)<<4)) < 0){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg1->sixteenValue>>15 != reg2 -> sixteenValue>>15) && reg1->sixteenValue != res>>15){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){ //zf is set if the result is zero
                zf=true;
            }
            if(res>>15 == 1){//most significant bit is 1 if number is negative according to sign convention, sf is set
                sf=true;
            }
        }else{//both 8 bit registers
            unsigned char res;
            short rres= reg1->eightValue - reg2 ->eightValue;
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            } else res = rres;

            if((reg1->eightValue - ((reg1->eightValue>>4)<<4 ) - reg2->eightValue + ((reg2->eightValue>>4)<<4)) < 0 ){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg1->eightValue>>7 != reg2 -> eightValue>>7) && reg1->eightValue != res>>7){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){
                zf=true;
            }
            if(res>>15 == 1){//negative according to sign convention
                sf=true;
            }
        }
    }else{ //different registers
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void cmp_opr(reg* reg, pair<int,unsigned short> mem){ //cmp <reg><mem>
    if(mem.first== 16 || mem.first == 's'){
        if(reg->regType==8){//invalid operation
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//both 16 bit
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned short res;
            unsigned short memval = (memory[mem.second+1]<<8) + memory[mem.second];
            int rres= reg->sixteenValue - memval;
            if(rres> 65535 || rres<-65535){//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if(rres<0){//result needs a borrow -> carry flag is set
                cf=true;
                res = rres + 65536;
            } else res = rres;
            if(reg->low - ((reg->low>>4)<<4 ) - memory[mem.second+1] + ((memory[mem.second+1]>>4)<<4) < 0){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg->sixteenValue>>15 != memval>>15) && reg->sixteenValue != res>>15){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){ //zf is set if the result is zero
                zf=true;
            }
            if(res>>15 == 1){//most significant bit is 1 if number is negative according to sign convention, sf is set
                sf=true;
            }
        }
    }else{
        if(reg->regType==16) {
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//both 8 bit registers
            unsigned char res;
            short rres= reg->eightValue + memory[mem.second];
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            }else res = rres;
            if(reg->eightValue - ((reg->eightValue>>4)<<4 ) - memory[mem.second+1] + ((memory[mem.second+1]>>4)<<4) < 0 ){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg->eightValue>>7 != memory[mem.second]>>7) && reg->eightValue != res>>7){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){
                zf=true;
            }
            if(res>>15 == 1){//negative according to sign convention
                sf=true;
            }
        }
    }
}

void cmp_opr(pair<int,unsigned short>mem, reg* reg){ //cmp<mem><reg>
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(mem.first==16){
        if(reg->regType==8){//invalid operation
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else {//both 16 bit
            unsigned short res;
            unsigned short memval = (memory[mem.second+1] << 8) + memory[mem.second];
            int rres = memval - reg->sixteenValue;
            if (rres > 65535 || rres < -65535) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 65536;
            } else res = rres;
            if (memory[mem.second + 1] - ((memory[mem.second + 1] >> 4) << 4) - reg->low + ((reg->low >> 4) << 4) < 0) {
                //af is set when there is carry from the least significant 4 bits
                af = true;
            }
            if ((memval >> 15 != reg->sixteenValue >> 15) && memval >> 15 != res >> 15) {
                //overflow has occurred if two registers have different, and the result's sign is different from first.
                of = true;
            }
            if (res == 0) { //zf is set when the result is zero
                zf = true;
            }
            if (res >> 15 ==1) {//sf is set if the most significant bit is 1, indicating negative sign on signed numbers
                sf = true;
            }
        }
    }else {
        if(reg->regType==16) {
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else {//both 8 bit registers
            unsigned short res;
            int rres = memory[mem.second] - reg->eightValue;
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            } else res = rres;
            if (memory[mem.second] - ((memory[mem.second] >> 4) << 4) - reg->eightValue + ((reg->eightValue >> 4) << 4) <
                0) {
                //af is set when there is borrow from the least significant 4 bits
                af = true;
            }
            if (memory[mem.second] >> 7 != reg->eightValue >> 7 && memory[mem.second] >> 7 != res >> 7) {
                //overflow has occurred if two registers have different sign, and the result's sign is different from first.
                of = true;
            }
            if (res == 0) { //zf is set when the result is zero
                zf = true;
            }
            if (res >> 7 ==
                1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf = true;
            }
        }
    }

}

void cmp_opr(reg* reg, unsigned short con, char constType){ //cmp<reg>,<con>
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(reg->regType==16){
        if(con <= 255 && (con>>7)==1){//constants are sign-extended to the length of the destination.
            con+= (255<<8);
        } //todo bu mu geçerli error mu
        unsigned short res;
        int rres = reg->sixteenValue - con;
        if(rres> 65535 || rres<-65535){//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if(rres<0){//result needs a borrow -> carry flag is set
            cf=true;
            res = rres + 65536;
        } else res = rres;

        if(reg->low - ((reg->low>>4)<<4)- con + ((con>>4)<<4) < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((reg->sixteenValue>>15 != con>>15) && reg->sixteenValue>>15 != res>>15){
            //overflow has occurred if two registers have different, and the result's sign is different from first.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
    }else {
        unsigned short res;
        int rres = reg->eightValue - con;
        if (rres > 256 || rres < -256) {//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if (rres < 0) {//result needs a borrow -> carry flag is set
            cf = true;
            res = rres + 256;
        } else res = rres;

        if (reg->eightValue - ((reg->eightValue >> 4) << 4) - con + ((con >> 4) << 4) < 0) {
            //af is set when there is borrow from the least significant 4 bits
            af = true;
        }
        if (reg->eightValue >> 7 != con >> 7 && reg->eightValue >> 7 != res >> 7) {
            //overflow has occurred if two registers have different sign, and the result's sign is different from first.
            of = true;
        }
        if (res == 0) { //zf is set when the result is zero
            zf = true;
        }
        if (res >> 7 == 1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf = true;
        }
    }
}

void cmp_opr(pair<int,unsigned short>mem, unsigned short con){ //todo delete maybe
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(mem.first==16){
        if(con <= 255 && (con>>7)==1){//constants are sign-extended to the length of the destination.
            con+= (255<<8);
        }//todo sign extend vs error
        unsigned short res;
        unsigned short memval = (memory[mem.second+1]<<8) + memory[mem.second];
        int rres = memval - con;
        if(rres> 65535 || rres<-65535){//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if(rres<0){//result needs a borrow -> carry flag is set
            cf=true;
            res = rres + 65536;
        } else res = rres;

        if(memory[mem.second+1] - ((memory[mem.second+1]>>4)<<4)- con + ((con>>4)<<4) < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((memval>>15 != con>>15) && memval>>15 != res>>15){
            //overflow has occurred if two registers have different, and the result's sign is different from first.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
    }else {
        unsigned short res;
        int rres = memory[mem.second] - con;
        if (rres > 256 || rres < -256) {//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if (rres < 0) {//result needs a borrow -> carry flag is set
            cf = true;
            res = rres + 256;
        } else res = rres;

        if (memory[mem.second] - ((memory[mem.second] >> 4) << 4) - con + ((con >> 4) << 4) < 0) {
            //af is set when there is borrow from the least significant 4 bits
            af = true;
        }
        if (memory[mem.second] >> 7 != con >> 7 && memory[mem.second] >> 7 != res >> 7) {
            //overflow has occurred if two registers have different sign, and the result's sign is different from first.
            of = true;
        }
        if (res == 0) { //zf is set when the result is zero
            zf = true;
        }
        if (res >> 7 == 1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf = true;
        }
    }
}


//----------------------------------------STACK OPERATIONS
//No flags affected
//SP points to the  top free location on the stack.
//You can only push words (i.e. 2 bytes)  on to the stack.

//The push instruction places its operand onto the top of the hardware supported stack in memory.
// Specifically, push first decrements ESP by 4, then places its operand into the contents of the 32-bit location at address [ESP].
// ESP (the stack pointer) is decremented by push since the x86 stack grows down - i.e. the stack grows from high addresses to lower addresses.

/*SYNTAX
 * push <reg>
 * push <mem>
 * push <con>
 */
void push_opr(reg* reg){  // push <reg>
    if(reg->regType==16){   //pushing 16 bit value
        sp.update_16(sp.sixteenValue-2); //take sp 2 steps down
        unsigned short stackAddress=sp.sixteenValue;
        unsigned short temp= reg->sixteenValue; //value to push
        memory[stackAddress+2]=temp>>8; //8 most significant bits
        memory[stackAddress+1]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else{ //pushing 8 bit value
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //You can only push words on to the stack
    }
}

void push_opr(pair<int, unsigned short> realAddress){ //push <mem>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    if(type==16 || type =='s'){  //pushing 16 bit value
        sp.update_16(sp.sixteenValue-2);  //take sp 2 steps down
        unsigned short stackAddress=sp.sixteenValue;
        unsigned short temp= concat(memory[addressValue],memory[addressValue+1]); //value to push
        memory[stackAddress+2]=temp>>8; //8 most significant bits
        memory[stackAddress+1]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else{//pushing 8 bit value
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //You can only push words on to the stack

    }
}

void push_opr(unsigned constant,char constType){ //push <con>

    if(constType=='s'){  //pushing 16 bit value
        sp.update_16(sp.sixteenValue-2); //take sp 2 steps down
        unsigned short stackAddress=sp.sixteenValue;
        unsigned short temp= (unsigned short) constant; //value to push
        memory[stackAddress+2]=temp>>8; //8 most significant bits
        memory[stackAddress+1]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else{//pushing 8 bit value
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //You can only push words on to the stack
    }
}


//The pop instruction removes the 4-byte data element from the top of the hardware-supported stack into the specified operand (i.e. register or memory location).
//It first moves the 4 bytes located at memory location [SP] into the specified register or memory location, and then increments SP by 4.
/*SYNTAX
pop <reg>
pop <mem>
*/

void pop_opr(reg* reg){  // pop <reg>
    if(sp.sixteenValue==(2<<15)-2){
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //Empty Stack
    }
    if(reg->regType==16){ //popping 16 bit value
        unsigned short stackAddress=sp.sixteenValue;
        unsigned short temp= concat(memory[stackAddress+1], memory[stackAddress+2]); //value to pop
        reg->update_16(temp);
        sp.update_16(sp.sixteenValue+2); // take sp 2 steps up

    }
    else{ //popping 8 bit value
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //You can only push words on to the stack

    }
}

void pop_opr(pair<int, unsigned short> realAddress){ //pop <mem>
    if(sp.sixteenValue==(2<<15)-2){
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //Empty Stack
    }
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    if(type==16){ //popping 16 bit value
        unsigned short stackAddress=sp.sixteenValue;
        unsigned short temp= concat(memory[stackAddress+1], memory[stackAddress+2]); //value to pop
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
        sp.update_16(sp.sixteenValue+2);  // take sp 2 steps up
    }
    else{//popping 8 bit value
        cout << "ERROR"<< endl;
        exit(0);
        return;
        //You can only push words on to the stack

    }
}

//----------------------------------------MATH OPERATIONS


//------------------------------ARITHMETIC ADD
/* ADD arithmetic addition
FLAGS: AF, CF, OF, ZF SF
add <reg>,<reg>
add <reg>,<mem>
add <mem>,<reg>
add <reg>,<con>
add <mem>,<con>
*/
void add_opr(reg* reg1, reg* reg2){ // add<reg><reg>
    if(reg1->regType==reg2->regType){
        cf=false;
        af=false;
        of=false;
        zf=false;
        sf=false;
        if(reg1->regType==16){ //both are 16 bit registers
            unsigned short res;
            unsigned int rres = reg1->sixteenValue + reg2->sixteenValue;
            if(rres>>16== 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = rres - 65536;
            } else res = rres;
            if((reg1->low - ((reg1->low>>4)<<4 ) + reg2->low - ((reg2->low>>4)<<4)) > 15){
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg1->sixteenValue >>15 == reg2 -> sixteenValue>>15) && reg1->sixteenValue>>15 != res>>15){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){ //zf is set when the result is zero
                zf=true;
            }
            if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            reg1->update_16(res);
        }else{//both 8 bit registers
            unsigned char res;
            unsigned short rres= reg1->eightValue + reg2 ->eightValue;
            if((rres>>8) == 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = (rres - 256);
            } else res = rres;
            if((reg1->eightValue - ((reg1->eightValue>>4)<<4 ) + reg2->eightValue - ((reg2->eightValue>>4)<<4)) > 15){
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg1->eightValue>>7 == reg2 -> eightValue>>7) && reg1->eightValue>>7 != res>>7){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){//zf is set when the result is zero
                zf=true;
            }
            if(res>>7 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            reg1->update_8(res);
        }
    }else{ //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void add_opr(reg* reg, pair<int,unsigned short> address){ //add <reg><mem>
    if(address.first == 16 || address.first == 's'){
        if(reg->regType == 8){
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//16 bits
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned short res;
            unsigned short memval = (memory[address.second+1]<<8) + memory[address.second];
            unsigned int rres = reg->sixteenValue + memval;
            if(rres>>16 == 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = rres - 65536;
            } else res = rres;
            if(reg->low - ((reg->low>>4)<<4 ) + memory[address.second+1] - ((memory[address.second+1]>>4)<<4) > 15){ //15 is 1111 binary
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg->sixteenValue>>15 == memval>>15) && reg->sixteenValue>>15 != res>>15){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){ //zf is set when the result is zero
                zf=true;
            }
            if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            reg->update_16(res);
        }
    }else{
        if(reg->regType == 16){
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//8bits
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned char res;
            unsigned short rres= reg->eightValue + memory[address.second];
            if(rres>>8 == 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = (rres - 256);
            } else res = rres;
            if(reg->eightValue - ((reg->eightValue>>4)<<4 ) + memory[address.second] - ((memory[address.second]>>4)<<4) > 15){
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg->eightValue>>7 == memory[address.second]>>7) && reg->eightValue>>7 != res>>7){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){//zf is set when the result is zero
                zf=true;
            }
            if(res>>7 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            reg->update_8(res);
        }
    }
}

void add_opr(pair<int,unsigned short> address,reg* reg) { //add <mem><reg>
    if(address.first == 16){
        if(reg->regType == 8){
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//16 bits
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned short res;
            unsigned short memval = (memory[address.second+1]<<8 )+ memory[address.second];
            unsigned int rres = reg->sixteenValue + memval;
            if(rres>>16 == 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = rres - 65536;
            } else res = rres;
            if(reg->low - ((reg->low>>4)<<4 ) + memory[address.second+1] - ((memory[address.second+1]>>4)<<4) > 15){ //15 is 1111
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg->sixteenValue>>15 == memval>>15) && reg->sixteenValue>>15 != res>>15){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){ //zf is set when the result is zero
                zf=true;
            }
            if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            unsigned char hi = res>>8;
            unsigned char lo = res - (hi<<8);
            memory[address.second+1] = hi;
            memory[address.second] = lo;
        }
    }else{
        if(reg->regType == 16){
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//8bits
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned char res;
            unsigned short rres= reg->eightValue + memory[address.second];
            if(rres>>8 == 1){
                //result requires one more byte -> carry flag is set
                cf=true;
                res = (rres - 256);
            } else res = rres;
            if(reg->eightValue - ((reg->eightValue>>4)<<4 ) + memory[address.second] - ((memory[address.second]>>4)<<4) > 15){
                //af is set when there is carry from the least significant 4 bits
                af=true;
            }
            if((reg->eightValue>>7 == memory[address.second]>>7) && reg->eightValue>>7 != res>>7){
                //overflow has occurred if two registers have the same sign, and the result's sign is different.
                of=true;
            }
            if(res == 0){//zf is set when the result is zero
                zf=true;
            }
            if(res>>7 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf=true;
            }
            memory[address.second] = res;
        }
    }
}

void add_opr(reg* reg, unsigned short constant){ //add <reg><con>
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(reg->regType==16){
        if(constant <= 255 && (constant>>7)==1){//constants are sign-extended to the length of the destination.
            constant+= (255<<8);
        }
        unsigned short res;
        unsigned int rres = reg->sixteenValue + constant;
        if(rres>>16 == 1){
            //result requires one more byte -> carry flag is set
            cf=true;
            res = rres - 65536;
        } else res = rres;
        if(reg->low - ((reg->low>>4)<<4 ) + constant - ((constant>>4)<<4 )> 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((reg->sixteenValue>>15 == constant>>15) && reg->sixteenValue>>15 != res>>15){
            //overflow has occurred if two registers have the same sign, and the result's sign is different.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        reg->update_16(res);
    }else{
        unsigned short res;
        unsigned int rres = reg->eightValue + constant;
        if(rres>>8 == 1){
            //result requires one more byte -> carry flag is set
            cf=true;
            res = rres - 256;
        } else res = rres;
        if(reg->eightValue - ((reg->eightValue>>4)<<4 ) + constant - ((constant>>4)<<4) > 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((reg->eightValue>>7 == constant>>7) && reg->eightValue>>7 != res>>7){
            //overflow has occurred if two registers have the same sign, and the result's sign is different.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>7 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        reg->update_8(res);
    }
}

void add_opr(pair<int,unsigned short> address, unsigned short constant){ //add<mem><con>
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(address.first==16){
        if(constant <= 255 && (constant>>7)==1){//constants are sign-extended to the length of the destination.
            constant+= (255<<8);
        }//todo bu mu geçerli error mu?
        unsigned short res;
        unsigned short memval = (memory[address.second+1]<<8 )+ memory[address.second];
        unsigned int rres = memval + constant;
        if(rres>>16 == 1){
            //result requires one more byte -> carry flag is set
            cf=true;
            res = rres - 65536;
        } else res = rres;
        if(memory[address.second+1] - ((memory[address.second+1]>>4)<<4)+ constant - ((constant>>4)<<4 )> 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((memval>>15 == constant>>15) && memval>>15 != res>>15){
            //overflow has occurred if two registers have the same sign, and the result's sign is different.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        unsigned char hi = res>>8;
        unsigned char lo = res - (hi<<8);
        memory[address.second] = lo;
        memory[address.second+1] = hi;
    }else{
        unsigned short res;
        unsigned int rres = memory[address.second] + constant;
        if(rres>>8 == 1){
            //result requires one more byte -> carry flag is set
            cf=true;
            res = rres - 256;
        } else res = rres;
        if(memory[address.second] - ((memory[address.second]>>4)<<4) + constant - ((constant>>4)<<4 )> 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if(memory[address.second]>>7 == constant>>7 && constant>>7 != res>>7){
            //overflow has occurred if two registers have the same sign, and the result's sign is different.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>7 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        memory[address.second]=res;
    }
}



//------------------------------ARITHMETIC SUB

/* SUB
FLAGS: AF CF OF ZF SF
sub <reg>,<reg>
sub <reg>,<mem>
sub <mem>,<reg>
sub <reg>,<con>
sub <mem>,<con>
*/
void sub_opr(reg* reg1, reg* reg2){ // sub<reg><reg>
    if(reg1->regType==reg2->regType){
        cf=false;
        af=false;
        of=false;
        zf=false;
        sf=false;
        if(reg1->regType==16){ //both 16 bit registers
            unsigned short res;
            int rres= reg1->sixteenValue - reg2->sixteenValue;
            if(rres> 65535 || rres<-65535){//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if(rres<0){//result needs a borrow -> carry flag is set
                cf=true;
                res = rres + 65536;
            } else res = rres;
            if((reg1->low - ((reg1->low>>4)<<4 ) - reg2->low + ((reg2->low>>4)<<4)) < 0){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg1->sixteenValue>>15 != reg2 -> sixteenValue>>15) && reg1->sixteenValue != res>>15){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){ //zf is set if the result is zero
                zf=true;
            }
            if(res>>15 == 1){//most significant bit is 1 if number is negative according to sign convention, sf is set
                sf=true;
            }
            reg1->update_16(res);
        }else{//both 8 bit registers
            unsigned char res;
            short rres= reg1->eightValue - reg2 ->eightValue;
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            } else res = rres;

            if((reg1->eightValue - ((reg1->eightValue>>4)<<4 ) - reg2->eightValue + ((reg2->eightValue>>4)<<4)) < 0 ){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg1->eightValue>>7 != reg2 -> eightValue>>7) && reg1->eightValue != res>>7){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){
                zf=true;
            }
            if(res>>15 == 1){//negative according to sign convention
                sf=true;
            }
            reg1->update_8(res);
        }
    }else{ //different registers
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void sub_opr(reg* reg, pair<int,unsigned short> mem){ //sub <reg><mem>
    if(mem.first== 16 || mem.first == 's'){
        if(reg->regType==8){//invalid operation
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//both 16 bit
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            unsigned short res;
            unsigned short memval = (memory[mem.second+1]<<8) + memory[mem.second];
            int rres= reg->sixteenValue - memval;
            if(rres> 65535 || rres<-65535){//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if(rres<0){//result needs a borrow -> carry flag is set
                cf=true;
                res = rres + 65536;
            } else res = rres;
            if(reg->low - ((reg->low>>4)<<4 ) - memory[mem.second+1] + ((memory[mem.second+1]>>4)<<4) < 0){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg->sixteenValue>>15 != memval>>15) && reg->sixteenValue != res>>15){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){ //zf is set if the result is zero
                zf=true;
            }
            if(res>>15 == 1){//most significant bit is 1 if number is negative according to sign convention, sf is set
                sf=true;
            }
            reg->update_16(res);
        }
    }else{
        if(reg->regType==16) {
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else{//both 8 bit registers
            unsigned char res;
            short rres= reg->eightValue + memory[mem.second];
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            cf=false;
            af=false;
            of=false;
            zf=false;
            sf=false;
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            }else res = rres;
            if(reg->eightValue - ((reg->eightValue>>4)<<4 ) - memory[mem.second+1] + ((memory[mem.second+1]>>4)<<4) < 0 ){
                //af is set when there is borrow from the least significant 4 bits
                af=true;
            }
            if((reg->eightValue>>7 != memory[mem.second]>>7) && reg->eightValue != res>>7){
                //overflow occurs if two registers have different sign, and the result's sign is different from register 1
                of=true;
            }
            if(res == 0){
                zf=true;
            }
            if(res>>15 == 1){//negative according to sign convention
                sf=true;
            }
            reg->update_8(res);
        }
    }
}

void sub_opr(pair<int,unsigned short>mem, reg* reg){ //sub<mem><reg>
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(mem.first==16){
        if(reg->regType==8){//invalid operation
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else {//both 16 bit
            unsigned short res;
            unsigned short memval = (memory[mem.second+1] << 8) + memory[mem.second];
            int rres = memval - reg->sixteenValue;
            if (rres > 65535 || rres < -65535) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 65536;
            } else res = rres;
            if (memory[mem.second + 1] - ((memory[mem.second + 1] >> 4) << 4) - reg->low + ((reg->low >> 4) << 4) < 0) {
                //af is set when there is carry from the least significant 4 bits
                af = true;
            }
            if ((memval >> 15 != reg->sixteenValue >> 15) && memval >> 15 != res >> 15) {
                //overflow has occurred if two registers have different, and the result's sign is different from first.
                of = true;
            }
            if (res == 0) { //zf is set when the result is zero
                zf = true;
            }
            if (res >> 15 ==1) {//sf is set if the most significant bit is 1, indicating negative sign on signed numbers
                sf = true;
            }
            unsigned char hi = res >> 8;
            unsigned char lo = res - (hi << 8);
            memory[mem.second+1] = hi;
            memory[mem.second ] = lo;
        }
    }else {
        if(reg->regType==16) {
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }else {//both 8 bit registers
            unsigned short res;
            int rres = memory[mem.second] - reg->eightValue;
            if (rres > 256 || rres < -256) {//invalid result
                cout << "ERROR"<< endl;
                exit(0);
                return;
            }
            if (rres < 0) {//result needs a borrow -> carry flag is set
                cf = true;
                res = rres + 256;
            } else res = rres;
            if (memory[mem.second] - ((memory[mem.second] >> 4) << 4) - reg->eightValue + ((reg->eightValue >> 4) << 4) <
                0) {
                //af is set when there is borrow from the least significant 4 bits
                af = true;
            }
            if (memory[mem.second] >> 7 != reg->eightValue >> 7 && memory[mem.second] >> 7 != res >> 7) {
                //overflow has occurred if two registers have different sign, and the result's sign is different from first.
                of = true;
            }
            if (res == 0) { //zf is set when the result is zero
                zf = true;
            }
            if (res >> 7 ==
                1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
                sf = true;
            }
            memory[mem.second] = res;
        }
    }

}

void sub_opr(reg* reg, unsigned short con){
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(reg->regType==16){
        if(con <= 255 && (con>>7)==1){//constants are sign-extended to the length of the destination.
            con+= (255<<8);
        } //todo bu mu geçerli error mu
        unsigned short res;
        int rres = reg->sixteenValue - con;
        if(rres> 65535 || rres<-65535){//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if(rres<0){//result needs a borrow -> carry flag is set
            cf=true;
            res = rres + 65536;
        } else res = rres;

        if(reg->low - ((reg->low>>4)<<4)- con + ((con>>4)<<4) < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((reg->sixteenValue>>15 != con>>15) && reg->sixteenValue>>15 != res>>15){
            //overflow has occurred if two registers have different, and the result's sign is different from first.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        reg->update_16(res);
    }else {
        unsigned short res;
        int rres = reg->eightValue - con;
        if (rres > 256 || rres < -256) {//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if (rres < 0) {//result needs a borrow -> carry flag is set
            cf = true;
            res = rres + 256;
        } else res = rres;

        if (reg->eightValue - ((reg->eightValue >> 4) << 4) - con + ((con >> 4) << 4) < 0) {
            //af is set when there is borrow from the least significant 4 bits
            af = true;
        }
        if (reg->eightValue >> 7 != con >> 7 && reg->eightValue >> 7 != res >> 7) {
            //overflow has occurred if two registers have different sign, and the result's sign is different from first.
            of = true;
        }
        if (res == 0) { //zf is set when the result is zero
            zf = true;
        }
        if (res >> 7 == 1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf = true;
        }
        reg->update_8(res);
    }
}

void sub_opr(pair<int,unsigned short>mem, unsigned short con){
    cf=false;
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(mem.first==16){
        if(con <= 255 && (con>>7)==1){//constants are sign-extended to the length of the destination.
            con+= (255<<8);
        }//todo sign extend vs error
        unsigned short res;
        unsigned short memval = (memory[mem.second+1]<<8) + memory[mem.second];
        int rres = memval - con;
        if(rres> 65535 || rres<-65535){//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if(rres<0){//result needs a borrow -> carry flag is set
            cf=true;
            res = rres + 65536;
        } else res = rres;

        if(memory[mem.second+1] - ((memory[mem.second+1]>>4)<<4)- con + ((con>>4)<<4) < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if((memval>>15 != con>>15) && memval>>15 != res>>15){
            //overflow has occurred if two registers have different, and the result's sign is different from first.
            of=true;
        }
        if(res == 0){ //zf is set when the result is zero
            zf=true;
        }
        if(res>>15 == 1){//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf=true;
        }
        unsigned char hi = res>>8;
        unsigned char lo = res - (hi<<8);
        memory[mem.second+1] = hi;
        memory[mem.second] = lo;
    }else {
        unsigned short res;
        int rres = memory[mem.second] - con;
        if (rres > 256 || rres < -256) {//invalid result
            cout << "ERROR"<< endl;
            exit(0);
            return;
        }
        if (rres < 0) {//result needs a borrow -> carry flag is set
            cf = true;
            res = rres + 256;
        } else res = rres;

        if (memory[mem.second] - ((memory[mem.second] >> 4) << 4) - con + ((con >> 4) << 4) < 0) {
            //af is set when there is borrow from the least significant 4 bits
            af = true;
        }
        if (memory[mem.second] >> 7 != con >> 7 && memory[mem.second] >> 7 != res >> 7) {
            //overflow has occurred if two registers have different sign, and the result's sign is different from first.
            of = true;
        }
        if (res == 0) { //zf is set when the result is zero
            zf = true;
        }
        if (res >> 7 == 1) {//sf is set when the most significant bit is 1, indicating negative sign for signed numbers
            sf = true;
        }
        memory[mem.second] = res;
    }
}



//------------------------------ARITHMETIC INC DEC

/*INC/DEC increments or decrements the operand's contents by 1
FLAGS: AF OF ZF SF
inc <reg>
inc <mem>
dec <reg>
dec <mem>
 */
void inc_opr(reg* reg){
    af=false;
    of=false;
    zf=false;
    sf=false;    if(reg->regType==16){
        unsigned short res;
        unsigned int rres = reg-> sixteenValue +1;
        if(rres>>16 == 1){
            of = true;
            res = rres - 65536;
        } else res = rres;
        if(res==0) zf = true;
        if(res>>15 == 1) sf = true;
        if(reg->low - ((reg->low>>4)<<4 ) + 1 > 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        reg->update_16(res);
    }else{
        unsigned char res;
        unsigned short rres = reg-> eightValue +1;
        if(rres>>8 == 1){
            of = true;
            res = rres - 256;
        } else res = rres;
        if(res==0) zf = true;
        if(res>>7 == 1) sf = true;
        if((reg->eightValue - ((reg->eightValue>>4)<<4 ) + 1 > 15)){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        reg->update_8(res);
    }
}

void inc_opr(pair<int,unsigned short> address){
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(address.first==16 || address.first == 's'){
        unsigned short memval;
        unsigned int rres = (memory[address.second+1]<<8 )+ memory[address.second] +1;//this is required in case there is carry from the lower 8bits
        if(rres>>16 == 1){
            of = true;
            memval = rres - 65536;
        } else memval = rres;
        if(memval==0) zf = true;
        if(memval>>7 == 1) sf = true;
        if(memory[address.second+1] - ((memory[address.second+1]>>4)<<4)+ 1> 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        unsigned char hi = memval>>8;
        unsigned char lo = memval - (hi<<8);
        memory[address.second+1] = hi;
        memory[address.second] = lo;
    }else{
        unsigned char memval;
        unsigned short rres  = memory[address.second]+1;
        if(rres>>8 == 1){
            of = true;
            memval = rres - 256 ;
        } else memval = rres;
        if(memval==0) zf = true;
        if(memval>>7 == 1) sf = true;
        if(memory[address.second] - ((memory[address.second]>>4)<<4) +1> 15){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        memory[address.second] = memval;
    }
}

void dec_opr(reg* reg){
    af=false;
    of=false;
    zf=false;
    sf=false;    if(reg->regType==16){
        unsigned short res;
        int rres = reg-> sixteenValue - 1;
        if(rres< 0 ){
            of = true;
            res = rres + 65536;
        } else res = rres;
        if(res==0) zf = true;
        if(res>>15 == 1) sf = true;
        if(reg->low - ((reg->low>>4)<<4 ) - 1 < 0){
            //af is set when there is borrow from the least significant 4 bits
            af=true;
        }
        reg->update_16(res);
    }else{
        unsigned char res;
        unsigned short rres = reg-> eightValue - 1;
        if(rres < 0){
            of = true;
            res = rres + 256;
        } else res = rres;
        if(res==0) zf = true;
        if(res>>7 == 1) sf = true;
        if(reg->eightValue - ((reg->eightValue>>4)<<4 ) - 1 < 0 ){
            //af is set when there is borrow from the least significant 4 bits
            af=true;
        }
        reg->update_8(res);
    }
}

void dec_opr(pair<int,unsigned short> address){
    af=false;
    of=false;
    zf=false;
    sf=false;
    if(address.first==16 || address.first == 's'){
        unsigned short memval;
        unsigned int rres = (memory[address.second+1]<<8 )+ memory[address.second]-1;//this is required in case there is borrow from the higher 8bits
        if(rres< 0 ){
            of = true;
            memval = rres + 65536;
        } else memval = rres;
        if(memory[address.second+1] - ((memory[address.second+1]>>4)<<4)- 1 < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        if(memval==0) zf = true;
        if(memval>>15 == 1) sf = true;
        unsigned char hi = memval>>8;
        unsigned char lo = memval - (hi<<8);
        memory[address.second+1] = hi;
        memory[address.second] = lo;
    }else{
        unsigned char memval;
        short rres  = memory[address.second]-1;
        if(rres<0){
            of = true;
            memval = rres + 256 ;
        } else memval = rres;
        if(memval==0) zf = true;
        if(memval>>7 == 1) sf = true;
        if(memory[address.second] - ((memory[address.second]>>4)<<4)- 1 < 0){
            //af is set when there is carry from the least significant 4 bits
            af=true;
        }
        memory[address.second] = memval;
    }
}


//------------------------------ARITHMETIC MUL

/* MUL arithmetic multiplication
 * FLAGS: CF OF
 * mul<mem>
 * mul<reg>
 */

void mul_opr(reg* source){ //mul<reg>
    if(source->regType==8){
        unsigned short res = al.eightValue * source->eightValue;
        if(res>>8 >0){
            cf = true;
            of = true;
        } else{
            cf = false;
            of = false;
        }
        pax->update_16(res);
    }
    else{
        unsigned int res = ax.sixteenValue * source->sixteenValue;
        if(res>>16 >0){
            cf = true;
            of = true;
        }else{
            cf = false;
            of = false;
        }
        unsigned aa=res>>16;
        unsigned bb=res - ((res>>16)<<16);
        pdx-> update_16(res>>16);
        pax->update_16(res - ((res>>16)<<16));
    }

}

void mul_opr(pair<int,unsigned short>address){//mul<mem>
    if(address.first==8 || address.first == 'c'){
        unsigned short res = al.eightValue * memory[address.second];
        if(res>>8 >0){
            cf = true;
            of = true;
        }else{
            cf = false;
            of = false;
        }
        pax-> update_16(res);
    }
    else{
        unsigned short memval = (memory[address.second+1]<<8 )+ memory[address.second];
        unsigned int res = ax.sixteenValue * memval;
        if(res>>16 >0){
            cf = true;
            of = true;
        }else{
            cf = false;
            of = false;
        }
        pdx->update_16(res>>16);
        pax->update_16(res - ((res>>16)<<16));
    }
}

//------------------------------ARITHMETIC DIV


/*DIV arithmetic division
 * div<reg>
 * div<mem>
 */
void div_opr(reg* operand){
    if(operand->regType==8){
        if(operand->eightValue==0){ //division by zero
            cout <<"ERROR ";
            exit(0);
            return;
        }
        unsigned short quot = ax.sixteenValue / operand->eightValue;
        unsigned short rem = ax.sixteenValue % operand->eightValue;
        if(quot > 256){
            cout <<"ERROR ";
            exit(0);
            return;
        }
        pal->update_8(quot);
        pah->update_8(rem);
    }else{
        if(operand->sixteenValue==0){ //division by zero
            cout <<"ERROR ";
            exit(0);
            return;
        }
        unsigned int dividend = (dx.sixteenValue<<16) + ax.sixteenValue;
        unsigned short quot = dividend / (operand->sixteenValue);
        unsigned short rem = dividend % (operand-> sixteenValue);
        if(dividend/operand->sixteenValue > 65536){
            cout <<"ERROR ";
            exit(0);
            return;
        }
        pax->update_16(quot);
        pdx->update_16(rem);
    }
}

void div_opr(pair<int,unsigned short>address){
    if(address.first == 8 || address.first == 'c'){
        if(memory[address.second]==0){ //division by zero
            cout <<"ERROR ";
            exit(0);
            return;
        }
        unsigned short quot = ax.sixteenValue / memory[address.second];
        unsigned short rem = ax.sixteenValue % memory[address.second];
        if( quot > 256){
            cout <<"ERROR ";
            exit(0);
            return;
        }
        pal->update_8(quot);
        pah->update_8(rem);
    }else{
        unsigned short memval = (memory[address.second+1]<<8) + memory[address.second];
        if(memval==0){ //division by zero
            cout <<"ERROR ";
            exit(0);
            return;
        }
        unsigned int dividend = (dx.sixteenValue<<16) + ax.sixteenValue;
        if(dividend/memval > 65536){
            cout <<"ERROR ";
            exit(0);
            return;
        }
        unsigned short quot = dividend / memval;
        unsigned short rem = dividend % memval;
        pax->update_16(quot);
        pdx->update_16(rem);
    }
}

//----------------------------------------LOGICAL OPERATIONS


//------------------------------LOGICAL AND

//The output of bitwise AND is 1 if the corresponding bits of two operands is 1.
//If either bit of an operand is 0, the result of corresponding bit is evaluated to 0.
//The source operand can be an immediate, a register, or a memory location; the destination operand can be a register or a memory location.
//The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
/* SYNTAX
and <reg>,<reg>
and <reg>,<mem>
and <mem>,<reg>
and <reg>,<con>
and <mem>,<con>
 */

void and_opr(reg* reg1, reg* reg2) { //and <reg>,<reg>
    //clear of and cf flags
    of = false;
    cf = false;
    if (reg1->regType == reg2->regType) {
        if (reg1->regType == 16) { //both 16 bit registers
            unsigned short temp = reg1->sixteenValue & reg2->sixteenValue;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_16(temp);
        } else { //both 8 bit registers
            unsigned char temp = reg1->eightValue & reg2->eightValue;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_8(temp);
        }
    } else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void and_opr(reg* reg1, pair<int, unsigned short> realAddress){ // and <reg>,<mem>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type || (reg1->regType == 16 && type == 's') || (reg1->regType == 8 && type == 'c')){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue & memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_16(temp);
        }
        else{ // both 8 bit values
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue & memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void and_opr(pair<int, unsigned short> realAddress, reg* reg1){ // and <mem>, <reg>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue & memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            // x & ((1 << N) - 1)
            memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
        }
        else{ // both 8 bit balues
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue & memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            memory[addressValue]= temp;
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void and_opr(reg* reg1, unsigned constant, char constType){ // and <reg>,<const>
    //clear of and cf flags
    of = false;
    cf = false;

    if(reg1->regType==16 && constType=='s'){ //both 16 bit values
        unsigned short temp = reg1->sixteenValue & ((unsigned short) constant);
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_16(temp);
    }
    else if(reg1->regType==8 && constType=='c'){ // both 8 bit values
        unsigned char temp = reg1->eightValue &((unsigned char) constant);
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_8(temp);
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void and_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType){ // and <mem>, <const>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(type==16 && constType=='s'){ //both 16 bit values
        unsigned short memVal=concat(memory[addressValue],memory[addressValue+1]);
        unsigned short temp = constant & memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else if(type==8 && constType=='c'){ // both 8 bit
        unsigned char memVal= memory[addressValue];
        unsigned char temp = constant & memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue]= temp;
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}
//------------------------------LOGICAL OR

//The output of bitwise OR is 1 if at least one corresponding bit of two operands is 1.
//The source operand can be an immediate, a register, or a memory location; the destination operand can be a register or a memory location.
//The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
/*SYNTAX
or <reg>,<reg>
or <reg>,<mem>
or <mem>,<reg>
or <reg>,<con>
or <mem>,<con>
 */

void or_opr(reg* reg1, reg* reg2) { // or <reg>,<reg> |
    //clear of and cf flags
    of = false;
    cf = false;
    if(reg1->regType==reg2->regType){
        if(reg1->regType==16){ //both 16 bit registers
            unsigned short temp = reg1->sixteenValue | reg2->sixteenValue;

            if(temp == 0){
                zf=true;
            }
            if(temp>>16 == 1){//negative according to sign convention
                sf=true;
            }
            reg1->update_16(temp);
        }
        else{ //both 8 bit registers
            unsigned char temp = reg1->eightValue | reg2->eightValue;
            if(temp == 0){
                zf=true;
            }
            if(temp>>8 == 1){//negative according to sign convention
                sf=true;
            }
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }

}

void or_opr(reg* reg1, pair<int, unsigned short> realAddress){ // or <reg>,<mem>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type || (reg1->regType == 16 && type == 's') || (reg1->regType == 8 && type == 'c')){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue | memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_16(temp);
        }
        else{ // both 8 bit values
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue | memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void or_opr(pair<int, unsigned short> realAddress, reg* reg1){ // or <mem>, <reg>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue | memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            // x & ((1 << N) - 1)
            memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
        }
        else{ // both 8 bit values
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue | memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            memory[addressValue]= temp;
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void or_opr(reg* reg1, unsigned constant, char constType){ // or <reg>,<const>
    //clear of and cf flags
    of = false;
    cf = false;

    if(reg1->regType==16 && constType=='s'){ //both 16 bit values
        unsigned short temp = reg1->sixteenValue | ((unsigned short) constant);
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_16(temp);
    }
    else if(reg1->regType==8 && constType=='c'){ // both 8 bit values
        unsigned char temp = reg1->eightValue |(unsigned char) constant;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_8(temp);
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void or_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType){ // or <mem>, <const>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(type==16 && constType=='s'){ //both 16 bit values
        unsigned short memVal=concat(memory[addressValue],memory[addressValue+1]);
        unsigned short temp = constant | memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else if(type==8 && constType=='c'){ // both 8 bit values
        unsigned char memVal= memory[addressValue];
        unsigned char temp = constant | memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue]= temp;
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

//------------------------------LOGICAL XOR

//The result of bitwise XOR operator is 1 if the corresponding bits of two operands are opposite.
//The source operand can be an immediate, a register, or a memory location; the destination operand can be a register or a memory location.
//The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
/*SYNTAX
xor <reg>,<reg>
xor <reg>,<mem>
xor <mem>,<reg>
xor <reg>,<con>
xor <mem>,<con>
 */

void xor_opr(reg* reg1, reg* reg2){ // xor <reg>,<reg> ^
    //clear of and cf flags
    of = false;
    cf = false;
    if(reg1->regType==reg2->regType){
        if(reg1->regType==16){ //both 16 bit registers
            unsigned short temp = reg1->sixteenValue  ^ reg2->sixteenValue;

            if(temp == 0){
                zf=true;
            }
            if(temp>>16 == 1){//negative according to sign convention
                sf=true;
            }
            reg1->update_16(temp);
        }
        else{ //both 8 bit registers
            unsigned char temp = reg1->eightValue ^ reg2->eightValue;
            if(temp == 0){
                zf=true;
            }
            if(temp>>8 == 1){//negative according to sign convention
                sf=true;
            }
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }

}

void xor_opr(reg* reg1, pair<int, unsigned short> realAddress){ // xor <reg>,<mem>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type || (reg1->regType == 16 && type == 's') || (reg1->regType == 8 && type == 'c')){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue ^ memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_16(temp);
        }
        else{ // both 8 bit values
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue ^ memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            reg1->update_8(temp);
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void xor_opr(pair<int, unsigned short> realAddress, reg* reg1){ // xor <mem>, <reg>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(reg1->regType==type){
        if(reg1->regType==16){ //both 16 bit values
            unsigned short memVal= concat(memory[addressValue],memory[addressValue+1]);
            unsigned short temp = reg1->sixteenValue ^ memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 16 == 1) {//negative according to sign convention
                sf = true;
            }
            // x & ((1 << N) - 1)
            memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
        }
        else{ // both 8 bit values
            unsigned char memVal= memory[addressValue];
            unsigned char temp = reg1->eightValue ^ memVal;
            if (temp == 0) {
                zf = true;
            }
            if (temp >> 8 == 1) {//negative according to sign convention
                sf = true;
            }
            memory[addressValue]= temp;
        }
    }
    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void xor_opr(reg* reg1, unsigned constant, char constType){ // xor <reg>,<const>
    //clear of and cf flags
    of = false;
    cf = false;

    if(reg1->regType==16 && constType=='s'){ //both 16 bit values
        unsigned short temp = reg1->sixteenValue ^ ((unsigned short) constant);
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_16(temp);
    }
    else if(reg1->regType==8 && constType=='c'){ // both 8 bit values
        unsigned char temp = reg1->eightValue ^(unsigned char) constant;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        reg1->update_8(temp);
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

void xor_opr(pair<int, unsigned short> realAddress, unsigned constant, char constType){ // xor <mem>, <const>
    //clear of and cf flags
    of = false;
    cf = false;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)

    if(type==16 && constType=='s'){ //both 16 bit values
        unsigned short memVal=concat(memory[addressValue],memory[addressValue+1]);
        unsigned short temp = constant ^ memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 16 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else if(type==8 && constType=='c'){ // both 8 bit values
        unsigned char memVal= memory[addressValue];
        unsigned char temp = constant ^ memVal;
        if (temp == 0) {
            zf = true;
        }
        if (temp >> 8 == 1) {//negative according to sign convention
            sf = true;
        }
        memory[addressValue]= temp;
    }

    else { //both operands must be of same type
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
}

//------------------------------LOGICAL NOT

//Logically negates the operand contents (that is, flips all bit values in the operand).
//Bitwise compliment operator is an unary operator (works on only one operand). It changes 1 to 0 and 0 to 1.
//The destination operand can be a register or a memory location.
// No flags affected
/*SYNTAX
not <reg>
not <mem>
 */

void not_opr(reg* reg1){ //not <reg> ~
    if(reg1->regType==16){ //16 bit reg
        unsigned short temp= ~ (reg1->sixteenValue);
        reg1->update_16(temp);
    }
    else{ //8 bit reg
        unsigned char temp = ~ (reg1->eightValue);
        reg1->update_8(temp);
    }
}

void not_opr(pair<int, unsigned short> realAddress) { //not <mem>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    // Now for all options, we have addressValue which is the location in memory and type, (byte or word)
    if (type == 8) { // 8 bit char value
        unsigned char temp = memory[addressValue];
        temp = ~ temp;
        memory[addressValue] = temp;
    } else { // 16 bit unsigned short value
        unsigned char temp = concat(memory[addressValue], memory[addressValue + 1]);
        temp = ~ temp;
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
}


//----------------------------------------SHIFT OPERATIONS




//------------------------------SHL AND SHR

//These instructions shift the bits in their first operand's contents left and right, padding the resulting empty bit positions with zeros.
// The shifted operand can be shifted up to 31 places. The number of bits to shift is specified by the second operand, which can be either an 8-bit constant
// or the register CL. In either case, shifts counts of greater then 31 are performed modulo 32.
// Bits shifted beyond the destination operand boundary are first shifted into the CF flag, then discarded.
// At the end of the shift operation, the CF flag contains the last bit shifted out of the destination operand.
//The destination operand can be a register or a memory location. The count operand can be an immediate value or register CL.
// The count is masked to 5 bits, which limits the count range to 0 to 31. A special opcode encoding is provided for a count of 1.
//For each shift count, the most significant bit of the destination operand is shifted into the CF flag, and the least significant bit is cleared
//The OF flag is affected only on 1-bit shifts. For left shifts, the OF flag is set to 0 if the mostsignificant bit of the result is the same as the CF flag (that is, the top two bits of the original operand were the same); otherwise, it is set to 1. For the SAR instruction, the OF flag is cleared for all 1-bit shifts.
//

/*SYNTAX
shr <reg>,<con8>
shr <mem>,<con8>
shr <reg>,<cl>
shr <mem>,<cl>
 */

//Right shift operator shifts all bits towards right by certain number of specified bits. It is denoted by >>
//Bit shifted out goes into the carry flag CF.
// For the SHR instruction, the OF flag is set to the most-significant bit of the original operand.
//Zero bit is shifted in at the other end
// Specification of count greater than 31 is not allowed. If greater, only the least significant 5 bits are actually used.
void shift_right_opr(reg* reg1, unsigned constant){ //shr <reg>,<con>
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry lsb into carry flag
            cf= LSB_16(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(reg1->sixteenValue);
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= LSB_8(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(reg1->eightValue);
        }
        reg1->update_8(temp);
    }
}

void shift_right_opr(pair<int, unsigned short> realAddress, unsigned constant){ //shr <mem>,<con>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(type==16){
        unsigned short temp=concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry lsb into carry flag
            cf= LSB_16(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(concat(memory[addressValue],memory[addressValue+1]));
        }

        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= LSB_8(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7)== 1) {
            of = MSB_8(memory[addressValue]);
        }

        memory[addressValue]=temp;
    }
}

void shift_right_opr(reg* reg1, reg* reg2){ //shr <reg>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant=cl.eightValue;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry lsb into carry flag
            cf= LSB_16(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(reg1->sixteenValue);
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= LSB_8(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(reg1->eightValue);
        }
        reg1->update_8(temp);
    }
}

void shift_right_opr(pair<int, unsigned short> realAddress, reg* reg2){ //shr <mem>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant=cl.eightValue;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(type==16){
        unsigned short temp=concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry lsb into carry flag
            cf= LSB_16(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(concat(memory[addressValue],memory[addressValue+1]));
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= LSB_8(temp);
            temp= temp>>1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(memory[addressValue]);
        }

        memory[addressValue]=temp;
    }
}



/*SYNTAX
shl <reg>,<con8>
shl <mem>,<con8>
shl <reg>,<cl>
shl <mem>,<cl>
 */

// Left shift operator shifts all bits towards left by certain number of specified bits. It is denoted by <<.
//Bit shifted out goes into the carry flag CF.
//Zero bit is shifted in at the other end
// Specification of count greater than 31 is not allowed. If greater, only the least significant 5 bits are actually used.
void shift_left_opr(reg* reg1, unsigned constant){ //shl <reg>,<con8>
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }

        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        reg1->update_8(temp);
    }
}

void shift_left_opr(pair<int, unsigned short> realAddress, unsigned constant){ //shl <mem>,<con>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(type==16){
        unsigned short temp= concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }

        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits;
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        memory[addressValue]=temp;
    }
}

void shift_left_opr(reg* reg1, reg* reg2){ //shl <reg>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant= cl.eightValue;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        reg1->update_8(temp);
    }
}

void shift_left_opr(pair<int, unsigned short> realAddress, reg* reg2){ //shl <mem>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant= cl.eightValue;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant;
    if(constant>16)
        temporaryCount=0;
    if(type==16){
        unsigned short temp= concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits;
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= temp<<1;
            temporaryCount--;
        }
        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }
        memory[addressValue]=temp;
    }
}

//---------------------------------------- ROTATE WITH CARRY OPERATIONS


//The CF flag contains the value of the bit shifted into it.
// The OF flag is affected only for singlebit rotates; it is undefined for multi-bit rotates.
// The SF, ZF, AF  flags are not affected.

//Shifts (rotates) the bits of the first operand (destination operand) the number of bit positions specified in the second operand (count operand) and stores the result in the destination operand.
//The destination operand can be a register or a memory location; the count operand is an unsigned integer that can be an immediate or a value in the CL register.
// The processor restricts the count to a number between 0 and 8 or 16 by masking all the bits in the count operand.


//RCL: Rotates the bits in the destination to the left "count" times with all data pushed out the left side re-entering on the right.
// The Carry Flag holds the last bit rotated out.
// For left rotates, the OF flag is set to the exclusive OR of the CF bit (after the rotate) and the most-significant bit of the result.

void rotate_left_opr(reg* reg1, unsigned constant){ //rcl <reg>,<con>
    unsigned temporaryCount;
    if(reg1->regType==16){
        temporaryCount= constant % 15;
        unsigned short temp= reg1->sixteenValue;
        int precf=cf;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }

        reg1->update_16(temp);
    }
    else { // 8 bit reg
        temporaryCount= constant % 7;
        unsigned char temp= reg1->eightValue;
        int precf=cf;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        reg1->update_8(temp);
    }
}

void rotate_left_opr(pair<int, unsigned short> realAddress, unsigned constant){ //rcl <mem>,<con>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount;
    if(type==16){
        temporaryCount= constant % 15;
        unsigned short temp= concat(memory[addressValue],memory[addressValue+1]);
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }

        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits;
    }
    else { // 8 bit reg
        temporaryCount= constant % 7;
        unsigned char temp= memory[addressValue];
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        memory[addressValue]=temp;
    }
}

void rotate_left_opr(reg* reg1, reg* reg2){ //rcl <reg>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant= cl.eightValue;
    unsigned temporaryCount=constant%15;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }

        reg1->update_8(temp);
    }
}

void rotate_left_opr(pair<int, unsigned short> realAddress, reg* reg2){ //rcl <mem>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant= cl.eightValue;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant%15;
    if(type==16){
        unsigned short temp= concat(memory[addressValue],memory[addressValue+1]);
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_16(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(temp) ^ cf;
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits;
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        int precf=0;
        while(temporaryCount!=0){
            //carry msb into carry flag
            cf= MSB_8(temp);
            temp= (temp<<1)|precf;
            precf= cf;
            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(temp) ^ cf;
        }
        memory[addressValue]=temp;
    }
}

//RCR:Rotates the bits in the destination to the right "count" times with all data pushed out the right side re-entering on the left.
// The Carry Flag holds the last bit rotated out.
// For right rotates, the OF flag is set to the exclusive OR of the two most-significant bits of the result.

void rotate_right_opr(reg* reg1, unsigned constant){ //rcr <reg>,<con>
    unsigned temporaryCount;
    if(reg1->regType==16){
        temporaryCount= constant % 15;
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry lsb into carry flag

            temp= (temp>>1)|(cf<<15);
            cf= LSB_16(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(reg1->sixteenValue);
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        temporaryCount= constant % 7;
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            temp= (temp>>1)|(cf<<7);
            cf= LSB_8(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(reg1->eightValue);
        }
        reg1->update_8(temp);
    }
}

void rotate_right_opr(pair<int, unsigned short> realAddress, unsigned constant){ //rcr <mem>,<con>
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount;
    if(type==16){
        temporaryCount= constant % 15;
        unsigned short temp=concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry lsb into carry flag
            temp= (temp>>1)|(cf<<15);
            cf= LSB_16(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(concat(memory[addressValue],memory[addressValue+1]));
        }

        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else { // 8 bit reg
        temporaryCount= constant % 7;
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            temp= (temp>>1)|(cf<<7);
            cf= LSB_8(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 7)== 1) {
            of = MSB_8(memory[addressValue]);
        }

        memory[addressValue]=temp;
    }
}

void rotate_right_opr(reg* reg1, reg* reg2){ //rcr <reg>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant=cl.eightValue;
    unsigned temporaryCount=constant%15;
    if(reg1->regType==16){
        unsigned short temp= reg1->sixteenValue;
        while(temporaryCount!=0){
            //carry lsb into carry flag
            temp= (temp>>1)|(cf<<15);
            cf= LSB_16(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(reg1->sixteenValue);
        }
        reg1->update_16(temp);
    }
    else { // 8 bit reg
        unsigned char temp= reg1->eightValue;
        while(temporaryCount!=0){
            //carry msb into carry flag
            temp= (temp>>1)|(cf<<7);
            cf= LSB_8(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(reg1->eightValue);
        }
        reg1->update_8(temp);
    }
}

void rotate_right_opr(pair<int, unsigned short> realAddress, reg* reg2){ //rcr <mem>,<cl>
    if(reg2!=&cl){
        cout << "ERROR"<< endl;
        exit(0);
        return;
    }
    unsigned char constant=cl.eightValue;
    int type=realAddress.first;
    unsigned short addressValue=realAddress.second;
    unsigned temporaryCount=constant%15;
    if(type==16){
        unsigned short temp=concat(memory[addressValue],memory[addressValue+1]);
        while(temporaryCount!=0){
            //carry lsb into carry flag
            temp= (temp>>1)|(cf<<15);
            cf= LSB_16(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 15) == 1) {
            of = MSB_16(concat(memory[addressValue],memory[addressValue+1]));
        }
        memory[addressValue+1]=temp>>8; //8 most significant bits
            memory[addressValue]=temp & ((1<<8)-1); // 8 least significant bits
    }
    else { // 8 bit reg
        unsigned char temp= memory[addressValue];
        while(temporaryCount!=0){
            //carry msb into carry flag
            temp= (temp>>1)|(cf<<7);
            cf= LSB_8(temp);

            temporaryCount--;
        }

        //todo check here
        if((constant & 7) == 1) {
            of = MSB_8(memory[addressValue]);
        }

        memory[addressValue]=temp;
    }
}

//----------------------------------------HELPER FUNCTIONS

//concatanetes binary values of two 8 bit values and returns corresponding decimal value
unsigned concat(unsigned char a, unsigned char b) {
    // multiply first one by 2^8 (shift 8 bits left) and append the second number
    unsigned temp=b*256+a;
    return temp;
}


// Function to convert hexadecimal to decimal
unsigned long hex2dec(string hex){
    unsigned long result = 0;
    if(hex[hex.length()-1]=='h'){
        for (int i=0; i<hex.length()-1; i++) {
            if (hex[i]>=48 && hex[i]<=57){
                result += (hex[i]-48)*pow(16,hex.length()-i-2);
            } else if (hex[i]>=65 && hex[i]<=70) {
                result += (hex[i]-55)*pow(16,hex.length( )-i-2);
            } else if (hex[i]>=97 && hex[i]<=102) {
                result += (hex[i]-87)*pow(16,hex.length()-i-2);
            }
        }
    }
    else{
        for (int i=0; i<hex.length(); i++) {
            if (hex[i]>=48 && hex[i]<=57){
                result += (hex[i]-48)*pow(16,hex.length()-i-1);
            } else if (hex[i]>=65 && hex[i]<=70) {
                result += (hex[i]-55)*pow(16,hex.length( )-i-1);
            } else if (hex[i]>=97 && hex[i]<=102) {
                result += (hex[i]-87)*pow(16,hex.length()-i-1);
            }
        }
    }
    return result;
}


pair<int,unsigned int> realAddressFinder(string address){
    int type = 0;
    bool isItReg = false;
    unsigned addressValue = 0;
    //check if it is word length or byte length
    if (address[0] == 'w') {
        type = 16;
        address = address.substr(2, address.length() - 3);
    } else {
        if (address[0] == 'b') {
            type = 8;
            address = address.substr(2, address.length() - 3);
        } else { //it is [, assume b for default
            type = 8;
            address = address.substr(1, address.length() - 2);
        }
    }

    // We learned the type and extracted the value between []
    //If the address contains a register, we have to look at the value in the register to learn the memory adress
    for (auto itr = sixteenBitRegs.find(address); itr != sixteenBitRegs.end(); itr++) {
        isItReg = true;
        addressValue = itr->second->sixteenValue;
        break;
    }
    for (auto itr = eightBitRegs.find(address); itr != eightBitRegs.end(); itr++) {
        isItReg = true;
        addressValue = (itr->second->eightValue)+0;
        break;
    }
    //our address is not a register, it is a number, dec or hex
    if (!isItReg) {
        if (address[address.length() - 1] == 'h'|| address[0]=='0') { // the address is given as hexadecimal number
            if(address[0]=='0')
                addressValue=hex2dec(address);
            else
                addressValue = hex2dec(address.substr(0,address.length()));     //convert hex to decimal
        } else {
            stringstream converter(address);
            converter >> addressValue;            //convert string to decimal
        }
    }
    if(addressValue>65536){
        cout<<"ERROR"<<endl;
        exit(0);
    }
    //return proper memory address and length in pair
    return make_pair(type,addressValue);

}

//function to convert decimal number to binary
unsigned long long int decToBinary(unsigned long long int num) {
    unsigned long long int binaryNum = 0;
    unsigned long long int i = 1, rem;

    //while loop to convert decimal to binary
    while (num != 0){
        rem = num % 2;
        num /= 2;
        binaryNum += rem * i;
        i *= 10;
    }
    return binaryNum;

}

char findOperandType(string op) { // returns m for memory, r for register, c for constant

    //if it contains brackets, it is a memory location
    if (op.find('[') != std::string::npos){
        return 'm';
    }
    //else it can be a register, we search for the operand in map of registers
    for (auto itr = sixteenBitRegs.find(op); itr != sixteenBitRegs.end(); itr++) {
        return 'r';
        break;
    }
    for (auto itr = eightBitRegs.find(op); itr != eightBitRegs.end(); itr++) {
        return 'r';
        break;
    }
    for (auto itr = vars.find(op); itr != vars.end(); itr++) {
        return 'v';
        break;
    }
    if(op.length()==3 && op[0]=='\'' && op[2]=='\'')
        return 'c';

    if(op[0]=='0'|| op[op.length()-1]=='h'){
        for(int i=0; i<op.length(); i++){
            if(i==op.length()-1 && op[i]=='h')
                continue;

            if(op[i]=='0' || op[i]=='1' || op[i]=='2' || op[i]=='3' || op[i]=='4' || op[i]=='5' || op[i]=='6' || op[i]=='7' || op[i]=='8' || op[i]=='9'|| op[i]=='a'|| op[i]=='b'|| op[i]=='c'|| op[i]=='d'|| op[i]=='e'|| op[i]=='f'|| op[i]=='A'|| op[i]=='B'|| op[i]=='C'|| op[i]=='D'|| op[i]=='E'|| op[i]=='F')
                continue;
            else{
                cout<<"ERROR"<<endl;
                exit(0);
            }
        }
    }
    else{
        for(int i=0; i<op.length(); i++){
            if(i==op.length()-1 && op[i]=='d')
                continue;
            if(op[i]=='0' || op[i]=='1' || op[i]=='2' || op[i]=='3' || op[i]=='4' || op[i]=='5' || op[i]=='6' || op[i]=='7' || op[i]=='8' || op[i]=='9')
                continue;
            else{
                cout<<"ERROR"<<endl;
                exit(0);
            }
        }
    }

    return 'c';

    //if not var,reg or memory, it is a constant
}

unsigned char MSB_16(unsigned short temp) {
    return temp>>15;
}
unsigned char MSB_8(unsigned char temp) {
    return temp>>7;
}
unsigned char LSB_16(unsigned short temp) {
    return temp&1;
}
unsigned char LSB_8(unsigned char temp) {
    return temp&1;
}