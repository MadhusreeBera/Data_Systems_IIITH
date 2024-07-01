#include"syntacticParser.h"

bool semanticParse();

bool semanticParseCLEAR();
bool semanticParseCROSS();
bool semanticParseDISTINCT();
bool semanticParseEXPORT();
bool semanticParseINDEX();
bool semanticParseJOIN();
bool semanticParseLIST();
bool semanticParseLOAD();
bool semanticParsePRINT();
bool semanticParsePROJECTION();
bool semanticParseRENAME();
bool semanticParseSELECTION();
bool semanticParseSORT();
bool semanticParseSOURCE();

//matrix

bool semanticParseEXPORTMATRIX();
bool semanticParseLOADMATRIX();
bool semanticParsePRINTMATRIX();
bool semanticParseRENAMEMATRIX();
//<<<<<<< Transpose
bool semanticParseTRANSPOSEMATRIX();
//=======
bool semanticParseCHECKSYMMETRY();
bool semanticParseCOMPUTE();
//>>>>>>> blockstore
