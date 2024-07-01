#include "cursor.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT).
 *
 */
class Table
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string tableName = "";
    vector<string> columns;
    vector<uint> distinctValuesPerColumnCount;
    uint columnCount = 0;
    long long int rowCount = 0;
    uint blockCount = 0;
    uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    vector<uint> columnsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;
    bool isMatrix = false;
    int matrixBlocks = 0;
    vector<bool> pagesym;
    vector<bool> pagetran;

    bool extractColumnNames(string firstLine);
    bool blockify();
    void updateStatistics(vector<int> row);
    Table();
    Table(string tableName);
    Table(Table *table, string tableName);
    Table(string tableName, bool isMatrix);
    Table(string tableName, vector<string> columns);
    bool load();
    bool isColumn(string columnName);
    void renameColumn(string fromColumnName, string toColumnName);
    void renameMatrix(string fromMatrix, string toMatrix);
    void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    void unload();
    // matrix
    bool loadMatrix();
    bool blockifyMatrix();
    void exportMatrix();
    void printMatrix();
    bool checkSymMatrix(string matrixName);
    void Transpose(string matrixName);
    void computeMatrix(string matrixName);
    void sortPages(vector<bool> sortingStrategy, vector<int> columnIndexes/*, string resultRelationName*/);

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Table::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }
    template <typename T>
    void writeRow(vector<T> row, ostream &fout, int columnCount)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

    /**
     * @brief Static function that takes a vector of valued and prints them out in a
     * comma seperated format.
     *
     * @tparam T current usaages include int and string
     * @param row
     */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Table::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
    void printBlockDetails(int blocksRead, int blocksWritten)
    {
        cout << "Number of blocks read: " << blocksRead << endl;
        cout << "Number of blocks written: " << blocksWritten << endl;
        cout << "Number of blocks accessed: " << blocksRead + blocksWritten << endl;
    }
};
