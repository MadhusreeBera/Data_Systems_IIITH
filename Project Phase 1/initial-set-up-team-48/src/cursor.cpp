#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}
Cursor::Cursor(string tableName, int pageIndex, bool isMatrix){
    logger.log("Cursor::Cursor " + tableName + "_Page"+ std::to_string(pageIndex));
    this->page = bufferManager.getPage(tableName, pageIndex,isMatrix);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}
/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
vector<int> Cursor::getNextMatrixRow(){
    logger.log("Cursor::getNextMatrixRow: page: "+ to_string(this->pageIndex)+"  Row " + to_string(this->pagePointer));
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    return result;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}