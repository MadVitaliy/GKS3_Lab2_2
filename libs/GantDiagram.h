//
// Created by Vitalik on 22.03.2021.
//

#ifndef LABA2_GANTDIAGRAM_H
#define LABA2_GANTDIAGRAM_H

//just to poviyobuvatsa
#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <map>
#include <algorithm>
#include "ZipUnzip.h"

#include "Matrix.h"

enum TextColor {
    N_BLUE = 1,
    N_GREEN = 2,
    N_CYAN = 3,
    N_RED = 4,
    N_PURPLE = 5,
    N_YELLOW = 6,
    N_DEFAULT_WHITE = 7,
    N_GRAY = 8,
    N_BRIGHT_BLUE = 9,
    N_BRIGHT_GREEN = 10,
    N_BRIGHT_CYAN = 11,
    N_BRIGHT_RED = 12,
    N_PINK = 13,
    N_BRIGHT_YELLOW = 14,
    N_BRIGHT_WHITE = 15
};

#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define PURPLE 5
#define YELLOW 6
#define DEFAULT_WHITE 7
#define GRAY 8
#define BRIGHT_BLUE 9
#define BRIGHT_GREEN 10
#define BRIGHT_CYAN 11
#define BRIGHT_RED 12
#define PINK 13
#define BRIGHT_YELLOW 14
#define BRIGHT_WHITE 15

void resetConsoleTextColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TextColor(N_DEFAULT_WHITE));
}
/*
void textColor(TextColor textColor = TextColor(N_DEFAULT_WHITE)){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}*/

void textColor(int color = DEFAULT_WHITE){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}


void printColorString(std::string str, int color = DEFAULT_WHITE) {
    textColor(color);
    std::cout << str << std::endl;
    resetConsoleTextColor();
}

void setCursor( COORD c) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setCursor(int x, int y) {
    COORD c;
    c.X = static_cast<short>(x);
    c.Y = static_cast<short>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}


COORD drawAxis(COORD curPos, int yAxisLength, int xAxisLength) {
    COORD zeroPos;
    //draw Y narrow
    curPos.Y = curPos.Y+1;
    //curPos.X = 0;
    setCursor(curPos);
    putch('^');

    for (int i = 1; i < yAxisLength - 1; ++i) {
        curPos.Y = curPos.Y+1;
        setCursor(curPos);
        putch('|');
    }
    curPos.Y = curPos.Y+1;
    setCursor(curPos);
    putch('0');
    zeroPos = curPos;
    for (int i = 0; i < xAxisLength - 1; ++i) {
        curPos.X = curPos.X+1;
        setCursor(curPos);
        putch('-');
    }
    curPos.X = curPos.X+1;
    setCursor(curPos);
    putch('>');
    return zeroPos;
}

void drawLine(COORD from, unsigned int length, char symbol = '#'){
    setCursor(from);
    for (int i = 0; i < length; ++i) {
        putchar(symbol);
    }

}

void writeWord(COORD from, std::string str) {
    setCursor(from);
    for(char symbol: str){
        putchar(symbol);
    }
}

void writeVWord(COORD from, std::string str) {
    static int color = 0;
    color = (color + 1)%2;
    if(color == 0) {
        textColor(BRIGHT_WHITE);
    } else {
        textColor(BRIGHT_YELLOW);
    }

    for (char symbol: str) {
        setCursor(from);
        putchar(symbol);
        from.Y = from.Y + static_cast<short>(1);
    }
    textColor();
}


void drawGantDiagram(Matrix<int> start, Matrix<int> end) {
    if(start.getHeight() != end.getHeight() || start.getWidth() != end.getWidth()) {

        return;
    }
    unsigned  int n = start.getHeight();
    unsigned  int m = start.getWidth();
    int scale = 2;
    int yAxisLength = m*scale+scale;
    int xAxisLength = (end.max()+1)*scale;

    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbsi)) {
        std::cout << "ass";
        return;
    }

    COORD pos = cbsi.dwCursorPosition;
    pos.X = pos.X + 5;
    COORD zeroPos = drawAxis(pos, yAxisLength, xAxisLength);


    int length;


    //sign oy
    pos.X = 0;
    for (int j = 0; j < m; ++j) {
        pos.Y = zeroPos.Y - static_cast<short>(m - j*scale+scale);
        writeWord(pos, "GVM" + std::to_string(j+1));
    }
    //sign ox

    for (int i = 1; i*scale < xAxisLength; i++) {
        pos.Y = zeroPos.Y + static_cast<short>(1);
        pos.X = zeroPos.X + static_cast<short>(i*scale);
        writeVWord(pos, std::to_string(i));
    }



    //draw diagram lines
    for (int i = 0; i < n; ++i) {
        textColor(i+9);
        for (int j = 0; j < m; ++j) {
            length = (end[i][j] - start[i][j])*scale;
            pos.X = zeroPos.X + static_cast<short>(scale*start[i][j]+1);
            pos.Y = zeroPos.Y - static_cast<short>(m - j*scale+scale);
            drawLine(pos, length, i+48+1);
        }
    }
    textColor();

    pos.X = 0;
    pos.Y = zeroPos.Y + 3;
    setCursor(pos);
}





void uniqueElementInVector(std::vector<float>& v, Matrix<float> st){
    int n = st.getHeight();
    int m = st.getWidth();
    for (int i = 0; i < n*m; ++i) {
        float temp = st[0][i];
        if (std::find(v.begin(), v.end(), temp) == v.end())
            v.push_back(st[0][i]);
    }
}

std::string to_string(float num, int precision) {
    std::stringstream ss;
    ss.precision(precision);
    ss << std::fixed << num;
    return ss.str();
}

void gap(std::stringstream& ss, int len, char sumb = ' ') {
    for (int j = 0; j < len; ++j)
        ss<<sumb;
}

std::string drawGantDiagram(Matrix<int> tm, Matrix<float> to, Matrix<float> st){

    std::stringstream ss;
    ss.precision(1);
    int numOfMachine = tm.max();
    int numOfDetails = tm.getHeight();
    int numOfOperation = tm.getWidth();
    int scale = 2;

    std::map<int, std::vector<Matrix<int>>> operationOnMachines;
    //std::vector<Matrix<int>> operationOnMachine;

    for (int i = 0; i < numOfDetails; ++i)
        for (int j = 0; j < numOfOperation; ++j)
            tm[i][j] = tm[i][j]-1;


    for (int k = 0; k < numOfMachine; ++k)
        operationOnMachines[k] = std::vector<Matrix<int>>();


    Matrix<int> index(1,2);
    for (int i = 0; i < numOfDetails; ++i) {
        for (int j = 0; j < numOfOperation; ++j) {
            index[0][0] = i;
            index[0][1] = j;
            operationOnMachines[tm[i][j]].push_back(index);
        }
    }

/*    std::cout << "Lists of indexes:" << std::endl;
    for (int k = 0; k < numOfMachine; ++k) {
        for(Matrix<int> ind: operationOnMachines[k]) {
            std::cout << '(' << ind[0][0] << ';' << ind[0][1] << ')' << ' ';
        }
        std::cout << std::endl;
    }*/




    for (int k = 0; k < numOfMachine; ++k){
        std::vector<Matrix<int>> indexes = operationOnMachines[k];
        std::vector<float> times;
        for (Matrix<int> ind: indexes){
            times.push_back(st[ind[0][0]][ind[0][1]]);
        }

        std::vector<std::pair<Matrix<int>, float >> zipped;

        zip(indexes,times, zipped);
        // Sort the vector of pairs
        std::sort(std::begin(zipped), std::end(zipped),
                  [&](const auto& a, const auto& b)
                  {
                      return a.second <= b.second;
                  });
        // Write the sorted pairs back to the original vectors
        unzip(zipped,  indexes, times);
        operationOnMachines[k] = indexes;
    }

    Matrix<float> et = st+to;
    float endTime = et.max();


    //std::cout << "Start:" << std::endl << st.str() << std::endl;
    //std::cout << "Duration:" << std::endl << to.str() << std::endl;



    std::string labels = "GVM" + std::to_string(numOfMachine);
    int maxLabelLength = static_cast<int>(labels.length());
    //it seem i can start to draw diagram;
    maxLabelLength += 1; //add offset to axis;

    gap(ss,maxLabelLength);
    ss << '^'<< '\n' ;
    gap(ss,maxLabelLength);
    ss << '|' << '\n';

    float currPos;
    for (int k = 0; k < numOfMachine; ++k) {
        std::string tempStr = "GVM" + std::to_string(k+1);
        ss << tempStr ;
        gap(ss,maxLabelLength-tempStr.length());
        ss << '|';
        currPos = 0;
        for(Matrix<int> ind: operationOnMachines[k]) {

            int raw = ind[0][0];//detail
            int col = ind[0][1];//operation

            float delta = st[raw][col]*scale - currPos;
            for (int i = 0; i < delta; ++i) {
                ss << ' ';
            }
            float line = to[raw][col]*scale;
            for (int i = 0; i < line; ++i) {
                ss << raw+1;
            }
            currPos = et[raw][col]*scale;
        }

        ss << '\n';
        gap(ss,maxLabelLength);
        if(k != numOfMachine -1 )
            ss << '|' << '\n';
        else
            ss << '+';

    }
    for (int j = 0; j < endTime*scale+scale; ++j) {
        ss << '-';
    }
    ss << '>' << '\n';

    //create vector that contained times for ordinat axis
    std::vector<float> times;
    std::map<float,std::string> strTimes;
    //std::vector<std::string> strTimes;
    uniqueElementInVector(times, st);
    uniqueElementInVector(times, et);

    std::sort(times.begin(),times.end());

    for (float time: times) {
        strTimes[time] = to_string(time,0);
    }

    int maxTimeLength = 0;
    for (float time: times) {
        if(strTimes[time].length() > maxTimeLength)
            maxTimeLength = static_cast<int>(strTimes[time].length());
    }




    for (int i = 0; i < maxTimeLength; ++i) {
        gap(ss,maxLabelLength);
        currPos = 0;
        for (float time: times) {
            float delta = time*scale - currPos;

            for (int j = 0; j < delta; ++j) {
                ss << ' ';
            }
            if(strTimes[time].length() > i)
                ss << strTimes[time][i];
            else
                ss << ' ';

            currPos = time*scale + 1;

        }
        ss << '\n';
    }

    ss << '\n' << '\t' << "Scale: " << scale << " symb -- 1 time unit" << '\n';

    return ss.str();
}


#endif //LABA2_GANTDIAGRAM_H
