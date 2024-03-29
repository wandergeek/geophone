#ifndef osc_h
#define osc_h
#include "Arduino.h"
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <string>
#include <cstring>

typedef struct LLNode{ 
  unsigned short int _nodeID;
  char* _controllName;
  unsigned short int _numOfVars;
  char _varType;
  float _currentValue;
  float* _controllValueArray;
  unsigned long _timer[3];
  LLNode* nextNode; 
}LLNODE;

class osc
{
  public:
    osc();
    void parseOSCPacket();
    void toggleState();
    void clearBuffer();
    void addControll(char* controllName);
    void addControll(char* controllName, unsigned short int numOfValues);
    void addControll(char* controllName, unsigned short int numOfValues, char valueType);
    void addControll(char* controllName, unsigned short int numOfValues, char valueType, unsigned long timeOut);
    LLNODE* findLast();
    LLNODE* findByID(unsigned short int nodeID);
    float getValue(unsigned short int nodeID);
    void setValue(char* controllName, float valueToSet);
    void generateOSCPacket(char* controllName);
    void clearTXBuffer();
    LLNODE* findByName(char* controllName);
    void deleteNode(unsigned short int nodeID);
    LLNODE* findPrev(unsigned short int nodeID);
	
    LLNODE* startPointer;
    unsigned short int totalNodes;
    
    char* packetBuffer;
    char* txPacketBuffer;
    unsigned short int txPacketBufferLength;
    unsigned short int maxPacketBufferSize;
    unsigned short int currentPacketSize;
    byte isChanged;
    float primaryControllValue;
    unsigned char* floatCharPointer;
    unsigned short int currentControllID;
	  unsigned short int counter;
	  unsigned short int packetIndex;
	
};

#endif
