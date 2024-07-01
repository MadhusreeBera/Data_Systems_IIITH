#include "global.h"
/**
 * @brief
 * SYNTAX:
 */

bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeMatrix = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("bool semanticParseCOMPUTE()");

    if (!tableCatalogue.isTable(parsedQuery.computeMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Table* matrix = tableCatalogue.getTable(parsedQuery.computeMatrix);
    matrix->computeMatrix(parsedQuery.computeMatrix);
    cout<<"Matrix is computed";
    return;
}
