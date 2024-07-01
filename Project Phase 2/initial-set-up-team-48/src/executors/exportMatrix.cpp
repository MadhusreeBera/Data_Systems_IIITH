#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT MATRIX <matrix_name> 
 */

bool syntacticParseEXPORTMATRIX()
{
    logger.log("syntacticParseEXPORTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORTMATRIX;
    parsedQuery.exportRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORTMATRIX()
{
    logger.log("semanticParseEXPORTMATRIX");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such matrix exists" << endl;
    return false;
}

void executeEXPORTMATRIX()
{
    logger.log("executeEXPORTMATRIX");
    Table* matrix = tableCatalogue.getTable(parsedQuery.exportRelationName);
    matrix->exportMatrix();
    return;
}