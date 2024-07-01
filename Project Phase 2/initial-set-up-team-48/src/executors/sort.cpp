#include "global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 *
 * sorting_order = ASC | DESC
 */
bool syntacticParseSORT()
{
    logger.log("syntacticParseSORT");
    if (tokenizedQuery.size() < 6 || tokenizedQuery[2] != "BY")
    {
        cout << "SYNTAX ERROR:1" << endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    parsedQuery.sortAscendingArray.clear();
    parsedQuery.sortColumnArray.clear();
    bool flag = false;
    for (int i = 3; i < tokenizedQuery.size(); i++)
    {
        // cout << tokenizedQuery[i] << endl;
        if (tokenizedQuery[i] == "IN")
        {
            flag = true;
            continue;
        }
        if (flag) // order
        {
            if (tokenizedQuery[i] == "ASC")
                parsedQuery.sortAscendingArray.emplace_back(true);
            else if (tokenizedQuery[i] == "DESC")
                parsedQuery.sortAscendingArray.emplace_back(false);
            else
            {
                cout << "SYNTAX ERROR: 2" << endl;
                return false;
            }
        }
        else // column
        {
            parsedQuery.sortColumnArray.emplace_back(tokenizedQuery[i]);
        }
    }
    if (!flag)
    { // if IN is not present
        cout << "SYNTAX ERROR:3" << endl;
        return false;
    }
    if (parsedQuery.sortAscendingArray.size() != parsedQuery.sortColumnArray.size())
    {
        cout << "SYNTAX ERROR: Columns and sorting order number does not match" << endl;
        return false;
    }
    
    return true;
}

bool semanticParseSORT()
{
    logger.log("semanticParseSORT");

    

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    for (auto colName : parsedQuery.sortColumnArray)
    {

        if (!tableCatalogue.isColumnFromTable(colName, parsedQuery.sortRelationName))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
        }
    }

    return true;
}

void executeSORT()
{
    logger.log("executeSORT");

    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);

    parsedQuery.sortColumnIndex.clear();
    for(auto colName : parsedQuery.sortColumnArray){
        parsedQuery.sortColumnIndex.emplace_back(table->getColumnIndex(colName));
    }

    logger.log(parsedQuery.sortRelationName);
    parsedQuery.sortResultRelationName = "PLEASEWORK";
    logger.log(parsedQuery.sortResultRelationName);
    table->sortPages(parsedQuery.sortAscendingArray, parsedQuery.sortColumnIndex/*, parsedQuery.sortResultRelationName*/);
    cout << "Table sorted successfully" << endl;
    return;
}