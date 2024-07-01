#include "global.h"
/**
 * @brief
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    regex delim("[^\\s,]+");

    logger.log("executeSOURCE");
    syntacticParseSOURCE();
    semanticParseSOURCE();
    // cout << parsedQuery.sourceFileName << endl;

    logger.log("Source file name: " + parsedQuery.sourceFileName);

    fstream source_file;
    string file_name = "../data/" + parsedQuery.sourceFileName + ".ra";
    source_file.open(file_name, ios::in);
    string line;
    while (getline(source_file, line))
    {
        // cout << line << "\n";
        tokenizedQuery.clear();
        parsedQuery.clear();
        logger.log("\nReading New Command: " + line);

        auto words_begin = std::sregex_iterator(line.begin(), line.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
        {
            break;
        }

        if (tokenizedQuery.empty())
        {
            continue;
        }

        if (tokenizedQuery.size() == 1)
        {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }
        logger.log("Executing command: " + line);
        if (syntacticParse() && semanticParse())
            executeCommand();
    }
    return;
}
