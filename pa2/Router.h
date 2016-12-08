#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

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
    void addPriorNode(Interval* priorNode) {priorList.push_back(priorNode);}
    void update() {
        ready =true;
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

    //reporting functions
    void printInterval() const;

private:
    bool                    ready;
    bool                    done;
    size_t                  intervalID;
    size_t                  trackNum;
    size_t                  startTime;
    size_t                  endTime;
    vector< Interval* >     priorList;
};

//functional object
struct less_start
{
   bool operator() (const Interval* I1, const Interval* I2) const{
       return (I1->getStart() < I2->getStart());
   }
};

/*struct less_ID
{
    bool operator() (const Interval* I1, const Interval* I2) const{
        return (I1->getID() < I2->getID());
    }
};*/

class Router
{
public:
    Router():watermark(0){}

    bool readNet(const string& filename) ;
    void resetList() {
        for(size_t i=0; i < intervalList.size(); i++)
            delete intervalList[i];
    }

    //reporting functions
    void printTrack() const;
    void printChannelRouting() const;
    void printNet() const;

    //access functions
    Interval* getInterval(size_t ID) const{
        for(size_t i=0; i<intervalList.size(); i++)
            if(ID == intervalList[i]->getID())
                return intervalList[i];
    }

    void sortStartTime(){ ::sort(intervalList.begin(), intervalList.end(), less_start());}
    void writeTrack(ostream& outfile) const;

    vector< Interval* >         intervalList;
private:
    size_t                      watermark;
    vector< size_t >            upper;
    vector< size_t >            bottom;
    vector< vector<size_t> >    trackList;
};

#endif
