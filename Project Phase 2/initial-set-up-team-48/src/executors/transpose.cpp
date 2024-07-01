#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX matrix_name
 */
bool syntacticParseTRANSPOSEMATRIX()
{
    logger.log("syntacticParseTRANSPOSEMATRIX");
    if(tokenizedQuery.size()!=3)
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSEMATRIX;
    parsedQuery.transposeMatrix = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSEMATRIX()
{
    logger.log("semanticParseTRANSPOSEMATRIX");
    if (!tableCatalogue.isTable(parsedQuery.transposeMatrix))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void TRANSPOSE(vector<vector<int> > &b)
{
    if (b.size() == 0)
        return;

    vector<vector<int> > trans_vec(b[0].size(), vector<int>());

    for (int i = 0; i < b.size(); i++)
    {
        for (int j = 0; j < b[i].size(); j++)
        {
            trans_vec[i].push_back(b[j][i]);
        }
    }

    b = trans_vec;
    for (int i = 0; i < b.size(); i++)
    {
        for (int j = 0; j < b[i].size(); j++)
        {
            cout<<b[i][j]<<",";
        }
        cout<<endl;
    }
}

void executeTRANSPOSEMATRIX()
{
    logger.log("executeTRANSPOSEMATRIX");
    Table* matrixTran = tableCatalogue.getTable(parsedQuery.transposeMatrix);
    matrixTran->Transpose(parsedQuery.transposeMatrix);
    cout<<"Matrix is transposed";
    return;
}