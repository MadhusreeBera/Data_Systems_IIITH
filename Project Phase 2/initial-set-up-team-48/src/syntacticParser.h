#include "tableCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    EXPORTMATRIX,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    LOADMATRIX,
    PRINT,
    PRINTMATRIX,
    PROJECTION,
    RENAME,
    RENAMEMATRIX,
    SELECTION,
    SORT,
    SOURCE,
    TRANSPOSEMATRIX,
    CHECKSYMMETRY,
    COMPUTE,
    ORDERBY,
    GROUP_BY,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum OperationType
{
    MAX,
    MIN,
    SUM,
    AVG,
    NO_OPERATION_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";
    string joinBufferSize = "10";

    string loadRelationName = "";

    string printRelationName = "";

    string transposeRelatmatrixionName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    string renameFromRelationName = "";
    string renameToRelationName = "";
    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";

    vector<string> sortColumnArray;
    vector<bool> sortAscendingArray; //1 for ascending
    vector<int> sortColumnIndex;

    string sourceFileName = "";

    string checksymmetryMatrix = "";
    string computeMatrix = "";
    string transposeMatrix = "";

    string orderResultantRelation="";
    string orderColumnName="";
    bool orderbyorder=""; //0 for ascending, 1 for descending
    string orderRelationName="";

    string groupByResultRelationName = "";
    string groupByAttributeName = "";
    string groupByRelationName = "";
    OperationType groupByOperation = NO_OPERATION_CLAUSE;
    string groupByOperationOnAttributeName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseEXPORTMATRIX();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseLOADMATRIX();
bool syntacticParsePRINT();
bool syntacticParsePRINTMATRIX();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseTRANSPOSEMATRIX();
bool syntacticParseRENAMEMATRIX();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();
bool syntacticParseORDER();
bool syntacticParseGROUP_BY();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
