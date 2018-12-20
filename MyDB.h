#ifndef CHAT_MYDB_H
#define CHAT_MYDB_H

#include <mysql/mysql.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

namespace zlb_mysql {

    class CField {
    public:
        vector<string> m_name;
        vector<enum_field_types> m_type;
    public:
        CField();

        ~CField();

        bool IsNum(int nNum);

        bool IsNum(string strNum);

        bool IsData(int nData);

        bool IsData(string strData);

        bool IsChar(int nChar);

        bool IsChar(string strChar);

        bool IsBlob(int nBlob);

        bool IsBlob(string strBlob);

        int GetField_NO(string str_Field_name);

    };


    class CRecord {
    public:

        vector<string> m_rs;
        CField *m_field;
    public :
        CRecord() {};

        CRecord(CField *m_f);

        ~CRecord();


        void SetData(string value);

        /* [""]操作 */
        string operator[](string str);

        string operator[](int nNum);

        bool IsNull(int nNum);

        bool IsNull(string str);

        /* 用 value tab value 的形式 返回结果 */
        string GetTabText();
    };


    class CRecordSet {
    private:
        vector<CRecord> m_s;
        unsigned long m_pos;
        int m_nRecordConut;
        int m_nFieldNum;
        CField m_field;

        MYSQL_RES *m_pRes;
        MYSQL_FIELD *m_pFd;
        MYSQL_ROW m_Row;
        MYSQL *m_pData;
    public:
        CRecordSet();

        CRecordSet(MYSQL *hSQL);

        ~CRecordSet();

        void GetRecordSet(MYSQL *hSQL);

        int ExecuteSQL(const char *SQL);

        int GetRecordCount();

        int GetFieldNum();

        int MoveNext();

        long Move(long length);

        bool MoveFirst();

        bool MoveLast();

        unsigned long GetCurrentPos() const; //const 类的成员不能被改变

        bool GetCurrentFieldValue(const char *szFieldName, char *szValue);

        bool GetCurrentFieldValue(const int nFieldNum, char *szValue);

        bool GetFieldValue(long index, const char *szFieldName, char *szValue);

        bool GetFieldValue(long index, int nFieldNum, char *szValue);

        /* 是否到达游标尾部 */
        bool IsEof();

        CField *GetField();

        const char *GetFieldName(int nNum);

        const int GetFieldType(char *szName);

        const int GetFieldType(int nNum);

        CRecord operator[](int nNum);

    };


    class CDataBase {
    public:
        CDataBase();

        ~CDataBase();

    private:
        MYSQL *m_pData;
    public:
        MYSQL *Get_Mysql();

        int Connect(string host, string user,
                    string passwd, string db);

        void DisConnect();

        int ExecQuery(string sql);

        int Ping();

        int ShutDown();

        int ReBoot();

        int Start_Transaction();

        int Commit();

        int RollBack();

        const char *Get_Client_Info();

        const unsigned long Get_client_version();

        const char *Get_Host_Info();

        const char *Get_Server_Info();

        const char *Get_CharSet_Name();

        char *ExecQueryGetSingValue(string SQL);

        const char *GetSysTime();

        int Create_db(string strName);

        int Drop_db(string strName);

        bool IsDbConnected();

    private:
        bool m_bConnected;
    };
}

#endif //CHAT_MYDB_H
