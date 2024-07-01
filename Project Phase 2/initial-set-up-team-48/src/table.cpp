#include "global.h"
#include <cmath>

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}
Table::Table(Table *table, string tableName)
{
    this->tableName = tableName;
    this->columnCount = table->columnCount;
    this->maxRowsPerBlock = table->maxRowsPerBlock;
    this->matrixBlocks = table->matrixBlocks;
    this->rowCount = table->rowCount;
    this->blockCount = table->blockCount;
    this->maxRowsPerBlock = table->maxRowsPerBlock;
    this->rowsPerBlockCount = table->rowsPerBlockCount;
    this->columnsPerBlockCount = table->columnsPerBlockCount;
    this->isMatrix = table->isMatrix;
}

Table::Table(string tableName, bool isMatrix)
{
    logger.log("Table::Matrix constructor");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
    this->isMatrix = isMatrix;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line, word;
    if (getline(fin, line))
    {

        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}
bool Table::loadMatrix()
{

    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line, word;
    int columns = 0;

    if (getline(fin, line))
    {
        fin.close();

        stringstream s(line);
        logger.log(line);
        while (getline(s, word, ','))
        {
            columns++;
        }
        this->columnCount = columns;

        // calculating rows per block
        this->maxRowsPerBlock = floor(sqrt((BLOCK_SIZE * 1000) / (sizeof(uint))));
        logger.log("maxRowsPerBlock: " + to_string(this->maxRowsPerBlock));

        // calculating block number
        this->matrixBlocks = ceil((double)columns / this->maxRowsPerBlock);
        logger.log("matrixBlocks: " + to_string(this->matrixBlocks));

        this->blockifyMatrix();
    }
    fin.close();
    return true;
}

bool Table::blockifyMatrix()
{
    int blocksRead = 0, blocksWritten = 0;
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    int rowNumber = 0;
    int blockRowNumber = 0;
    int pageCounter = 0;

    vector<vector<vector<int>>> matrixVector(this->matrixBlocks, vector<vector<int>>(this->maxRowsPerBlock, vector<int>()));
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            matrixVector[columnCounter / this->maxRowsPerBlock][rowNumber % this->maxRowsPerBlock].push_back(stoi(word));
        }
        pageCounter++;

        rowNumber++;
        if (pageCounter == (this->maxRowsPerBlock))
        {
            for (int i = 0; i < this->matrixBlocks; i++)
            {
                int blockNumber = ((rowNumber - 1) / this->maxRowsPerBlock) * this->matrixBlocks + i;
                bufferManager.writePage(this->tableName, blockNumber, matrixVector[i], pageCounter);
                blocksWritten++;
                logger.log("Reaching");
                this->rowsPerBlockCount.emplace_back(pageCounter);
                logger.log(to_string(matrixVector[i][0].size()));
                this->columnsPerBlockCount.emplace_back(matrixVector[i][0].size());
            }

            pageCounter = 0;
            matrixVector.clear();
            matrixVector.resize(this->matrixBlocks, vector<vector<int>>(this->maxRowsPerBlock, vector<int>()));
        }

    } // while
    if (pageCounter)
    {
        for (int i = 0; i < this->matrixBlocks; i++)
        {
            int blockNumber = ((rowNumber - 1) / this->maxRowsPerBlock) * this->matrixBlocks + i;
            bufferManager.writePage(this->tableName, blockNumber, matrixVector[i], pageCounter);

            blocksWritten++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            this->columnsPerBlockCount.emplace_back(matrixVector[i][0].size());
        }
    }
    this->rowCount = rowNumber;
    printBlockDetails(blocksRead, blocksWritten);
    return true;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;

        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}
void Table::renameMatrix(string fromMatrix, string toMatrix)
{
    logger.log("Matrix::renameMatrix");
    int blocksRead = 0, blocksWritten = 0;
    int noOfBlocks = this->matrixBlocks * this->matrixBlocks;

    this->tableName = toMatrix;
    tableCatalogue.renameTable(fromMatrix, toMatrix);

    for (int pageCounter = 0; pageCounter < noOfBlocks; pageCounter++)
        bufferManager.renameFile(fromMatrix, toMatrix, pageCounter);
    printf("%s", "File name changed successfully");
    printBlockDetails(blocksRead, blocksWritten);
}

bool Table::checkSymMatrix(string matrixName)
{
    logger.log("Matrix::checkSymMatrix");
    int blocksRead = 0, blocksWritten = 0;

    // logger.log(to_string(this->maxRowsPerBlock));//15
    int noOfBlocks = this->matrixBlocks * this->matrixBlocks;
    // logger.log(to_string(matrixBlocks));
    logger.log("No. of blocks" + to_string(noOfBlocks));
    this->pagesym.resize(noOfBlocks, 0);
    for (int pageCounter = 0; pageCounter < noOfBlocks; pageCounter++)
    {
        logger.log("pagecounter: " + to_string(pageCounter));
        if (this->pagesym[pageCounter] == 0)
        {
            int r = pageCounter / matrixBlocks;
            int c = pageCounter % matrixBlocks;
            if (r == c)
            {
                logger.log("r==c");
                Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
                logger.log("Returned from cursor");
                vector<int> row;
                vector<vector<int>> data(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
                logger.log("rows in this page: " + to_string(this->rowsPerBlockCount[pageCounter]));
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
                {
                    logger.log("Rowcounter: " + to_string(rowCounter));
                    row = matrixCheckCursor->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data[rowCounter] = row;
                    }
                }
                blocksRead++;
                logger.log("calculating...");
                for (int row = 0; row < data.size(); row++)
                {

                    for (int col = 0; col < data[0].size(); col++)
                    {

                        if (data[row][col] != data[col][row])
                        {
                            printBlockDetails(blocksRead, blocksWritten);
                            return false;
                        }
                    }
                }
            }
            else
            {
                Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
                int newr = c, newc = r;
                int pageindex2 = (newr * matrixBlocks) + newc;
                Cursor *matrixCheckCursor2 = new Cursor(matrixName, pageindex2, true);
                vector<int> row;
                vector<vector<int>> data1(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
                vector<vector<int>> data2(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
                {
                    row = matrixCheckCursor->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data1[rowCounter] = row;
                    }
                }
                blocksRead++;
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageindex2]; rowCounter++)
                {
                    row = matrixCheckCursor2->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data2[rowCounter] = row;
                    }
                }
                blocksRead++;
                // taking transpose of one matrix
                for (int row = 0; row < data1.size(); row++)
                    for (int col = row; col < data1[0].size(); col++)
                        swap(data1[row][col], data1[col][row]);

                // comparing each value with other original matrix
                for (int row = 0; row < data1.size(); row++)
                {

                    for (int col = 0; col < data1[0].size(); col++)
                    {

                        if (data1[row][col] != data2[row][col])
                        {
                            printBlockDetails(blocksRead, blocksWritten);
                            return false;
                        }
                    }
                }
            }
            this->pagesym[pageCounter] = 1;
        }
    }
    return true;
    printBlockDetails(blocksRead, blocksWritten);
}

void Table::Transpose(string matrixName)
{
    logger.log("Matrix::Transpose");
    int blocksRead = 0, blocksWritten = 0;

    logger.log(matrixName);
    logger.log(to_string(this->maxRowsPerBlock)); // 15
    int noOfBlocks = this->matrixBlocks * this->matrixBlocks;
    logger.log(to_string(matrixBlocks));
    logger.log("No. of blocks" + to_string(noOfBlocks));
    this->pagetran.resize(noOfBlocks, 0);
    for (int pageCounter = 0; pageCounter < noOfBlocks; pageCounter++)
    {
        logger.log("pagecounter: " + to_string(pageCounter));
        if (this->pagetran[pageCounter] == 0)
        {
            int r = pageCounter / matrixBlocks;
            int c = pageCounter % matrixBlocks;
            if (r == c)
            {
                logger.log("r==c");
                Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
                logger.log("Returned from cursor");
                vector<int> row;
                vector<vector<int>> data(this->rowsPerBlockCount[pageCounter], vector<int>(this->columnsPerBlockCount[pageCounter], 0));
                logger.log("rows in this page: " + to_string(this->rowsPerBlockCount[pageCounter]));
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
                {
                    logger.log("Rowcounter: " + to_string(rowCounter));
                    row = matrixCheckCursor->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data[rowCounter] = row;
                    }
                }
                blocksRead++;
                logger.log("calculating...");
                for (int row = 0; row < data.size(); row++)
                    for (int col = row; col < data[0].size(); col++)
                        swap(data[row][col], data[col][row]);
                bufferManager.writePage(matrixName, pageCounter, data, this->rowsPerBlockCount[pageCounter]);
                bufferManager.removeFromPool(matrixName, pageCounter);
                blocksWritten++;
                logger.log("page is written");
            }
            else
            {
                Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
                int newr = c, newc = r;
                int pageindex2 = (newr * matrixBlocks) + newc;
                Cursor *matrixCheckCursor2 = new Cursor(matrixName, pageindex2, true);
                vector<int> row;
                vector<vector<int>> data1(this->rowsPerBlockCount[pageCounter], vector<int>(this->columnsPerBlockCount[pageCounter], 0));
                vector<vector<int>> data2(this->rowsPerBlockCount[pageindex2], vector<int>(this->columnsPerBlockCount[pageindex2], 0));
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
                {
                    row = matrixCheckCursor->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data1[rowCounter] = row;
                    }
                }
                blocksRead++;
                for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageindex2]; rowCounter++)
                {
                    row = matrixCheckCursor2->getNextMatrixRow();
                    if (!row.empty())
                    {
                        data2[rowCounter] = row;
                    }
                }
                blocksRead++;
                vector<vector<int>> data1_T(this->columnsPerBlockCount[pageCounter], vector<int>(this->rowsPerBlockCount[pageCounter], 0));
                vector<vector<int>> data2_T(this->columnsPerBlockCount[pageindex2], vector<int>(this->rowsPerBlockCount[pageindex2], 0));
                // taking transpose of 1st matrix
                for (int row = 0; row < data1.size(); row++)
                    for (int col = 0; col < data1[0].size(); col++)
                        data1_T[col][row] = data1[row][col];

                // taking transpose of 2nd matrix
                for (int row = 0; row < data2.size(); row++)
                    for (int col = 0; col < data2[0].size(); col++)
                        data2_T[col][row] = data2[row][col];

                bufferManager.writePage(matrixName, pageCounter, data2_T, this->rowsPerBlockCount[pageCounter]);
                bufferManager.removeFromPool(matrixName, pageCounter);
                blocksWritten++;

                bufferManager.writePage(matrixName, pageindex2, data1_T, this->rowsPerBlockCount[pageindex2]);
                bufferManager.removeFromPool(matrixName, pageindex2);
                blocksWritten++;
                this->pagetran[pageindex2] = 1;
            }
            this->pagetran[pageCounter] = 1;
        }
    }
    printBlockDetails(blocksRead, blocksWritten);
}

void Table::computeMatrix(string matrixName)
{
    logger.log("Matrix::Compute");
    int blocksRead = 0, blocksWritten = 0;

    logger.log(matrixName);
    logger.log(to_string(this->maxRowsPerBlock)); // 15
    int noOfBlocks = this->matrixBlocks * this->matrixBlocks;
    logger.log(to_string(matrixBlocks));
    logger.log("No. of blocks" + to_string(noOfBlocks));
    this->pagetran.resize(noOfBlocks, 0);
    string newmatrix = matrixName + "_Result";

    Table *new_matrix = new Table(this, newmatrix);
    tableCatalogue.insertTable(new_matrix);
    for (int pageCounter = 0; pageCounter < noOfBlocks; pageCounter++)
    {
        logger.log("pagecounter: " + to_string(pageCounter));

        int r = pageCounter / matrixBlocks;
        int c = pageCounter % matrixBlocks;
        if (r == c)
        {
            logger.log("r==c");
            Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
            logger.log("Returned from cursor");
            vector<int> row;
            vector<vector<int>> data(this->rowsPerBlockCount[pageCounter], vector<int>(this->columnsPerBlockCount[pageCounter], 0));
            logger.log("rows in this page: " + to_string(this->rowsPerBlockCount[pageCounter]));
            for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
            {
                logger.log("Rowcounter: " + to_string(rowCounter));
                row = matrixCheckCursor->getNextMatrixRow();
                if (!row.empty())
                {
                    data[rowCounter] = row;
                }
            }
            blocksRead++;

            logger.log("calculating...");
            vector<vector<int>> dataR(this->rowsPerBlockCount[pageCounter], vector<int>(this->columnsPerBlockCount[pageCounter], 0));
            for (int row = 0; row < data.size(); row++)
                for (int col = 0; col < data[0].size(); col++)
                {
                    dataR[row][col] = data[row][col] - data[col][row];
                }
            bufferManager.writePage(newmatrix, pageCounter, dataR, this->rowsPerBlockCount[pageCounter]);
            blocksWritten++;
            logger.log("page is written");
        }
        else
        {
            Cursor *matrixCheckCursor = new Cursor(matrixName, pageCounter, true);
            int newr = c, newc = r;
            int pageindex2 = (newr * matrixBlocks) + newc;
            Cursor *matrixCheckCursor2 = new Cursor(matrixName, pageindex2, true);
            vector<int> row;
            vector<vector<int>> data1(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
            vector<vector<int>> data2(this->maxRowsPerBlock, vector<int>(this->maxRowsPerBlock, 0));
            for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageCounter]; rowCounter++)
            {
                row = matrixCheckCursor->getNextMatrixRow();
                if (!row.empty())
                {
                    data1[rowCounter] = row;
                }
            }
            blocksRead++;
            for (int rowCounter = 0; rowCounter < this->rowsPerBlockCount[pageindex2]; rowCounter++)
            {
                row = matrixCheckCursor2->getNextMatrixRow();
                if (!row.empty())
                {
                    data2[rowCounter] = row;
                }
            }
            blocksRead++;

            // taking transpose of 2nd matrix
            // for (int row = 0; row < data2.size(); row++)
            //     for (int col = row; col < data2[0].size(); col++)
            //         swap(data2[row][col], data2[col][row]);

            vector<vector<int>> dataR(this->rowsPerBlockCount[pageCounter], vector<int>(this->columnsPerBlockCount[pageCounter], 0));
            for (int row = 0; row < dataR.size(); row++)
                for (int col = 0; col < dataR[0].size(); col++)
                {
                    dataR[row][col] = data1[row][col] - data2[col][row];
                }

            bufferManager.writePage(newmatrix, pageCounter, dataR, this->rowsPerBlockCount[pageCounter]);
            blocksWritten++;
        }
    }
    printBlockDetails(blocksRead, blocksWritten);
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    // print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}
void Table::printMatrix()
{

    logger.log("Matrix::print");
    int blocksRead = 0, blocksWritten = 0;

    uint count = min((long long)PRINT_COUNT, this->rowCount);

    logger.log("Matrix Blocks: " + to_string(this->matrixBlocks));
    int pageIndex = 0;
    int rowNumber = 0;
    bool rowFlag = true;
    vector<Cursor *> cursorVector;
    for (int i = 0; i < this->matrixBlocks && rowFlag; i++)
    {
        // getting cursors for all blocks in ith row
        for (int j = 0; j < this->matrixBlocks; j++)
        {
            Cursor *c = new Cursor(this->tableName, pageIndex, this->isMatrix);
            pageIndex++;
            blocksRead++;
            cursorVector.push_back(c);
            logger.log("Cursor for page: " + to_string(pageIndex));
        }

        // getting all columns for all blocks in ith row
        vector<int> matrixRow;

        bool flag = true;
        while (flag && rowFlag)
        {
            for (int j = 0; j < this->matrixBlocks; j++)
            {
                logger.log("j = " + to_string(j) + " size = " + to_string(cursorVector.size()));

                vector<int> pageRow = cursorVector[j]->getNextMatrixRow();
                // for (auto ele : pageRow)
                // {
                //     cout << ele << " ";
                // }
                // cout << endl;
                if (pageRow.empty())
                {
                    flag = false;
                    break;
                }

                matrixRow.insert(matrixRow.end(), pageRow.begin(), pageRow.end());
                pageRow.clear();
            }
            // writing the row
            if (flag)
            {

                this->writeRow(matrixRow, cout, count);
                matrixRow.clear();
                rowNumber++;
            }
            if (rowNumber == count)
            {
                rowFlag = false;
                break;
            }

        } // while
        cursorVector.clear();
    } // for

    printRowCount(this->rowCount);
    printBlockDetails(blocksRead, blocksWritten);
}
/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{

    if (this->isMatrix)
    {
        logger.log("Matrix::makePermanent");
        if (!this->isPermanent())
            bufferManager.deleteFile(this->sourceFileName);
        string newSourceFile = "../data/" + this->tableName + ".csv";
        ofstream fout(newSourceFile, ios::out);

        Cursor cursor(this->tableName, 0);
        vector<int> row;
        for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            row = cursor.getNext();
            this->writeRow(row, fout);
        }
        fout.close();
    }
    else
    {

        logger.log("Table::makePermanent");
        if (!this->isPermanent())
            bufferManager.deleteFile(this->sourceFileName);
        string newSourceFile = "../data/" + this->tableName + ".csv";
        ofstream fout(newSourceFile, ios::out);

        // print headings
        this->writeRow(this->columns, fout);

        Cursor cursor(this->tableName, 0);
        vector<int> row;
        for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            row = cursor.getNext();
            this->writeRow(row, fout);
        }
        fout.close();
    }
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
        return true;
    return false;
}
void Table::exportMatrix()
{

    logger.log("Matrix::export");
    int blocksRead = 0, blocksWritten = 0;
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    logger.log("Matrix Blocks: " + to_string(this->matrixBlocks));
    int pageIndex = 0;
    vector<Cursor *> cursorVector;
    for (int i = 0; i < this->matrixBlocks; i++)
    {
        // getting cursors for all blocks in ith row
        for (int j = 0; j < this->matrixBlocks; j++)
        {
            Cursor *c = new Cursor(this->tableName, pageIndex, this->isMatrix);
            blocksRead++;
            pageIndex++;
            cursorVector.push_back(c);
            logger.log("Cursor for page: " + to_string(pageIndex));
        }

        // getting all columns for all blocks in ith row
        vector<int> matrixRow;

        bool flag = true;
        while (flag)
        {
            for (int j = 0; j < this->matrixBlocks; j++)
            {
                logger.log("j = " + to_string(j) + " size = " + to_string(cursorVector.size()));

                vector<int> pageRow = cursorVector[j]->getNextMatrixRow();
                // for (auto ele : pageRow)
                // {
                //     cout << ele << " ";
                // }
                // cout << endl;
                if (pageRow.empty())
                {
                    flag = false;
                    break;
                }

                matrixRow.insert(matrixRow.end(), pageRow.begin(), pageRow.end());
                pageRow.clear();
            }
            // writing the row
            if (flag)
            {

                this->writeRow(matrixRow, fout);
                matrixRow.clear();
            }
        }
        cursorVector.clear();
    }
    fout.close();
    printBlockDetails(blocksRead, blocksWritten);
}
/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload()
{
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

void rename_the_pages(string tableName, int totalBlocks, int passBlockSize)
{
    int count = 0;
    for (int i = 0; i < passBlockSize; i++)
    {
        for (int j = i; j < totalBlocks; j += passBlockSize)
        {

            string fromName = "../data/temp/" + tableName + "_1_Page" + to_string(j);
            string toName = "../data/temp/" + tableName + "_Page" + to_string(count);
            logger.log("renaming " + fromName + " to " + toName);
            rename(fromName.c_str(), toName.c_str());
            count++;
        }
    }
}
void Table::sortPages(vector<bool> sortingStrategy, vector<int> columnIndexes /*, string resultRelationName*/)
{
    logger.log("Table::sort");
    vector<vector<int>> rowsInPage;
    vector<int> row;
    int pageCounter = 0;

    // sorting individual pages
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
    {
        rowsInPage = bufferManager.getPage(this->tableName, pageCounter).getRows();
        rowsInPage.resize(this->rowsPerBlockCount[pageCounter]);

        sort(rowsInPage.begin(), rowsInPage.end(), [columnIndexes, sortingStrategy](vector<int> &a, vector<int> &b)
             {
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
        for(int i=0; i<columnIndexes.size(); i++)
        {
            int columnCounter = columnIndexes[i];
            bool strategy = sortingStrategy[i];
            if(a[columnCounter]!=b[columnCounter])
            {
                if(strategy)
                {
                    return a[columnCounter] < b[columnCounter];
                }
                else
                {
                    return a[columnCounter] > b[columnCounter];
                }
            }
        }
        return false; });
        bufferManager.writePage(this->tableName, pageCounter, rowsInPage, rowsInPage.size());
    }

    // merging

    auto comp = [columnIndexes, sortingStrategy](pair<int, vector<int>> &a, pair<int, vector<int>> &b)
    {
            for(int i=0; i<columnIndexes.size(); i++)
            {
                int columnCounter = columnIndexes[i];
                bool strategy = sortingStrategy[i];
                if(a.second[columnCounter]!=b.second[columnCounter])
                {
                    if(strategy)
                    {
                        return a.second[columnCounter] > b.second[columnCounter];
                    }
                    else
                    {
                        return a.second[columnCounter] < b.second[columnCounter];
                    }
                }
            }
            return false; };

    // for (auto e : this->rowsPerBlockCount)
    // {
    //     cout << e << ' ';
    // }
    // cout << endl;
    vector<Cursor *> cursorVector;
    int mergeBlockSize = 1;

    int no_of_passes = ceil((double)log(this->blockCount) / log(BUFFER_SIZE - 1));
    // cout << "No of Passes: " << no_of_passes << endl;

    // first pass will always happen by default
    int sort_page_index = 0;
    int start_index = 0;
    int page_index = 0;
    vector<vector<int>> passBlockPagesVector;
    while (true)
    {
        vector<int> block_no_Vector;
        if (sort_page_index == this->blockCount)
            break;

        // getting cursors
        start_index = sort_page_index;
        // cout << "start " << start_index << endl;
        vector<pair<int, vector<int>>> temp;

        int total_rows = 0;
        temp.resize(0);
        cursorVector.resize(0);
        for (int i = 0; i < BUFFER_SIZE - 1 && sort_page_index < this->blockCount; i++)
        {
            Cursor *c = new Cursor(this->tableName, sort_page_index);
            logger.log("Cursor for page stored in vector: " + to_string(sort_page_index));
            total_rows += this->rowsPerBlockCount[sort_page_index];
            sort_page_index++;
            cursorVector.push_back(c);
            temp.push_back({i, c->getNextMatrixRow()});
        }

        int cursorVectorIndex = 0;

        priority_queue<pair<int, vector<int>>, vector<pair<int, vector<int>>>, decltype(comp)> pq(comp);

        // pushing elements in priority queue
        for (auto ele : temp)
        {
            pq.push(ele);
        }

        int row_index = temp.size();

        vector<vector<int>> rowsInPage;

        logger.log("total rows = " + to_string(total_rows));
        rowsInPage.resize(0);
        while (row_index < total_rows)
        {
            // cout << row_index << endl;
            auto popped_ele = pq.top();
            pq.pop();
            rowsInPage.emplace_back(popped_ele.second);
            if (rowsInPage.size() == this->rowsPerBlockCount[page_index])
            {
                bufferManager.writePage(this->tableName + "_1", page_index, rowsInPage, rowsInPage.size());
                block_no_Vector.emplace_back(page_index);
                page_index++;

                rowsInPage.resize(0);
                logger.log("Page size jiska block bana: " + to_string(rowsInPage.size()));
            }
            logger.log("Page Index: " + to_string(page_index));

            auto temp_row = cursorVector[popped_ele.first]->getNextMatrixRow();
            if (temp_row.empty())
                continue;

            pq.push({popped_ele.first, temp_row});
            row_index++;

            // bufferManager.writePage(this->tableName + "_2", 9, rowsInPage, rowsInPage.size());
        }
        while (!pq.empty())
        {
            rowsInPage.emplace_back(pq.top().second);
            pq.pop();
        }
        bufferManager.writePage(this->tableName + "_1", page_index, rowsInPage, rowsInPage.size());
        rowsInPage.resize(0);
        block_no_Vector.emplace_back(page_index);

        page_index++;

        passBlockPagesVector.emplace_back(block_no_Vector);
    }
    // deleting previous blocks and renaming all the blocks from table_1 to table
    for (int i = 0; i < this->blockCount; i++)
    {
        string fromName = "../data/temp/" + this->tableName + "_1_Page" + to_string(i);
        string toName = "../data/temp/" + this->tableName + "_Page" + to_string(i);
        rename(fromName.c_str(), toName.c_str());
        bufferManager.removeFromPool(this->tableName, i);
    }

    // more than 1 pass
    int passBlockSize = BUFFER_SIZE - 1;

    int passNo = 2;

    while (passNo <= no_of_passes)
    {
        vector<vector<int>> temp_passBlockPagesVector;
        temp_passBlockPagesVector.resize(0);
        passNo++;
        page_index = 0;

        for (int i = 0; i < passBlockPagesVector.size(); i += passBlockSize)
        {
            int total_rows = 0;
            vector<int> block_no_Vector;

            vector<int> indices;
            cursorVector.resize(0);
            vector<pair<int, vector<int>>> temp;

            for (int j = i; j < passBlockPagesVector.size() && j < i + passBlockSize; j++)
            {
                indices.emplace_back(0);
                Cursor *c = new Cursor(this->tableName, passBlockPagesVector[j][0]);
                logger.log("Cursor for page stored in vector for more than 1 pass: " + to_string(passBlockPagesVector[j][0]));
                cursorVector.push_back(c);
                temp.push_back({cursorVector.size() - 1, c->getNextMatrixRow()});

                for (auto k : passBlockPagesVector[j])
                    total_rows += this->rowsPerBlockCount[k];
            }
            // cout << "total_rows:   " << total_rows << endl;

            priority_queue<pair<int, vector<int>>, vector<pair<int, vector<int>>>, decltype(comp)> pq(comp);
            for (auto ele : temp)
            {
                pq.push(ele);
            }
            int row_index = temp.size();
            vector<vector<int>> rowsInPage;
            rowsInPage.resize(0);
            while (row_index < total_rows)
            {
                if (pq.empty())
                {
                    // cout << "pq empty" << endl;
                    break;
                }
                auto popped_ele = pq.top();
                pq.pop();
                rowsInPage.emplace_back(popped_ele.second);
                if (rowsInPage.size() == this->rowsPerBlockCount[page_index])
                {
                    bufferManager.writePage(this->tableName + "_1", page_index, rowsInPage, rowsInPage.size());
                    block_no_Vector.emplace_back(page_index);
                    // cout << page_index << " at 1" << endl;
                    page_index++;

                    rowsInPage.resize(0);
                }

                logger.log("Popped " + to_string(popped_ele.first));
                auto temp_row = cursorVector[popped_ele.first]->getNextMatrixRow();
                if (temp_row.empty())
                {
                    indices[popped_ele.first]++;
                    // cout << "indices";
                    // for (int ei : indices)
                    //     cout << ei << " ";
                    // cout << endl;
                    int pass_block_index = i + popped_ele.first;
                    if (pass_block_index < passBlockPagesVector.size() && indices[popped_ele.first] < passBlockPagesVector[pass_block_index].size())
                    {
                        // get the next cursor
                        Cursor *c = new Cursor(this->tableName, passBlockPagesVector[pass_block_index][indices[popped_ele.first]]);
                        cursorVector[popped_ele.first] = c;
                        logger.log("Cursor for page stored in vector: " + to_string(passBlockPagesVector[pass_block_index][indices[popped_ele.first]]));
                        pq.push({popped_ele.first, cursorVector[popped_ele.first]->getNextMatrixRow()});
                        row_index++;
                    }
                    continue;
                }
                pq.push({popped_ele.first, temp_row});
                row_index++;
            }

            while (!pq.empty())
            {
                rowsInPage.emplace_back(pq.top().second);
                pq.pop();
            }
            bufferManager.writePage(this->tableName + "_1", page_index, rowsInPage, rowsInPage.size());
            rowsInPage.resize(0);
            block_no_Vector.emplace_back(page_index);
            // cout << page_index << " at 2" << endl;

            page_index++;
            temp_passBlockPagesVector.emplace_back(block_no_Vector);
        }

        passBlockPagesVector = temp_passBlockPagesVector;
        // cout << passBlockPagesVector.size() << " size" << endl;
        passBlockSize *= passBlockSize;
        // deleting previous blocks and renaming all the blocks from table_1 to table
        for (int i = 0; i < this->blockCount; i++)
        {
            string fromName = "../data/temp/" + this->tableName + "_1_Page" + to_string(i);
            string toName = "../data/temp/" + this->tableName + "_Page" + to_string(i);
            rename(fromName.c_str(), toName.c_str());
            bufferManager.removeFromPool(this->tableName, i);
        }

        if (passBlockPagesVector.size() == 1)
            break;
    }
    this->makePermanent();
    
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
