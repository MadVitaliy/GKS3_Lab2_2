#include <iostream>
#include <fstream>
#include <sstream>

#include "libs/Matrix.h"
#include "libs/TechProcess.h"
#include "libs/GantDiagram.h"

static void show_usage();

static bool isReserved(std::string arg);

static int getIntFromFile(std::ifstream& in);

static Matrix<int> getMatrixFromFile(std::ifstream& in, unsigned int n, unsigned int m);

static Matrix<float> getFloatMatrixFromFile(std::ifstream& in, unsigned int n, unsigned int m);




int main(int argc, char **argv) {
    std::string inputPath = "./input.txt";
    std::string outputPath = "./output.txt";


    bool outInConcole = false;
    bool outInFile = false;


    if(argc > 1) {
        int i = 1;
        do {
            std::string arg = argv[i];
            if ((arg == "-h") || (arg == "--help")) {
                show_usage();
                return 0;
            }
            else if (arg == "-c"){
                if(outInFile) {
                    std::cerr << "You are not allowed to use -c and -o simultaneously\n";
                    return 3;
                }
                outInConcole = true;
            }
            else if (arg == "-o"){
                if(outInConcole) {
                    std::cerr << "You are not allowed to use -c and -o simultaneously\n";
                    return 3;
                }
                outInFile = true;

                if(i+1 != argc)
                    if(isReserved(argv[i+1]))
                        outputPath = argv[i+1];
            }
            else if (arg == "-i"){
                if(i+1 != argc)
                    if(isReserved(argv[i+1]))
                        outputPath = argv[i+1];
            }
            i++;
        }while( i < argc);
    } else {
        outInFile = true;
    }

    std::string line;
    std::ifstream in(inputPath, std::ifstream::in);
    int tempVar;
    if(!in.is_open()) {
        std::cout << "Cannot open input file: "<< inputPath << std::endl;
        return 1;
    }

    std::ofstream out;
    if(outInFile){
        out.open(outputPath);
        if(!out.is_open()){
            std::cout << "Cannot open output file: "<< outputPath << std::endl;
            exit(3);
        }
        //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    }

    unsigned int n = static_cast<unsigned int>(getIntFromFile(in));
    unsigned int M = static_cast<unsigned int>(getIntFromFile(in));
    Matrix<int> tm = getMatrixFromFile(in,n,M);
    Matrix<float> to = getFloatMatrixFromFile(in,n,M);
    int p = getIntFromFile(in);
    int r = getIntFromFile(in);
    in.close();

    std::cout << std::fixed;
    std::cout << std::setprecision(1);
    std::cout << "Number of details:\nN = "<< n << "\n\nNumber of steps: \nK = " << M << "\n\n";
    std::cout << "Technological route TM:" << std::endl;
    std::cout << tm.strWithNumerating() << std::endl;
    std::cout << "Time to do operations TO:" << std::endl;
    std::cout << to.strWithNumerating() << std::endl;
    std::cout << "Preference rule:\nP = "<< p << "\n\nEfficiency criterion: \nR = " << r << "\n\n";




    //std::cout << to.maxInRaw(3) << "\n\n";

    TechProcess techProcess(tm,to,p);
    techProcess.precess();
   // techProcess.findParametersOfProcess();
    //techProcess.findWaitingtime();
    techProcess.findTimeBeforeStart();
    Matrix<float> st = techProcess.getOperationStartTime();
    Matrix<float> et = st+to;
    Matrix<float> timeBeforeStart = techProcess.getTimeBeforeStart();

    std::cout << "Start operations time:" << std::endl;
    std::cout << st.strWithNumerating() << std::endl;
    std::cout << "End operations time:" << std::endl;
    std::cout << et.strWithNumerating() << std::endl;

    std::cout << '\n' << techProcess.prettyTable() << std::endl << std::endl;
    std::cout << "Gant's diagram:" << std::endl;
    std::cout << drawGantDiagram(tm,to,st) << std::endl;

    std::cout << "Time before start details operation for each machine:" << std::endl;
    std::cout << techProcess.getTimeBeforeStart().strWithNumerating() << std::endl;

    std::cout << "Before operations downtimes for each machine:" << std::endl;
    std::cout << techProcess.getBeforeOperationsDowntimes().strWithNumerating() << std::endl;

    std::cout << "Between operations downtimes for each machine:" << std::endl;
    std::cout << techProcess.getBetweenOperationsDowntimes().strWithNumerating() << std::endl;

    std::cout << "Machines total process time:" << std::endl;
    std::cout << techProcess.getMachinesTotalTimes().strWithNumerating() << std::endl;

    std::cout << "Efficiency criterion " << r << " is "
              << techProcess.efficiencyCriterion(r) << std::endl;

    return 0;
}

static void show_usage()
{
    std::cerr << "Use: " << "progname.exe " << " <option(s)>" << std::endl;
    std::cout << "Options:\n"
              << "-h,--help\tShow this help message\n\n"
              << "-i PATH\t\tset path to input data. Default \"./input.txt\"\n\n"
              << "-o PATH\t\tset path for output data. Default \"./output.txt\"\n\n"
              << "-c\t\toutput result in the console\n"
              << "\t\tDON\'T use with \'-o\' \n\n"
              << "\t\t25.03.2021\tby @mad_skipper"
              << std::endl;
}

static bool isReserved(std::string arg) {
    return !(arg == "-h" ||
             arg == "--help" ||
             arg == "-i" ||
             arg == "-o" ||
             arg == "-c");

/*    if(arg == "-h" ||
       arg == "--help" ||
       arg == "-i" ||
       arg == "-o" ||
       arg == "-c")
        return false;
    return true;*/
}

static int getIntFromFile(std::ifstream& in) {
    std::string line;
    do{
        getline(in, line);
    }while(line[0] == '/' && line[1] == '/');
    return  std::stoi(line);
}

static Matrix<int> getMatrixFromFile(std::ifstream& in, unsigned int n, unsigned int m) {
    Matrix<int> result(n,m);
    std::stringstream ss;
    std::string line;
    do{
        getline(in, line);
    }while(line[0] == '/' && line[1] == '/');

    while(!(line[0] == '/' && line[1] == '/')){
        ss << line << std::endl;
        getline(in, line);
    }

    // std::cout << ss.str() << std::endl;
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j) {
            ss >> result[i][j];
        }
    }

    return  result;
}

static Matrix<float> getFloatMatrixFromFile(std::ifstream& in, unsigned int n, unsigned int m) {
    Matrix<float> result(n,m);
    std::stringstream ss;
    std::string line;
    do{
        getline(in, line);
    }while(line[0] == '/' && line[1] == '/');

    while(!(line[0] == '/' && line[1] == '/')){
        ss << line << std::endl;
        getline(in, line);
    }

    // std::cout << ss.str() << std::endl;
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j) {
            ss >> result[i][j];
        }
    }

    return  result;
}
