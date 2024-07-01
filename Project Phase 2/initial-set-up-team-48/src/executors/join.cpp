#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeJOIN()
{
    logger.log("executeJOIN");

    Table table1 = *tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table table2 = *tableCatalogue.getTable(parsedQuery.joinSecondRelationName);
    vector<bool> sortAscendingArray;
    sortAscendingArray.push_back(1);
    vector<int> sortColumnIndex;
    sortColumnIndex.push_back(table1.getColumnIndex(parsedQuery.joinFirstColumnName));
    table1.sortPages(sortAscendingArray,sortColumnIndex);
    sortColumnIndex.clear();
    sortColumnIndex.push_back(table2.getColumnIndex(parsedQuery.joinSecondColumnName));
    table2.sortPages(sortAscendingArray,sortColumnIndex);
    vector<string> columns;

    //Creating list of column names
    for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
    {
        string columnName = table1.columns[columnCounter];
        if (table2.isColumn(columnName))
        {
            columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++)
    {
        string columnName = table2.columns[columnCounter];
        if (table1.isColumn(columnName))
        {
            columnName = parsedQuery.joinSecondRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    // we dont need to create resultTable, if we want to use it we can load it into memory?
    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);
    resultantTable->columnCount = columns.size();
    resultantTable->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * resultantTable->columnCount));

    Cursor cursor1 = table1.getCursor();

    vector<int> row1 = cursor1.getNext();
    vector<int> resultantRow;
    vector< vector <int>> resultantRows;
    
    int pageIdxResultantTable = 0;

    // RS <- JOIN USING NESTED A, DEPARTMENT ON a == Dnumber BUFFER 4

    while (!row1.empty())
    {
        // for(int i=0;i<int(row1.size());i++) cout << row1[i] << " ";
        // cout << endl;

        Cursor cursor2 = table2.getCursor();
        vector<int> row2 = cursor2.getNext();

        vector <vector<int> > table1Rows;

        while((cursor1.pageIndex+1)%(stoi(parsedQuery.joinBufferSize)-2)!=0)
            {
                // for(int i=0;i<int(row1.size());i++) cout << row1[i] << " ";
                // cout << endl;
                // cout << cursor1.pageIndex << endl;
                table1Rows.push_back(row1);
                // cout << "pushed back" << endl;
                row1 = cursor1.getNext();
                // cout << (stoi(parsedQuery.joinBufferSize)) << endl;
                // cout << "got next row" << endl;
                // cout << (cursor1.pageIndex+1)%(stoi(parsedQuery.joinBufferSize)-2) << endl;

                if (row1.empty())
                {
                    // cout << "row1 is empty" << endl;
                    break;
                } 
            }

        //cout << "entered out of while loop" << endl;

        while(!row2.empty())
        {
            //for(int i=0;i<int(row2.size());i++) cout << row2[i] << " ";
           // cout << endl;

            int table1ColIdx = distance(table1.columns.begin(),find(table1.columns.begin(), table1.columns.end(), parsedQuery.joinFirstColumnName));
            int table2ColIdx = distance(table2.columns.begin(),find(table2.columns.begin(), table2.columns.end(), parsedQuery.joinSecondColumnName));
            
            //cout << table1ColIdx << " " << table2ColIdx << endl;
            //cout << parsedQuery.joinBinaryOperator << endl;

            for(int i=0;i<int(table1Rows.size());i++)
            {
                resultantRow = table1Rows[i];
                //cout << table1Rows[i][table1ColIdx] << " " << row2[table2ColIdx] << endl;

                // cout << "befmore checking..." << endl;
                // for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                // cout << endl;

                // int operator = parsedQuery.joinBinaryOperator;

                switch(int(parsedQuery.joinBinaryOperator)){
                    case 0: 
                    {
                        if(table1Rows[i][table1ColIdx] < row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 1: 
                    {
                        if(table1Rows[i][table1ColIdx] > row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 2: 
                    {
                        if(table1Rows[i][table1ColIdx] <= row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 3: 
                    {
                        if(table1Rows[i][table1ColIdx] <= row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 4: {
                                if(table1Rows[i][table1ColIdx] == row2[table2ColIdx]) 
                                {
                                    resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                                    // cout << "after checking..." << endl;
                                    // for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                                    // cout << endl;
                                }

                                break;
                            }
                    case 5: 
                    {
                        if(table1Rows[i][table1ColIdx] != row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    default: 
                    {
                        //cout << "binop doesnt exist" << endl;
                        break;
                    }
                }

                //cout << "outside switch case..." << endl;
                //for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                //cout << endl;

                // in case the block allocated for storing results overflows, write it to the table and empty out this buffer
                if (resultantRow.size() > table1Rows[i].size())
                {
                    //for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                    //cout << endl;

                    if(int(resultantRows.size()+1)*4*int(columns.size()) > BLOCK_SIZE*1000)
                    {
                        //cout << "we are trying to write the page now" << endl;
                        // move to table, empty buffer
                        bufferManager.writePage(parsedQuery.joinResultRelationName,pageIdxResultantTable , resultantRows, int(resultantRows.size()));
                        resultantTable->rowCount+=resultantRows.size();
                        resultantTable->rowsPerBlockCount.push_back(resultantRows.size());
                        pageIdxResultantTable += 1;
                        resultantRows.clear();
                    }

                    resultantRows.push_back(resultantRow);
                    //resultantTable->rowCount+=resultantRows.size();

                }
                    
                
            }

            row2 = cursor2.getNext();
            
        }
        
    }

    // move to table, empty buffer
    bufferManager.writePage(parsedQuery.joinResultRelationName,pageIdxResultantTable , resultantRows, int(resultantRows.size()));
    resultantTable->rowCount+=resultantRows.size();
    resultantTable->rowsPerBlockCount.push_back(resultantRows.size());
    pageIdxResultantTable += 1;
    resultantTable->blockCount = pageIdxResultantTable;
    resultantRows.clear();


    tableCatalogue.insertTable(resultantTable);

    bufferManager.flushPool();
    return;
}