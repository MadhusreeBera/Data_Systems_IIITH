#include"global.h"

void executeCommand(){
    // cout << "executing" << parsedQuery.queryType <<endl;
    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case PRINT: executePRINT(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;

        // matrix cases:
        case LOADMATRIX: executeLOADMATRIX(); break;
        case PRINTMATRIX: executePRINTMATRIX(); break;
        case EXPORTMATRIX: executeEXPORTMATRIX(); break;
        case RENAMEMATRIX: executeRENAMEMATRIX(); break;
        case TRANSPOSEMATRIX: executeTRANSPOSEMATRIX(); break;
        case CHECKSYMMETRY: executeCHECKSYMMETRY(); break;
        case COMPUTE: executeCOMPUTE();break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}