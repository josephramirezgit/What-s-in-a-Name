// JOSEPH RAMIREZ
// NameTable.cpp

// This is a correct but inefficient implementation of
// the NameTable functionality.
#include "NameTable.h"
#include <string>
#include <vector>
#include <functional>
#include <list>
#include <stack>
#include <iostream>
using namespace std;

  // This class does the real work of the implementation.

class NameTableImpl
{
  public:
    /*
     Whenever a scope is entered, the enterScope member function is called.
     */
    void enterScope();

    /*
     Whenever a scope is exited, the exitScope member function is called. It returns true unless there have been more calls to exitScope than prior calls to enterScope. (I.e., you can't leave a scope that hasn't been entered.)
     */
    bool exitScope();

    /*
     Whenever an identifier is declared, the declare member function is called to record the line number associated with that declaration. It returns true unless that identifier has already been declared in the current scope or that so-called identifier is the empty string, in which cases it returns false without changing the name table. We'll consider any non-empty string to be a valid identifier.
     */
    bool declare(const string& id, int lineNum);

    /*
     Whenever an identifier is used, the find member function is called to determine the line number of the declaration corresponding to that use. It returns that line number, or -1 if the identifier has no active declaration.
     */
    int find(const string& id) const;
  private:
    // struct identifier allows for holding info
    struct Identifier
    {
        string id;
        int lineNum;
        int scope;
    };

    // Make the hash table
    list<Identifier> hashTable[19997];

    // Make a function to return a bucket number
    unsigned int bucketNumber(const string& id) const;

    // Hold a stack to save where we are in the scope
    stack<int> scopes;

    // Have a vector to save where you are in the scope
    vector<string> scopeContains;
};

unsigned int NameTableImpl::bucketNumber(const string& id) const
{
    // create a hash function to determine the hash bucket
    std::hash<std::string> giveBucketNumber;

    // return whatever the hash function returns and mod by the largest prime number before 20000
    // 19997 is the size of this hash table
    return giveBucketNumber(id) % 19997;
}

void NameTableImpl::enterScope()
{
    // Entering the scope will result in pushing the empty string to denote entering a new scope
    scopeContains.push_back("");

    // Update the scope size
    scopes.push(static_cast<int>(scopes.size()));
}

bool NameTableImpl::exitScope()
{
    // start at the end of the vector that has all the materials in it
    auto endOfVector = scopeContains.end() - 1;
    
    // until you reach the end of the scope or until it is empty
    while (!scopeContains.empty() && endOfVector != scopeContains.begin() && *endOfVector != "")
    {
        // find the right place where you hash
        unsigned int bucket = bucketNumber(*endOfVector);

        // get rid of that value in the hash table
        for (auto del = hashTable[bucket].begin(); del != hashTable[bucket].end(); del++)
        {
            // make sure it is the right value you want to get rid of
            if (del->id == *endOfVector && del->scope == scopes.size())
            {
                // get rid of that value
                del = hashTable[bucket].erase(del);
                break;
            }
        }

        // now that you got rid of the end of the vector in the hash table, get rid of it in the scopeContains vector
        scopeContains.pop_back();
        
        // fix where endOfVector is pointing
        endOfVector--;
    }

    // get rid of the empty string at the end
    scopeContains.pop_back();

    // Make sure you update the size
    if (scopes.size() != 0)
    {
        scopes.pop();
    }

    // you cannot call this if it is empty
    // prevent excessive calls
    if (scopeContains.empty())
    {
        return false;
    }

    else
    {
        return true;
    }

}

bool NameTableImpl::declare(const string& id, int lineNum)
{

    // You cannot have the empty string as an ID
    if (id.empty())
        return false;

    // Compute a bucket number with a hash function
    unsigned int bucket = bucketNumber(id);

    // Now find if there are any repeats
    // begin searching for the right Identifer struct
    auto searchIter = hashTable[bucket].begin();

    // if it is ever in the list, then return false
    if (hashTable[bucket].size() != 0)
    {
        for ( ;searchIter != hashTable[bucket].end(); searchIter++)
        {
            // return false if the value was already found
            if (searchIter->id == id && searchIter->scope == scopes.size())
            {
                return false;
            }
        }
    }

    // Add the new value at the bucket number
    // First make the node you want to add
    Identifier newIdentifier;
    newIdentifier.id = id;
    newIdentifier.lineNum = lineNum;
    newIdentifier.scope = static_cast<int>(scopes.size());

    // Put the value at that new bucket number
    hashTable[bucket].push_back(newIdentifier);

    // Anytime you add to the scope, keep note of that in the vector
    scopeContains.push_back(id);
    
    // this means you were able to add the node in
    return true;
}

int NameTableImpl::find(const string& id) const
{

    // if the string is empty, then return -1
    if (id.empty())
        return -1;

    // First find the bucket at which the id would be at
    unsigned int bucket = bucketNumber(id);

    // begin searching for the right Identifer struct
    // start at the end of hash table since it will likley be faster
    auto searchIter = hashTable[bucket].rbegin();
    
    for ( ;searchIter != hashTable[bucket].rend(); searchIter++)
    {
        // if the ID macthes, then return that line number
        if (searchIter->id == id)
        {
            return searchIter->lineNum;
        }
    }
    
    // if you can make it to the end, then return -1 because that means you never found it
    return -1;
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
