#ifndef ROUTER_H
#define ROUTER_H

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
    addOverlap(size_t Id) {overlap.push_back(Id);}
    addPriorNode(size_t Id) {priorNode.push_back(Id);}

private:
    size_t           intervalID;
    size_t           trackNum;
    size_t           startTime;
    size_t           endTime;
    vector<size_t>   overlap;
    vector<size_t>   priorNode;
}

class Router
{
public:
    bool readNet(const string& filename);
    void writeTrack(ostream& outfile);
private:
    size_t watermark;
    vector<Interval*>       IntervalList;
    vector<vector<int>>     trackList;
}

#endif
