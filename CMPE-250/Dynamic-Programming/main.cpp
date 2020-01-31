#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

typedef long long int lli;


using namespace std;

int main(int argc, char* argv[]){
    //auto start = clock();

    // Open the input and output files, check for failures
    ifstream inFile(argv[1]);
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }

    ofstream outFile(argv[2]);
    if (!outFile) {
        cerr << "There was a problem opening \"" << argv[2] << "\" as output file" << endl;
        return 1;
    }
    int n,m;
    string line, tmp;
    getline(inFile, line);
    istringstream firstLine(line);
    getline(firstLine, tmp, ' ');
    n = stoi(tmp);
    getline(firstLine, tmp, ' ');
    m = stoi(tmp);
    getline(inFile, line);
    istringstream secondLine(line);
    vector<int> real(n,0);
    vector<lli> addBack(n,0);

    for(int i=0;i<n;i++){
    getline(secondLine, tmp, ' ');
        int k = stoi(tmp);
        real[i]=k;
    }
    std::sort(real.begin(), real.end());
    // I hold the initial prices of all chocolates in sorted order

    // this is where we use dynamic programming
    // in each step we add all the m'th elements going backwards but that's costly.
    // instead we hold sums of the all m'th elements in an array starting from that index
    // so that when calculating the amount we need to add, we just look for once
    for(int i=m; i<n; i++){
        addBack[i]=addBack[i-m]+real[i-m];
    }

    // the result increases in each step by the amount of new added element plus the addBack value.
    lli result=0;
    for(int i=1; i<=n ;i++){
        outFile<<result+real[i-1]+addBack[i-1]<<" ";
        result+=real[i-1]+addBack[i-1];
    }

    outFile.close();
    inFile.close();

    /*auto end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

    cout << "Time taken by program is : " << fixed

         << time_taken << setprecision(5);

    cout << " sec " << endl;
     */
    return 0;
}
