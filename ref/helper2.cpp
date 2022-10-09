#include <iostream>
#include <fstream>
using namespace std;
#define int long long

// nome da pasta como primeiro argumento, nome dos arquivos como segundo, dimensão da matriz como terceiro
int32_t main(int32_t argc, char** argv){
    string pasta{argv[1]};
    string path{pasta + "/" + argv[2]};
    int p{(atoi(argv[3]) * atoi(argv[3]))/8};
    int num_pastas{(atoi(argv[3]) * atoi(argv[3]))/p};

    int max_time=0, cur_time;
    ifstream fileResult;
    for(int i =0; i < num_pastas; ++i){
        fileResult.open(path + to_string(i+1) + ".txt");

        if(true/*fileResult.is_open()*/) {
        fileResult.seekg(-2,ios_base::end);                // go to one spot before the EOF

        bool keepLooping = true;
        while(keepLooping) {
            char ch;
            fileResult.get(ch);                            // Get current byte's data

            if((int)fileResult.tellg() <= 1) {             // If the data was at or before the 0th byte
                fileResult.seekg(0);                       // The first line is the last line
                keepLooping = false;                // So stop there
            }
            else if(ch == '\n') {                   // If the data was a newline
                keepLooping = false;                // Stop at the current position.
            }
            else {                                  // If the data was neither a newline nor at the 0 byte
                fileResult.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
            }
        }

        string lastLine;            
        getline(fileResult,lastLine);                      // Read the current line
        cur_time = stoi(lastLine);
        max_time = max(max_time, cur_time);
        }
        fileResult.close();
    }
    cout << max_time<<endl;

}