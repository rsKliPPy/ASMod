#ifndef GAME_SERVER_ANGELSCRIPT_SCRIPTAPI_SQL_ASHLSQL_H
#define GAME_SERVER_ANGELSCRIPT_SCRIPTAPI_SQL_ASHLSQL_H

class asIScriptEngine;
class CASSQLThreadPool;

extern CASSQLThreadPool* g_pSQLThreadPool;

void RegisterScriptHLSQL( asIScriptEngine& engine );

#endif //GAME_SERVER_ANGELSCRIPT_SCRIPTAPI_SQL_ASHLSQL_H