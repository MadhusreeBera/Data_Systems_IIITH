#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT MATRIX matrix_name
 */
bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Table* matrix = tableCatalogue.getTable(parsedQuery.printRelationName);
    matrix->printMatrix();
    return;
}
