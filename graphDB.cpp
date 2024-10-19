#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

const int NODE_RECORD_SIZE = 9;
const int RELATIONSHIP_RECORD_SIZE = 37;

struct NodeRecord {
    int id;
    int firstRel;
    char inUse;

    NodeRecord(int nodeId = -1, int firstRelationship = -1, char usage = 0) {
        id = nodeId;
        firstRel = firstRelationship;
        inUse = usage;
    }
};

struct RelationshipRecord {
    int id;
    int fromNode;
    int toNode;
    int nextRel;
    char inUse;
    char type[20];

    RelationshipRecord(int relId = -1, int from = -1, int to = -1, int next = -1, char usage = 0, const string& relType = "") {
        id = relId;
        fromNode = from;
        toNode = to;
        nextRel = next;
        inUse = usage;
        strncpy(type, relType.c_str(), sizeof(type) - 1); 
        type[sizeof(type) - 1] = '\0';
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
        nodeFile.open(nodeFilename, ios::in | ios::out | ios::binary);
        if (!nodeFile) {
            nodeFile.open(nodeFilename, ios::out | ios::binary);
            nodeFile.close();
            nodeFile.open(nodeFilename, ios::in | ios::out | ios::binary);
        }

        relFile.open(relFilename, ios::in | ios::out | ios::binary);
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
        NodeRecord node(nextNodeId, -1, 1);
        int offset = nextNodeId * NODE_RECORD_SIZE;

        nodeFile.seekp(offset, ios::beg);
        nodeFile.write(reinterpret_cast<char*>(&node), sizeof(NodeRecord));
        return nextNodeId++;
    }

    void addRelationship(int fromNodeId, int toNodeId, const string& type) {
        int fromNodeOffset = fromNodeId * NODE_RECORD_SIZE;
        RelationshipRecord rel(nextRelId, fromNodeId, toNodeId, -1, 1, type);
        int relOffset = nextRelId * RELATIONSHIP_RECORD_SIZE;
        relFile.seekp(relOffset, ios::beg);
        relFile.write(reinterpret_cast<char*>(&rel), sizeof(RelationshipRecord));

        nodeFile.seekg(fromNodeOffset, ios::beg);
        NodeRecord fromNode;
        nodeFile.read(reinterpret_cast<char*>(&fromNode), sizeof(NodeRecord));
        if (fromNode.firstRel == -1) {
            fromNode.firstRel = nextRelId;
        } else {
            int curRelId = fromNode.firstRel;
            while (curRelId != -1) {
                relFile.seekg(curRelId * RELATIONSHIP_RECORD_SIZE, ios::beg);
                RelationshipRecord curRel;
                relFile.read(reinterpret_cast<char*>(&curRel), sizeof(RelationshipRecord));

                if (curRel.nextRel == -1) {
                    curRel.nextRel = nextRelId;
                    relFile.seekp(curRelId * RELATIONSHIP_RECORD_SIZE, ios::beg);
                    relFile.write(reinterpret_cast<char*>(&curRel), sizeof(RelationshipRecord));
                    break;
                }
                curRelId = curRel.nextRel;
            }
        }
        nodeFile.seekp(fromNodeOffset, ios::beg);
        nodeFile.write(reinterpret_cast<char*>(&fromNode), sizeof(NodeRecord));
        nextRelId++;
    }

    void traverseFromNode(int nodeId) {
        int nodeOffset = nodeId * NODE_RECORD_SIZE;
        nodeFile.seekg(nodeOffset, ios::beg);
        NodeRecord node;
        nodeFile.read(reinterpret_cast<char*>(&node), sizeof(NodeRecord));

        if (node.inUse) {
            cout << "Traversing from Node ID: " << nodeId << endl;

            int relId = node.firstRel;
            while (relId != -1) {
                int relOffset = relId * RELATIONSHIP_RECORD_SIZE;
                relFile.seekg(relOffset, ios::beg);

                RelationshipRecord rel;
                relFile.read(reinterpret_cast<char*>(&rel), sizeof(RelationshipRecord));

                if (rel.inUse) {
                    cout << "Node " << nodeId << " is connected to Node " << rel.toNode << " via relationship type: " << rel.type << endl;
                }

                relId = rel.nextRel;
            }
        } else {
            cout << "Node ID " << nodeId << " not found or not in use." << endl;
        }
    }

    NodeRecord readNode(int nodeId) {
        NodeRecord node;
        int nodeOffset = nodeId * NODE_RECORD_SIZE;
        nodeFile.seekg(nodeOffset, ios::beg);
        nodeFile.read(reinterpret_cast<char*>(&node), sizeof(NodeRecord));
        return node;
    }

    RelationshipRecord readRelationship(int relId) {
        RelationshipRecord rel;
        int relOffset = relId * RELATIONSHIP_RECORD_SIZE;
        relFile.seekg(relOffset, ios::beg);
        relFile.read(reinterpret_cast<char*>(&rel), sizeof(RelationshipRecord));
        return rel;
    }

    void updateNode(int nodeId, NodeRecord updatedNode) {
        int nodeOffset = nodeId * NODE_RECORD_SIZE;
        nodeFile.seekp(nodeOffset, ios::beg);
        nodeFile.write(reinterpret_cast<char*>(&updatedNode), sizeof(NodeRecord));
    }

    void updateRelationship(int relId, RelationshipRecord updatedRel) {
        int relOffset = relId * RELATIONSHIP_RECORD_SIZE;
        relFile.seekp(relOffset, ios::beg);
        relFile.write(reinterpret_cast<char*>(&updatedRel), sizeof(RelationshipRecord));
    }

    void deleteNode(int nodeId) {
        NodeRecord node = readNode(nodeId);
        node.inUse = 0;
        updateNode(nodeId, node);
    }

    void deleteRelationship(int relId) {
        RelationshipRecord rel = readRelationship(relId);
        rel.inUse = 0;
        updateRelationship(relId, rel);
    }


    void traverseByType(int nodeId, const string& typeFilter) {
    int nodeOffset = nodeId * NODE_RECORD_SIZE;
    nodeFile.seekg(nodeOffset, ios::beg);
    NodeRecord node;
    nodeFile.read(reinterpret_cast<char*>(&node), sizeof(NodeRecord));

    if (node.inUse) {
        cout << "Traversing from Node ID: " << nodeId << " with relationship type: " << typeFilter << endl;

        int relId = node.firstRel;
        while (relId != -1) {
            int relOffset = relId * RELATIONSHIP_RECORD_SIZE;
            relFile.seekg(relOffset, ios::beg);

            RelationshipRecord rel;
            relFile.read(reinterpret_cast<char*>(&rel), sizeof(RelationshipRecord));

            if (rel.inUse && strcmp(rel.type, typeFilter.c_str()) == 0) {
                cout << "Node " << nodeId << " is connected to Node " << rel.toNode << " via relationship type: " << rel.type << endl;
            }

            relId = rel.nextRel;
        }
    } else {
        cout << "Node ID " << nodeId << " not found or not in use." << endl;
    }
}

    
};

int main() {
    GraphDB graph("nodes.db", "relationships.db");

    int nodeA = graph.addNode();
    int nodeB = graph.addNode();
    int nodeC = graph.addNode();

    graph.addRelationship(nodeA, nodeB, "WORKS_FOR");
    graph.addRelationship(nodeA, nodeC, "WORKS_FOR");

    graph.traverseFromNode(nodeA);

    return 0;
}
