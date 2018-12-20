//#include "StdAfx.h"
#include "MyDB.h"
#include "string.h"

namespace zlb_mysql {


    CField::CField() {

    }

    CField::~CField() {

    }

    bool CField::IsNum(int nNum) {
        if (IS_NUM(m_type[nNum])) {
            return true;
        }
        return false;
    }

    bool CField::IsNum(string strNum) {
        if (IS_NUM(m_type[GetField_NO(strNum)])) {
            return true;
        }
        return false;
    }

    bool CField::IsData(int nData) {
        if (
                FIELD_TYPE_DATE == m_type[nData] ||
                FIELD_TYPE_DATETIME == m_type[nData]
                ) {
            return true;
        }
        return false;

    }

    bool CField::IsData(string strData) {
        int temp;
        temp = GetField_NO(strData);
        if (
                FIELD_TYPE_DATE == m_type[temp] ||
                FIELD_TYPE_DATETIME == m_type[temp]
                ) {
            return true;
        }
        return false;

    }

    bool CField::IsChar(int nChar) {
        if (
                m_type[nChar] == FIELD_TYPE_STRING ||
                m_type[nChar] == FIELD_TYPE_VAR_STRING ||
                m_type[nChar] == FIELD_TYPE_CHAR
                ) {
            return true;
        }
        return false;


    }

    bool CField::IsChar(string strChar) {
        int temp;
        temp = this->GetField_NO(strChar);
        if (
                m_type[temp] == FIELD_TYPE_STRING ||
                m_type[temp] == FIELD_TYPE_VAR_STRING ||
                m_type[temp] == FIELD_TYPE_CHAR
                ) {
            return true;
        }
        return false;

    }

    bool CField::IsBlob(int nBlob) {
        if (IS_BLOB(m_type[nBlob]))
            return true;
        else
            return false;

    }

    bool CField::IsBlob(string strBlob) {
        if (IS_BLOB(m_type[GetField_NO(strBlob)]))
            return true;
        else
            return false;

    }

    int CField::GetField_NO(string str_Field_name) {

        for (unsigned int i = 0; i < m_name.size(); i++) {
            if (!m_name[i].compare(str_Field_name)) {
                return i;
            }
        }
        return -1;

    }


    CRecord::CRecord(CField *m_f) {
        m_field = m_f;
    }

    CRecord::~CRecord() {

    }


    void CRecord::SetData(string value) {
        m_rs.push_back(value);
    }

/* [""]操作 */
    string CRecord::operator[](string str) {
        return m_rs[m_field->GetField_NO(str)];
    }

    string CRecord::operator[](int nNum) {
        return m_rs[nNum];
    }

    bool CRecord::IsNull(int nNum) {
        if ("" == m_rs[nNum].c_str()) {
            return true;
        }
        return false;
    }

    bool CRecord::IsNull(string str) {
        if ("" == m_rs[m_field->GetField_NO(str)].c_str()) {
            return true;
        }
        return false;
    }

/* 用 value tab value 的形式 返回结果 */
    string CRecord::GetTabText() {
        string temp;
        for (vector<string>::size_type i = 0; i < m_rs.size(); i++) {
            temp += m_rs[i];
            if (i < m_rs.size() - 1) {
                temp += "\t";
            }
        }
        return temp;
    }


    CRecordSet::CRecordSet() {
        m_Row = NULL;
        m_pRes = NULL;
        m_pos = 0;
    }

    CRecordSet::CRecordSet(MYSQL *hSQL) {
        m_pos = 0;
        m_pRes = NULL;
        m_Row = NULL;
        m_pData = hSQL;
    }

    CRecordSet::~CRecordSet() {

    }

    void CRecordSet::GetRecordSet(MYSQL *hSQL) {
        m_pData = hSQL;
    }


    int CRecordSet::ExecuteSQL(const char *SQL) {
            if (!mysql_real_query(m_pData, SQL, strlen(SQL))) {
            if (!m_s.empty()) {
                m_s.clear();
                m_field.m_name.clear();
                m_field.m_type.clear();
            }
            m_pRes = mysql_store_result(m_pData);
            m_nRecordConut = (int) mysql_num_rows(m_pRes);
            m_nFieldNum = mysql_num_fields(m_pRes);
            for (int x = 0; m_pFd = mysql_fetch_field(m_pRes); x++) {
                m_field.m_name.push_back(m_pFd->name);
                m_field.m_type.push_back(m_pFd->type);
            }

            while (m_Row = mysql_fetch_row(m_pRes)) {
                CRecord temp(&m_field);
                for (int k = 0; k < m_nFieldNum; k++) {
                    if (m_Row[k] == NULL || (!strlen(m_Row[k]))) {
                        temp.SetData("");
                    } else {
                        temp.SetData(m_Row[k]);
                    }
                }
                m_s.push_back(temp);
            }
            mysql_free_result(m_pRes);
            return m_s.size();

        }
        return -1;
    }

    int CRecordSet::GetRecordCount() {
        return m_nRecordConut;
    }

    int CRecordSet::GetFieldNum() {
        return m_nFieldNum;
    }

    int CRecordSet::MoveNext() {
        return ++m_pos;
    }

    long CRecordSet::Move(long length) {
        int l = m_pos + length;
        if (l < 0) {
            m_pos = 0;
            return 0;
        } else {
            if (l > m_s.size()) {
                m_pos = m_s.size() - 1;
                return m_pos;
            } else {
                m_pos = l;
                return m_pos;
            }
        }
    }

    bool CRecordSet::MoveFirst() {
        m_pos = 0;
        return true;
    }

    bool CRecordSet::MoveLast() {
        m_pos = m_s.size() - 1;
        return true;
    }

    unsigned long CRecordSet::GetCurrentPos() const //const 类的成员不能被改变
    {
        return m_pos;
    }

    bool CRecordSet::GetCurrentFieldValue(const char *szFieldName, char *szValue) {
        strcpy(szValue, m_s[m_pos][szFieldName].c_str());
        return true;
    }

    bool CRecordSet::GetCurrentFieldValue(const int nFieldNum, char *szValue) {
        strcpy(szValue, m_s[m_pos][nFieldNum].c_str());
        return true;
    }

    bool CRecordSet::GetFieldValue(long index, const char *szFieldName, char *szValue) {
        strcpy(szValue, m_s[index][szFieldName].c_str());
        return true;
    }

    bool CRecordSet::GetFieldValue(long index, int nFieldNum, char *szValue) {
        strcpy(szValue, m_s[index][nFieldNum].c_str());
        return true;
    }

/* 是否到达游标尾部 */
    bool CRecordSet::IsEof() {
        return (m_pos == m_s.size()) ? true : false;
    }

    CField *CRecordSet::GetField() {
        return &m_field;
    }

    const char *CRecordSet::GetFieldName(int nNum) {
        return m_field.m_name.at(nNum).c_str();
    }

    const int CRecordSet::GetFieldType(char *szName) {
        int i = m_field.GetField_NO(szName);
        return m_field.m_type.at(i);
    }

    const int CRecordSet::GetFieldType(int nNum) {
        return m_field.m_type.at(nNum);
    }

    CRecord CRecordSet::operator[](int nNum) {
        return m_s[nNum];
    }


    CDataBase::CDataBase() {
        m_pData = NULL;
        m_bConnected = false;
    }

    CDataBase::~CDataBase() {
        if (NULL != m_pData) {
            DisConnect();
        }
    }

    MYSQL *CDataBase::Get_Mysql() {
        return m_pData;
    }


    int CDataBase::Connect(string host, string user,
                           string passwd, string db) {
        if (
                (m_pData = mysql_init(NULL)) &&
                mysql_real_connect(
                        m_pData, host.c_str(),
                        user.c_str(), passwd.c_str(),
                        db.c_str(), MYSQL_PORT, NULL, 0
                )
                ) {
            if (mysql_select_db(m_pData, db.c_str()) < 0) {
                mysql_errno(m_pData);
                mysql_close(m_pData);
                return -1;
            }
        } else {
            mysql_errno(m_pData);
            mysql_close(m_pData);
            return -1;
        }

        m_bConnected = true;
        return 0;
    }

    void CDataBase::DisConnect() {
        if (m_bConnected) {
            mysql_close(m_pData);
            m_bConnected = false;
        }
    }

    bool CDataBase::IsDbConnected() {
        return m_bConnected;
    }

    int CDataBase::ExecQuery(string sql) {
        string strSql = "SET NAMES 'gb2312'";
        if (mysql_real_query(m_pData, strSql.c_str(), (unsigned long) strSql.length())) {
            printf("%s", "ExecQuery 中文乱码");
        }
        if (!mysql_real_query(m_pData, sql.c_str(), (unsigned long) sql.length())) {
            //得到受影响的行数
            return (int) mysql_affected_rows(m_pData);
        } else {
            mysql_errno(m_pData);
            //执行查询失败
            return -1;
        }
    }

    int CDataBase::Ping() {
        if (!mysql_ping(m_pData)) {
            return 0;
        }
        return -1;
    }

    int CDataBase::ShutDown() {
        if (!mysql_shutdown(m_pData, SHUTDOWN_DEFAULT)) {
            return 0;
        }
        return -1;
    }

    int CDataBase::ReBoot() {
        if (!mysql_reload(m_pData)) {
            return 0;
        }
        return -1;
    }

    int CDataBase::Start_Transaction() {
        if (
                !mysql_real_query(m_pData, "START TRANSACTION",
                                  (unsigned long) strlen("START TRANSACTION"))
                ) {
            return 0;
        } else {
            return -1;
        }
    }

    int CDataBase::Commit() {
        if (
                !mysql_real_query(m_pData, "COMMIT",
                                  (unsigned long) strlen("COMMIT"))
                ) {
            return 0;
        } else {
            return -1;
        }
    }

    int CDataBase::RollBack() {
        if (
                !mysql_real_query(m_pData, "ROLLBACK",
                                  (unsigned long) strlen("ROLLBACK"))
                ) {
            return 0;
        } else {
            return -1;
        }
    }

    const char *CDataBase::Get_Client_Info() {
        return mysql_get_client_info();
    }

    const unsigned long CDataBase::Get_client_version() {
        return mysql_get_client_version();
    }

    const char *CDataBase::Get_Host_Info() {
        return mysql_get_host_info(m_pData);
    }

    const char *CDataBase::Get_Server_Info() {
        return mysql_get_server_info(m_pData);
    }

    const char *CDataBase::Get_CharSet_Name() {
        return mysql_character_set_name(m_pData);
    }

    char *CDataBase::ExecQueryGetSingValue(string SQL) {
        MYSQL_RES *res;
        MYSQL_ROW row;
        char *p = NULL;
        if (!mysql_real_query(m_pData, SQL.c_str(), (unsigned long) SQL.length())) {
            res = mysql_store_result(m_pData);
            row = mysql_fetch_row(res);
            if (row == NULL || (row[0] == NULL) || (!strlen(row[0])))
                p = NULL;
            else
                p = const_cast<char *>(row[0]);
            //p = const_cast<char*>(((row[0]==NULL)||(!strlen(row[0])))?"-1":row[0]);
            mysql_free_result(res);
        } else {
            mysql_errno(m_pData);
        }
        return p;
    }

    const char *CDataBase::GetSysTime() {
        return ExecQueryGetSingValue("select now()");
    }

    int CDataBase::Create_db(string strName) {
        string temp;
        temp = "CREATE DATABASE ";
        temp += strName;
        if (!mysql_real_query(
                m_pData, strName.c_str(),
                (unsigned long) temp.length()
        )
                )
            return 0;

        else
            //执行查询失败
            return -1;
    }

    int CDataBase::Drop_db(string strName) {
        string temp;
        temp = "DROP DATABASE ";
        temp += strName;
        if (!mysql_real_query(m_pData, temp.c_str(),
                              (unsigned long) temp.length())) {
            return 0;
        } else {    //执行查询失败
            return -1;
        }
    }

};

