#include <iostream>
#include "Router.h"

using namespace std;

bool
Router::readNet(const string& filename)
{
    return true;
}

void
Router::printTrack() const
{   //cout<<123;
    for(size_t i=0; i<trackList.size(); i++){
        if(i != 0) cout << '\n';
        cout << "track" << i << ':' ;
        for(size_t j=0; j<trackList[i].size(); j++)
            cout << " i" << trackList[i][j] ;
    }
}
void
Router::printChannelRouting() const
{

}

void
Router::writeTrack(ostream& outfile) const
{

}
