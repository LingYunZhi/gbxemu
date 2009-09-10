#pragma once

extern bool regEnabled;

char *regQueryStringValue(const char *key, char *def);
DWORD regQueryDwordValue(const char *key, DWORD def, bool force=false);
BOOL regQueryBinaryValue(const char *key, char *value, int count);
void regSetStringValue(const char *key,const char *value);
void regSetDwordValue(const char *key,DWORD value,bool force=false);
void regSetBinaryValue(const char *key, char *value, int count);
void regDeleteValue(char *key);
void regInit(const char *, bool force = false);
void regShutdown();
