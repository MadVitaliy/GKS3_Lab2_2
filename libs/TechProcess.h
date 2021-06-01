//
// Created by Vitalik on 01.04.2021.
//

#ifndef LABA2PART2_TECHPROCESS_H
#define LABA2PART2_TECHPROCESS_H

#include <vector>
#include <map>
#include <algorithm>
#include "ZipUnzip.h"

#include "Matrix.h"

class Case {
public:
    int chosen = -1;
    int numOfDet = 0;
    std::vector<int> details;
    std::vector<float> criterias;

    bool hasDetail(int detail) {
        return std::find(details.begin(), details.end(), detail) != details.end();
    }

    void addDetail(int detail, float criteria){
        if(hasDetail(detail))
            return;
        numOfDet++;
        details.push_back(detail);
        criterias.push_back(criteria);
    }

    int findMin(){
        if(criterias.empty())
            return -1;
        int index = static_cast<int>(std::min_element(criterias.begin(),criterias.end()) - criterias.begin());
        return details[index];
    }
    int findMax(){
        if(criterias.empty())
            return -1;
        int index = static_cast<int>(std::max_element(criterias.begin(),criterias.end()) - criterias.begin());
        return details[index];
    }

    Case getNotChose() {
        Case result;
        for (int i = 0; i < details.size(); ++i) {
            if(details[i] == chosen)
                continue;

            result.addDetail(details[i],criterias[i]);
        }
        return result;
    }
};


class TechProcess {
private:
    bool _initialized = false;

    unsigned int _n = 0;        //num of details
    unsigned int _m = 0;        //num of machines
    unsigned int _M = 0;        //num of operations
    unsigned int _p = 0;        //preference rule
    Matrix<int> _tm;            //_n*_M  process root -- every raw -- machines detail shoult go through
    Matrix<float> _to;          //_n*_M  operations times -- every raw -- time of processing on the machine
    Matrix<float> _operationStartTime;  //_n*_M
    Matrix<float> _operationEndTime;    //_n*_M
    Matrix<float> _machineStartTime;    //_n*_M
    Matrix<float> _machineEndTime;      //_n*_m
    Matrix<int> _processState;          //_n*_m

    Matrix<float> _timeBeforeStart;     //_n*1
    Matrix<float> _beforeOperationsDowntimes;   //_m*1
    Matrix<float> _betweenOperationsDowntimes;  //_m*1
    Matrix<float> _machinesTotalTimes;             //_m*1  -- total process time on machines


    std::vector<float> _times;  // -- key times when operations start or end
    std::map<float,std::vector<Case>> _briefcase;// map has _m elements

    void addNewTimeToBriefcase(float time){
        std::vector<Case> temp;
        for (int i = 0; i < _m; ++i) {
            temp.push_back(Case());
        }
        _briefcase[time] = temp;
    }

    bool detailInProcess(int detail, float time){

        float lowerlimit = _operationStartTime.maxInRaw(detail);
        float upperlimit = _operationEndTime.maxInRaw(detail);

        return lowerlimit < time && upperlimit > time;

    }

    bool machineInProcess(int machine, float time){
         return _machineEndTime.maxInColumn(machine) > time;
    }

    int getNextMachine(int detail) {
        for (int i = 0; i < _M; ++i) {
            if(_processState[detail][i] == 0)
                return _tm[detail][i];
        }
        return -1;
    }


    int getNextNextMachine(int detail) {
        int nextOperation = 0;
        for (int i = 0; i < _M; ++i) {
            if(_processState[detail][i] == 0) {
                //return _tm[detail][i];
                nextOperation = i;
                break;
            }
        }
        if(nextOperation < _M-1)
            return _tm[detail][nextOperation+1];

        return -1;
    }


    //popravity
    float getNextTime(float time){
        float minTime = _machineEndTime.max();
        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < _m; ++j) {
                if(_machineEndTime[i][j] > time && _machineEndTime[i][j] < minTime )
                    minTime = _machineEndTime[i][j];
            }
        }
        return minTime;
    }

    int getCurrentOperation(int detail) {
        for (int i = 0; i < _M; ++i) {
            if(_processState[detail][i] == 0)
                return i;
        }

        return -1;
    }

    //work
    float SPT(int detail){
        float result = 0;
        int operation = getCurrentOperation(detail);
        result = _to[detail][operation];

        return result;
    }

    //work
    float MWKR(int detail){
        float result = 0;
        for (int i = 0; i < _M; ++i) {
            if(_processState[detail][i] != 1)
                result += _to[detail][i];
        }
        return result;
    }

    //to do
    float AMG(int detail){
        float result = 0;
        return result;
    }

    //work
    float LUKR(int detail){
        float result = 0;
        for (int i = 0; i < _M; ++i) {
            if(_processState[detail][i] != 1)
                result += _to[detail][i];
        }
        return result;
    }
    //work
    float LPT(int detail){
        float result = 0;
        int operation = getCurrentOperation(detail);
        result = _to[detail][operation];

        return result;
    }

    std::string drawLine(std::vector<int>& widths){
        std::stringstream ss;
        ss.precision(1);
        ss << '+';
        for(int width: widths) {
            for (int i = 0; i < width + 2; ++i) {
                ss << '-';
            }
            ss << '+';
        }
        ss << '\n';
        return ss.str();
    }

    std::string drawRaw(std::vector<int>& widths, std::vector<std::string>& words){
        if(widths.size() != words.size())
            return " ";

        std::stringstream ss;
        ss.precision(1);
        ss << "| ";


        int spaceSize = 0;
        for (int i = 0; i < words.size(); ++i) {
            ss << words[i];
            spaceSize = widths[i] - static_cast<int>(words[i].length());
            for (int j = 0; j < spaceSize; ++j) {
                ss << ' ';
            }
            ss << " | ";
        }
        ss << '\n';
        return ss.str();
    }

    float findQueueTime(int machine, float currentTime) {
        if(machine == -1)
            return 	2147483645;

        float resultTime = 0;
        int operation;
        for(int detail:  _briefcase[currentTime][machine].details) {
            operation = getCurrentOperation(detail);
            resultTime += _to[detail][operation];
        }

        return resultTime;
    }



public:

    void precess(){

        std::cout << std::fixed;
        std::cout << std::setprecision(1);

        if(!_initialized)
            return;
        float currentTime = -1;
        int machine;
        float criteria = 0;

        std::cout << '\t';
        for (int i = 0; i < _m; ++i) {
            std::cout << "GVM" << i+1 << '\t';
        }
        std::cout << '\n';


        std::vector<Case> transfer;
        //some magic tricks
        while(_processState.has(0)) {
            currentTime = getNextTime(currentTime);
            _times.push_back(currentTime);
            addNewTimeToBriefcase(currentTime);


            if(currentTime != 0) {
                _briefcase[currentTime] = transfer;
            }
            transfer.clear();

//            if(currentTime == 2) {
//               std::cout << "stop" << std::endl;
//            }


            for (int i = 0; i < _n; ++i) {

                if(detailInProcess(i, currentTime))
                    continue;
                machine = getNextMachine(i);
                if(machine == -1)
                    continue;
                if(machineInProcess(machine,currentTime))
                    continue;

                switch (_p) {
                    case 1:
                        criteria = SPT(i);
                        break;
                    case 2:
                        criteria = MWKR(i);
                        break;
                    case 3:
                        criteria = 0;
                        //criteria = AMG(i);
                        break;
                    case 4:
                        criteria = LUKR(i);
                        break;
                    case 5:
                        criteria = LPT(i);
                        break;
                    default:
                        criteria = 0;
                        break;
                }
                _briefcase[currentTime][machine].addDetail(i,criteria);
            }



            if(_p == 3) {
                std::cout << std::endl << "Current time: " << currentTime << std::endl;
                std::map<int,float> queueTimes;
                for (int i = 0; i < _m; ++i) {
                    queueTimes[i] = findQueueTime(i,currentTime);
                }


                for (int i = 0; i < _m; ++i) {
                    std::vector<Case> currentTimeCases(_briefcase[currentTime]);
                    std::vector<int> details(currentTimeCases[i].details);

                    std::cout << '\t' << "GVM" << i+1 <<" queue time: " << queueTimes[i] << std::endl;
                    std::cout << '\t' << "details:" << std::endl;


                    bool sameNextNextMachine = true;
                    for (int j = 0; j < _briefcase[currentTime][i].details.size(); ++j) {

                        //find next machine for this detail
                        //find queue of this machine
                        //find summary time of the details in the queue
                        //set criteria of the detail = sum
                        int detail = _briefcase[currentTime][i].details[j];
                        float tempCriteria;

                        int nextNextMachine = getNextNextMachine(detail);
                        if(nextNextMachine == -1)
                            tempCriteria =  2147483645;
                        else
                            tempCriteria = queueTimes[nextNextMachine];
                        _briefcase[currentTime][i].criterias[j] = tempCriteria;

                        if(j > 0 && sameNextNextMachine) {
                            if(_briefcase[currentTime][i].criterias[j-1] != _briefcase[currentTime][i].criterias[j])
                                sameNextNextMachine = false;
                        }


                        std::cout << '\t' << '\t' << detail + 1 <<  " next GVM:" << nextNextMachine + 1;
                        std::cout << "(" << _briefcase[currentTime][i].criterias[j] << ")\n";

                    }

                    std::cout <<  std::endl;

                    if(sameNextNextMachine && _briefcase[currentTime][i].details.size() > 1) {
                        std::cout << "Details have same criterias, so use SPT" << std::endl;
                        for (int j = 0; j < _briefcase[currentTime][i].details.size(); ++j) {
                            int detail = _briefcase[currentTime][i].details[j];
                            _briefcase[currentTime][i].criterias[j] = SPT(detail);
                            std::cout << '\t' << detail <<  "(" << _briefcase[currentTime][i].criterias[j] << ")\n";
                        }
                        std::cout <<  std::endl;
                    }


                }
            }


            for (int i = 0; i < _m; ++i) {
                if(machineInProcess(i,currentTime))
                    continue;

                int chosen;
                if(_p == 1 || _p == 3 || _p == 4)
                    chosen = _briefcase[currentTime][i].findMin();
                else
                    chosen = _briefcase[currentTime][i].findMax();
                _briefcase[currentTime][i].chosen = chosen;

                if(chosen == -1)
                    continue;

                int currentOperation = getCurrentOperation(chosen);
                _operationStartTime[chosen][currentOperation] = currentTime;
                _operationEndTime[chosen][currentOperation] = currentTime + _to[chosen][currentOperation];
                _machineStartTime[chosen][i] = currentTime;
                _machineEndTime[chosen][i] = currentTime + _to[chosen][currentOperation];
                _processState [chosen][currentOperation] = 1;


            }

            for (int i = 0; i < _m; ++i) {
                Case tempCase;
                transfer.push_back(_briefcase[currentTime][i].getNotChose());
            }

     /*       std::cout << currentTime << '\t';
            for (int i = 0; i < _m; ++i) {
                int chosen = _briefcase[currentTime][i].chosen;

                std::cout << '|';
                for (int j = 0; j < _briefcase[currentTime][i].numOfDet; ++j) {
                    int temp1 = _briefcase[currentTime][i].details[j];
                    float temp2 = _briefcase[currentTime][i].criterias[j];


                    if(temp1 == chosen)
                        std::cout << '*';
                    std::cout << temp1+1;
                    std::cout << '(' << temp2 << "),";
                }
                std::cout << '\t';
            }
            std::cout << std::endl;

            std::cout << "_operationStartTime" << '\n' <<_operationStartTime.str() << '\n';
            std::cout << "_operationEndTime" << '\n'<< _operationEndTime.str() <<'\n';
            std::cout << "_processState" << '\n'<< _processState.str() <<'\n';
            std::cout << "_machineStartTime" << '\n'<< _machineStartTime.str()  <<'\n';
            std::cout << "_machineEndTime" << '\n'<< _machineEndTime.str()  <<'\n';*/

           //getchar();
        }

        findParametersOfProcess();
    }




    std::string prettyTable(){

        std::stringstream ss;
        std::vector<std::vector<std::string>> table;
        std::vector<std::string> raw;
        std::vector<int> widths;

        //form table head
        ss.precision(1);

        raw.push_back(" ");
        for (int i = 0; i < _m; ++i) {
            ss << "GVM" << i+1;
            raw.push_back(ss.str());
            ss.str(std::string());
        }
        ss.str(std::string());
        table.push_back(raw);
        raw.clear();

        //form table body
        for(float time: _times) {
            ss << time;
            raw.push_back(ss.str());
            ss.str(std::string());

            for (int i = 0; i < _m; ++i) {

                int chosen = _briefcase[time][i].chosen;

                for (int j = 0; j < _briefcase[time][i].numOfDet; ++j) {
                    int temp1 = _briefcase[time][i].details[j];
                    float temp2 = _briefcase[time][i].criterias[j];
                    if(j != 0)
                        ss << " ";
                    if(temp1 == chosen)
                        ss << "*";
                    ss << std::to_string(temp1+1);
                    ss << "(" << std::fixed << temp2 << ")";
                    if(j != _briefcase[time][i].numOfDet - 1)
                        ss << ",";
                }
               // std::string tempStr = ;
                raw.push_back(ss.str());

                ss.str(std::string());
            }
            table.push_back(raw);
            raw.clear();
        }

        //found max width of each column
        for (int i = 0; i < _m+1; ++i) {
            widths.push_back(-1);
        }
        int tempMaxWidth = -1;
        for (auto tempRaw: table) {
            for (int j = 0; j < _m+1; ++j) {
                int tempWidth = tempRaw[j].length();

                if(tempWidth > widths[j])
                    widths[j] = tempWidth;
    /*            if(tempRaw[j].length() > widths[j])
                    widths[j] = static_cast<int>(tempRaw[j].length());*/
            }
        }


        //start to create table
        ss.str(std::string());
        ss << drawLine(widths);
        for (auto words: table) {
            ss << drawRaw(widths,words);
            ss << drawLine(widths);
        }

        return ss.str();
    }

    TechProcess(Matrix<int> tm, Matrix<float> to, int p){
        _tm = tm;
        _to = to;
        _n = tm.getHeight();
        _M = tm.getWidth();
        _m = tm.max();

        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < _M; ++j) {
                _tm[i][j] = _tm[i][j]-1;
            }
        }

        _operationStartTime = Matrix<float>(_n, _M, -1);
        _operationEndTime = Matrix<float>(_n, _M, -1);
        _machineStartTime = Matrix<float>(_n, _m);
        _machineEndTime = Matrix<float>(_n, _m);
        _processState = Matrix<int>(_n, _M);

        _timeBeforeStart = Matrix<float>(_n,1);

        _beforeOperationsDowntimes = Matrix<float>(_m, 1);

        _p = static_cast<unsigned int>(p);
        _initialized = true;

    }

    Matrix<float> getOperationStartTime() {
        return _operationStartTime;
    }

    void findParametersOfProcess() {

        //in map stored vector for each machine
        //in vector stored pairs that represent,
        //first -- num of detail, second -- num of operation
        std::map<int,std::vector<std::pair<int,int>>> yaZaibavcya;
        std::pair<int,int> tempCoord;
        std::vector<std::pair<int,int>> tempVectOfCoords;
        std::map<int,std::vector<float>> startTimes;
        std::map<int,std::vector<float>> endTimes;


        for (int k = 0; k < _m; ++k) {
            yaZaibavcya[k] = std::vector<std::pair<int,int>>();
        }

        //find machines in _tm
        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < _M; ++j) {
                tempCoord.first = i;
                tempCoord.second = j;
                yaZaibavcya[_tm[i][j]].push_back(tempCoord);
            }
        }
        //but now their order as is
        //for each machine operations should be sorted by their start time
        for (int k = 0; k < _m; ++k) {
            for (auto temp: yaZaibavcya[k])
                startTimes[k].push_back(_operationStartTime[temp.first][ temp.second]);
        }

        //output
/*        std::cout << "findDowntime:" << std::endl << std::endl;

        for (int k = 0; k < _m; ++k) {
            std::cout << "Machine " << k+1<< " :"<< std::endl<< "\t";
            for (auto temp: yaZaibavcya[k])
                std::cout << "(" << temp.first <<';' << temp.second << ")\t";

            std::cout << std::endl;
            std::cout << "times :"<< std::endl<< "\t";
            for (auto time: startTimes[k])
                std::cout << time << "\t";

            std::cout << std::endl<< std::endl;
        }*/



        //sort all vectors
        for (int k = 0; k < _m; ++k) {
            //prepare data to sort
            std::vector<std::pair<std::pair<int,int>, float >> zipped;
            zip(yaZaibavcya[k],startTimes[k], zipped);
            std::sort(std::begin(zipped), std::end(zipped),
                      [&](const auto& a, const auto& b)
                      {
                          return a.second <= b.second;
                      });
            unzip(zipped,  yaZaibavcya[k], startTimes[k]);
        }

        //add end times
        for (int k = 0; k < _m; ++k) {
            for (auto temp: yaZaibavcya[k])
                endTimes[k].push_back(_operationEndTime[temp.first][ temp.second]);
        }

        //output

 /*       //results
        std::cout << "after sort:" << std::endl << std::endl;

        for (int k = 0; k < _m; ++k) {
            std::cout << "Machine " << k+1<< " :"<< std::endl<< "\t";
            for (auto temp: yaZaibavcya[k]) {
                std::cout << "(" << temp.first <<';' << temp.second << ")\t";
            }
            std::cout << std::endl;
            std::cout << "times :"<< std::endl<< "\t";
            for (auto time: startTimes[k])
                std::cout << time << "\t";

               std::cout << std::endl;
               std::cout << "end times :"<< std::endl<< "\t";
               for (auto time: endTimes[k])
                   std::cout << time << "\t";

            std::cout << std::endl << std::endl;
        }*/


        for (int k = 0; k < _m; ++k) {
            _beforeOperationsDowntimes[k][0] = startTimes[k][0];
        }

        _betweenOperationsDowntimes = Matrix<float>(_m, 1);
        for (int k = 0; k < _m; ++k) {
            for (int i = 0; i < startTimes[k].size() - 1 ; ++i) {
                _betweenOperationsDowntimes[k][0] = _betweenOperationsDowntimes[k][0] +
                        ( startTimes[k][i+1] - endTimes[k][i])   ;
            }
        }

        _machinesTotalTimes = Matrix<float>(_m, 1);
        for (int k = 0; k < _m; ++k) {
            for (int i = 0; i < startTimes[k].size(); ++i) {

                _machinesTotalTimes[k][0] = _machinesTotalTimes[k][0] +
                        ( endTimes[k][i] - startTimes[k][i]);
            }
        }

        //output
/*        std::cout << "_before Operations Downtimes:" << std::endl << std::endl;
        for (int k = 0; k < _m; ++k) {
            std::cout <<   _beforeOperationsDowntimes[k][0] << '\t';
        }
        std::cout << std::endl;

        std::cout << "_between Operations Downtimes:" << std::endl << std::endl;
        for (int k = 0; k < _m; ++k) {
            std::cout <<   _betweenOperationsDowntimes[k][0] << '\t';
        }
        std::cout << std::endl;

        std::cout << "_machines Total Times:" << std::endl << std::endl;
        for (int k = 0; k < _m; ++k) {
            std::cout <<   _machinesTotalTimes[k][0] << '\t';
        }
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;*/

    }

    void findTimeBeforeStart() {
        for (int i = 0; i < _n; ++i) {
            _timeBeforeStart[i][0] = _operationStartTime.minInRaw(i);
        }
    }

    Matrix<float> getTimeBeforeStart(){ return _timeBeforeStart;}

    Matrix<float> getBeforeOperationsDowntimes(){ return _beforeOperationsDowntimes;}

    Matrix<float> getBetweenOperationsDowntimes(){ return _betweenOperationsDowntimes;}

    Matrix<float> getMachinesTotalTimes(){ return _machinesTotalTimes;}

    float efficiencyCriterion(int critNum) {
        if(critNum == 1)
            return _operationEndTime.max();
        else{
            //calculate all load factors
            Matrix<float> loadFactors(_m, 1);
            for (int k = 0; k < _m; ++k) {
                float temp = _machinesTotalTimes[k][0]/
                             (_machinesTotalTimes[k][0]+_beforeOperationsDowntimes[k][0]
                              +_betweenOperationsDowntimes[k][0]);
                loadFactors[k][0] = temp;
            }
            return loadFactors.min();
        }
    }
};





#endif //LABA2PART2_TECHPROCESS_H
