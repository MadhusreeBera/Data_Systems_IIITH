#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME matrix name with another name
 */
bool syntacticParseRENAMEMATRIX()
{
    logger.log("syntacticParseRENAME");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAMEMATRIX;
    parsedQuery.renameFromRelationName = tokenizedQuery[2];
    parsedQuery.renameToRelationName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAMEMATRIX()
{
    logger.log("semanticParseRENAMEMATRIX");

    if (!tableCatalogue.isTable(parsedQuery.renameFromRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }

    if (tableCatalogue.isTable(parsedQuery.renameToRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix name already exist" << endl;
        return false;
    }
    return true;
}

void executeRENAMEMATRIX()
{
    logger.log("executeRENAMEMATRIX");
    Table* matrix = tableCatalogue.getTable(parsedQuery.renameFromRelationName);
    matrix->renameMatrix(parsedQuery.renameFromRelationName, parsedQuery.renameToRelationName);
    return;
}