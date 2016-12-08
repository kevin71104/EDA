#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "Router.h"

using namespace std;

static unsigned num;

bool
myStr2UInt(const string& str, unsigned& num)
{
   num = 0;
   size_t i = 0;
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += unsigned(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   return valid;
}

void
Interval::printInterval() const{
    cout << setw(4) << intervalID << " : " << startTime << ' ' << endTime << ' '
         << trackNum << ' ' << ready << ' ' << done << '\n';
    if(priorList.empty()) return;
    cout << setw(7) << ' ' << intervalID <<"'s priorNodes are:";
    for(size_t i=0; i<priorList.size(); i++)
        cout << ' ' << priorList[i]->getID();
    cout << '\n';
}

bool
Router::readNet(const string& filename)
{
    ifstream ifs(filename.c_str());
    if (!ifs) {
       cerr << "Error: \"" << filename << "\" does not exist!!" << endl;
        return false;
    }
    string line,temp;
    stringstream ss;
    size_t TIME = 0;

    //read first line
    getline(ifs, line);
    ss << line;
    ss >> temp;
    while( !ss.eof() ){
        myStr2UInt(temp,num);
        upper.push_back(num);
        if(num != 0){
            bool flag = false;
            Interval* tempNode;
            //finding same ID
            for(size_t i=0; i<intervalList.size(); i++)
                if(intervalList[i]->getID() == num){
                    flag = true;
                    intervalList[i]->setEnd(TIME);
                    break;
                }
            if(!flag){
                tempNode = new Interval(TIME,TIME);
                intervalList.push_back(tempNode);
                tempNode->setID(num);
            }
        }
        TIME++;
        ss >> temp;
    }

    //reset
    ss.clear();
    ss.str("");
    TIME = 0;
    //read second line
    getline(ifs,line);
    ss << line;
    ss >> temp;
    while( !ss.eof() ){
        myStr2UInt(temp,num);
        bottom.push_back(num);
        if(num != 0){
            bool flag = false;
            Interval* tempNode;
            //finding same ID
            for(size_t i=0; i<intervalList.size(); i++)
                if(intervalList[i]->getID() == num){
                    flag = true;
                    tempNode = intervalList[i];
                    tempNode->setStart(TIME);
                    tempNode->setEnd(TIME);
                    break;
                }
            if(!flag){
                tempNode = new Interval(TIME,TIME);
                tempNode->setID(num);
                intervalList.push_back(tempNode);
            }
            if(upper[TIME] != 0 && !tempNode->priorOrnot(upper[TIME]) )
                tempNode->addPriorNode( getInterval(upper[TIME]) );
        }
        TIME++;
        ss >> temp;
    }
    //sort the intervalList by startTime
    sortStartTime();
    //update the ready & done of the nodes with no priorNodes
    for(size_t i=0; i<intervalList.size(); i++)
        intervalList[i]->update();
    return true;
}

void
Router::routing()
{
    vector<Interval*> list = intervalList;
    size_t trackNum = 0;
    while(!list.empty()){
        trackNum++;
        watermark = 0;
        IdList track;
        while(getTrack(list,track,trackNum)){
            printNet();
            for(size_t i=0; i<list.size(); i++)
                list[i]->update();
        }
        trackList.push_back(track);
    }
}

bool
Router::getTrack(IntervalList& list, IdList& track, size_t trackNum)
{
    //cout << trackNum << ' ';
    bool flag = false;
    for(size_t i=0; i<list.size(); i++)
        if( list[i]->isReady())
            if( watermark == 0 || (list[i]->getStart() > watermark) ){
                flag = true;
                list[i]->setTrack(trackNum);
                list[i]->setDone();
                watermark = list[i]->getEnd();
                track.push_back(list[i]->getID());
                list.erase( list.begin()+i );
                break;
            }
    return flag;
}

void
Router::printTrack() const
{
    for(size_t i=0; i<trackList.size(); i++){
        if(i != 0) cout << '\n';
        cout << "track" << i+1 << ':' ;
        for(size_t j=0; j<trackList[i].size(); j++)
            cout << " i" << trackList[i][j] ;
    }
    cout << '\n';
}
void
Router::printChannelRouting() const
{

}

void
Router::writeTrack(ostream& outfile) const
{
    for(size_t i=0; i<trackList.size(); i++){
        if(i != 0) cout << '\n';
        outfile << "track" << i << ':' ;
        for(size_t j=0; j<trackList[i].size(); j++)
            outfile << " i" << trackList[i][j] ;
    }
}

void
Router::printNet() const
{
    cout << "print net\n";
    for(size_t i=0; i<upper.size(); i++){
        if(i == 0)cout << upper[i];
        else cout << ' ' << upper[i];
    }
    cout << '\n';
    for(size_t i=0; i<bottom.size(); i++){
        if(i == 0)cout << bottom[i];
        else cout << ' ' << bottom[i];
    }
    cout << "\nprint interval... ID: start end tracknum ready done\n";
    for(size_t i=0; i<intervalList.size(); i++){
        intervalList[i]->printInterval();
    }
}
