#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
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
         << trackNum << ' ' ;

     cout << "\nit occurs at:";
     for(size_t i=0; i<timeList.size();i++)
        cout << ' ' << (timeList[i]>>1);
    if(priorList.empty()){
        cout << '\n';
        return;
    }
    cout << "\npriorNodes are:";
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
    while( !ss.eof() ){
        ss >> temp;
        if(temp == "")break;
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
                    intervalList[i]->addTimeList(2*TIME);
                    break;
                }
            if(!flag){
                tempNode = new Interval(TIME,TIME);
                intervalList.push_back(tempNode);
                tempNode->addTimeList(2*TIME);
                tempNode->setID(num);
            }
        }
        TIME++;
        temp = "";
    }
    size_t num_upper = intervalList.size();

    //reset
    ss.clear();
    ss.str("");
    TIME = 0;
    //read second line
    getline(ifs,line);
    ss << line;
    while( !ss.eof() ){
        ss >> temp;
        if(temp == "")break;
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
                    tempNode->addTimeList(2*TIME+1);
                    break;
                }
            if(!flag){
                tempNode = new Interval(TIME,TIME);
                tempNode->setID(num);
                tempNode->addTimeList(2*TIME+1);
                intervalList.push_back(tempNode);
            }
            if(upper[TIME] != 0 && !tempNode->priorOrnot(upper[TIME]) )
                tempNode->addPriorNode( getInterval(upper[TIME]) );
        }
        TIME++;
        temp = "";
    }
    //sort the intervalList by startTime
    ::sort(intervalList.begin(), intervalList.end(), less_start());
    //sort timeList
    for(size_t i=0; i<intervalList.size();i++)
        intervalList[i]->sortTimeList();
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
        IntervalList track;
        for(size_t i=0; i<list.size(); i++){
            list[i]->update();
            if( list[i]->isReady())
                if( watermark == 0 || (list[i]->getStart() > watermark) ){
                    list[i]->setTrack(trackNum);
                    list[i]->setDone();
                    watermark = list[i]->getEnd();
                    track.push_back(list[i]);
                    list.erase( list.begin()+i );
                    i--;    //keep searching start from the original next interval
                }
        }
        trackList.push_back(track);
    }
}

void
Router::printChannelRouting(ostream& outfile) const
{
    outfile << endl ;
    outfile << "Channel Routing Graph: \n";
    size_t routing_time = upper.size();
    //upper-boundary
    for(size_t i=0; i<upper.size(); i++){
        if(i == 0) outfile << upper[i];
        else outfile << "  " << upper[i] ;
    }
    outfile << endl;
    for(size_t i=0; i<2*upper.size()-1; i++){
        if(i%2) outfile << "--";
        else outfile << '*';
    }
    outfile << endl;

    //routing
    vector< string > tracksymbol;
    tracksymbol.reserve(trackList.size());
    tracksymbol.resize(trackList.size());
    for(size_t i=0;i<tracksymbol.size();i++){
        tracksymbol[i].insert(0,3*routing_time-2,' ');
    }
    //insert lines as '_'
    for(size_t i=0; i<trackList.size();i++){
        size_t start;
        size_t end;
        for(size_t j=0; j<trackList[i].size(); j++){
            start = trackList[i][j]->getStart();
            end = trackList[i][j]->getEnd();
            for(size_t k= 3*start+1;k<3*end;k++) tracksymbol[i][k] = '_';
        }
    }
    //insert start and end as '|'
    for(size_t i=0; i<intervalList.size(); i++){
        size_t trackNum = intervalList[i]->getTrackNum()-1;
        vector<size_t> list = intervalList[i]->getTimeList();
        for(size_t j=0; j < list.size(); j++){
            //appears at bottom
            bool bottom_or_not = list[j]%2;
            size_t times = list[j] >> 1;
            if(bottom_or_not)
                for(size_t k=trackNum+1; k<tracksymbol.size(); k++) tracksymbol[k][3*times] = '|';
            else
                for(size_t k=0; k <= trackNum; k++) tracksymbol[k][3*times] = '|';
        }
    }

    //print-routing
    for(size_t i=0; i<tracksymbol.size();i++){
        outfile << tracksymbol[i] << '\n';
    }
    //lower-boundary
    for(size_t i=0; i<bottom.size(); i++){
        if(i != 0) outfile << "  ";
        if(bottom[i] != 0) outfile << '|';
        else outfile << " ";
    }
    outfile << endl;
    for(size_t i=0; i<2*bottom.size()-1; i++){
        if(i%2) outfile << "--";
        else outfile << '*';
    }
    outfile << endl;
    for(size_t i=0; i<bottom.size(); i++){
        if(i == 0) outfile << bottom[i];
        else outfile << "  " << bottom[i] ;
    }
    outfile << endl;
}

void
Router::printTrack(ostream& outfile) const
{
    //outfile << "Routing Results: \n";
    for(size_t i=0; i<trackList.size(); i++){
        if(i != 0) outfile << '\n' ;
        outfile << "Track" << i+1 << ':' ;
        for(size_t j=0; j<trackList[i].size(); j++)
            outfile << " i" << trackList[i][j]->getID() ;
    }
    outfile << endl ;
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
    cout << "\nprint interval... ID: start end tracknum\n";
    for(size_t i=0; i<intervalList.size(); i++){
        intervalList[i]->printInterval();
    }
}
