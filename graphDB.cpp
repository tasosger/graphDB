#include <fstream>
#include <vector>
#include <cstring>


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