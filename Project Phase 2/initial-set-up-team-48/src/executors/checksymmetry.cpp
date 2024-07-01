#include "global.h"
/**
 * @brief
 * SYNTAX:
 */

bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checksymmetryMatrix = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");

    if (!tableCatalogue.isTable(parsedQuery.checksymmetryMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Table* matrix = tableCatalogue.getTable(parsedQuery.checksymmetryMatrix);
    if(matrix->checkSymMatrix(parsedQuery.checksymmetryMatrix))
        cout<<"The matrix is Symmetrical";
    else
        cout<<"The matrix is Asymmetrical";
    return;
}
