#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>

using namespace std;

class Interval
{
public:
    Interval(size_t start,size_t end):startTime(start), endTime(end){}
    ~Interval(){}

    //access functions
    size_t getStart(){ return startTime;}
    size_t getEnd(){ return endTime;}

    //setting functions
    void addOverlap(size_t Id) {overlap.push_back(Id);}
    void addPriorNode(size_t Id) {priorNode.push_back(Id);}

private:
    size_t              intervalID;
    size_t              trackNum;
    size_t              startTime;
    size_t              endTime;
    vector< size_t >    overlap;
    vector< size_t >    priorNode;
};

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

    void writeTrack(ostream& outfile) const;
private:
    size_t                    watermark;
    vector< Interval* >       intervalList;
    vector< vector<int> >     trackList;
};

#endif
