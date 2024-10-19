#include <fstream>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

const int NODE_RECORD_SIZE  = 9;
const int RELATIONSHIP_RECORD_SIZE = 17;


struct NodeRecord {
    int id;
    int firstRel;
    char inUse;

    NodeRecord(int nodeId = -1, int firstRelationship = -1, char usage = 0) {
        id = nodeId;
        firstRelationship = firstRelationship;
        inUse = usage;
    }
};


struct RelationshipRecord {
    int id;
    int fromNode;
    int toNode;
    char inUse;
    int nextRel;

    RelationshipRecord(int relId = -1, int from = -1 , int to= -1, int next = -1, char usage = 0 ) {
        id = relId;
        fromNode = from;
        toNode = next;
        nextRel = next;
        inUse = usage;
    }
};


class GraphDB {
private:
    fstream nodeFile;
    fstream relFile;
    int nextNodeId = 0;
    int nextRelId = 0;

public:
    GraphDB(const string& nodeFilename, const string& relFilename) {
        nodeFile.open(nodeFilename, ios::in | ios::out | ios::binary );
        if (!nodeFile) {
            nodeFile.open(nodeFilename, ios::out | ios::binary);
            nodeFile.close();
            nodeFile.open(nodeFilename, ios::in | ios::out | ios::binary);
        }

        relFile.open(relFilename, ios::in | ios::out | ios::binary );
        if (!relFile) {
            relFile.open(relFilename, ios::out | ios::binary);
            relFile.close();
            relFile.open(relFilename, ios::in | ios::out | ios::binary);
        }
    }

     ~GraphDB() {
        if (nodeFile.is_open()) nodeFile.close();
        if (relFile.is_open()) relFile.close();
    }


    int addNode() {
        NodeRecord node(nextNodeId, -1, -1);
        int offset = nextNodeId * NODE_RECORD_SIZE;

        nodeFile.seekp(offset, ios::beg);
        nodeFile.write(reinterpret_cast<char*>(&node), sizeof(NodeRecord));
        return nextNodeId++;
    }
};


int main() {

}