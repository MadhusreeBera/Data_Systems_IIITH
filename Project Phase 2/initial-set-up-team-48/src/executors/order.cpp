#include "global.h"
/**
 * @brief 
 * SYNTAX: <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
 */
bool syntacticParseORDER()
{
    logger.log("syntacticParseORDER");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[2] != "ORDER" || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    
    parsedQuery.queryType = ORDERBY;
    parsedQuery.orderResultantRelation = tokenizedQuery[0];
    logger.log(tokenizedQuery[4]);
    parsedQuery.orderColumnName=tokenizedQuery[4];
    if(tokenizedQuery[5]=="ASC")
        parsedQuery.orderbyorder=0;
    else if(tokenizedQuery[5]=="DESC")
        parsedQuery.orderbyorder=1;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.orderRelationName=tokenizedQuery[7];
    return true;
}

bool semanticParseORDER()
{
    logger.log("semanticParseORDER");

    if (tableCatalogue.isTable(parsedQuery.orderResultantRelation))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderColumnName,parsedQuery.orderRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeORDER()
{
    //creating copy of the csv file.
    string sourceFileName="../data/"+parsedQuery.orderRelationName+".csv";
    string copyFileName="../data/"+parsedQuery.orderResultantRelation+".csv";

    ifstream sourceFile(sourceFileName, std::ios::in);
    ofstream copyFile(copyFileName, std::ios::out);

    if (sourceFile && copyFile) {
        std::string line;

        while (std::getline(sourceFile, line)) {
            copyFile << line << "\n";
        }

        logger.log("File copied successfully!");

        sourceFile.close();
        copyFile.close();
    } else {
        logger.log("Error: Unable to create resultant file");
    }

    //loading the table
    logger.log("executing LOAD in ORDER");

    Table *table = new Table(parsedQuery.orderResultantRelation);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
    }

    //executing sort
    int colindex=table->getColumnIndex(parsedQuery.orderColumnName);
    // the sort function accepts a vector 
    vector<bool> sortingStrategy;
    sortingStrategy.push_back(!parsedQuery.orderbyorder);
    vector<int> columnIndexes;
    columnIndexes.push_back(colindex);
    table->sortPages(sortingStrategy, columnIndexes);

    //removing the table
    table->unload();
    cout << "Query ORDER BY executed successfully" << endl;
    return;
}