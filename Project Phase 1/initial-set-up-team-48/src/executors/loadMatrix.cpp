#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD MATRIX matrix_name
 */
bool syntacticParseLOADMATRIX()
{
    logger.log("syntacticParseLOADMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOADMATRIX;
    parsedQuery.loadRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOADMATRIX()
{
    logger.log("semanticParseLOADMATRIX");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOADMATRIX()
{
    logger.log("executeMatrixLOAD");

    Table *matrix = new Table(parsedQuery.loadRelationName, true);
    if (matrix->loadMatrix())
    {
        tableCatalogue.insertTable(matrix);
        cout << "Loaded Matrix. Column Count: " << matrix->columnCount << " Row Count: " << matrix->rowCount << endl;
    }
    return;
}