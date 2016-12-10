#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
#include <climits>
#include <algorithm>

using namespace std;

class Interval;
class Router;

typedef vector<Interval*>		IntervalList;
typedef vector<unsigned>		IdList;

class Interval
{
public:
    Interval(size_t start = UINT_MAX,size_t end = 0)
        :ready(false), done(false), startTime(start), endTime(end){}
    ~Interval(){}

    //setting functions
    void setStart(size_t Time) { if(Time < startTime) startTime = Time; }
    void setEnd(size_t Time) { if( Time > endTime) endTime = Time; }
    void setID(size_t ID) { intervalID = ID;}
    void setTrack(size_t num) { trackNum = num;}
    void setDone() { done = true;}
    void addTimeList(size_t times) {timeList.push_back(times);}
    void addPriorNode(Interval* priorNode) {priorList.push_back(priorNode);}
    void update() {
        ready = true;
        if(priorList.empty())
            return;
        for(size_t i=0;i<priorList.size();i++)
            if(!priorList[i]->isDone()){
                ready = false;
                break;
        }
    }

    //access functions
    size_t getStart() const{ return startTime;}
    size_t getEnd() const{ return endTime;}
    size_t getID() const { return intervalID;}
    size_t getTrackNum() const { return trackNum;}
    const vector<size_t>& getTimeList() const { return timeList;}

    //bool functions
    bool isReady() const { return ready;}
    bool isDone() const { return done;}
    bool priorOrnot(size_t ID) const {
        bool flag =false;
        for(size_t i=0;i<priorList.size();i++)
            if(priorList[i]->getID() == ID){
                flag = true;
                break;
            }
        return flag;
    }

    void printInterval() const;
    void sortTimeList(){ ::sort(timeList.begin(),timeList.end());}

private:
    mutable bool            ready;
    mutable bool            done;
    size_t                  intervalID;
    size_t                  trackNum;
    size_t                  startTime;
    size_t                  endTime;
    IntervalList            priorList;
    vector<size_t>          timeList;       //which time it occurs time*2 means upper time*2+1 means bottom
};

//functional object
struct less_start
{
   bool operator() (const Interval* I1, const Interval* I2) const{
       return (I1->getStart() < I2->getStart());
   }
};

class Router
{
public:
    Router():watermark(0){}
    ~Router(){ resetList();}

    //reporting functions
    void printChannelRouting(ostream& outfile = cout) const;
    void printNet() const;
    void printTrack(ostream& outfile = cout) const;

    //access functions
    Interval* getInterval(size_t ID) const{
        for(size_t i=0; i<intervalList.size(); i++)
            if(ID == intervalList[i]->getID())
                return intervalList[i];
    }

    //Constrained Left-Edge algorithm
    void routing();
    
    bool readNet(const string& filename) ;
    void resetList() {
        for(size_t i=0; i < intervalList.size(); i++)
            delete intervalList[i];
    }

private:
    size_t                  watermark;
    IdList                  upper;
    IdList                  bottom;
    IntervalList            intervalList;
    vector<IntervalList>    trackList;
};

#endif
