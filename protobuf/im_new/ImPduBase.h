#ifndef IMPDUBASE_H_
#define IMPDUBASE_H_

#include <google/protobuf/message_lite.h>

#include "ostype.h"
#include "UtilPdu.h"

#define IM_PDU_HEADER_LEN		16
#define IM_PDU_VERSION			1


//////////////////////////////
typedef struct {
    uint32_t 	length;		  // the whole pdu length
    uint16_t 	version;	  // pdu version number
    uint16_t	flag;		  // not used
    uint16_t	service_id;	  // 
    uint16_t	command_id;	  // 
    uint16_t	seq_num;     // 包序号
    uint16_t    reversed;    // 保留
} PduHeader_t;


class CImPdu
{
public:
    CImPdu();
    virtual ~CImPdu() {}
    
    uchar_t* GetBuffer();
    uint32_t GetLength();
    uchar_t* GetBodyData();
    uint32_t GetBodyLength();
    
    
    uint16_t GetVersion() { return m_pdu_header.version; }
    uint16_t GetFlag() { return m_pdu_header.flag; }
    uint16_t GetServiceId() { return m_pdu_header.service_id; }
    uint16_t GetCommandId() { return m_pdu_header.command_id; }
    uint16_t GetSeqNum() { return m_pdu_header.seq_num; }
    uint32_t GetReversed() { return m_pdu_header.reversed; }
    
    void SetVersion(uint16_t version);
    void SetFlag(uint16_t flag);
    void SetServiceId(uint16_t service_id);
    void SetCommandId(uint16_t command_id);
    void SetError(uint16_t error);
    void SetSeqNum(uint16_t seq_num);
    void SetReversed(uint32_t reversed);
    void WriteHeader();
    
    static bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len);
    static CImPdu* ReadPdu(uchar_t* buf, uint32_t len);
    void Write(uchar_t* buf, uint32_t len) { m_buf.Write((void*)buf, len); }
    int ReadPduHeader(uchar_t* buf, uint32_t len);
    void SetPBMsg(const google::protobuf::MessageLite* msg);
    
protected:
    CSimpleBuffer	m_buf;		// 缓存buffer， m_pdu_header + PB序列化后的数据
    PduHeader_t		m_pdu_header;
};


#endif	

