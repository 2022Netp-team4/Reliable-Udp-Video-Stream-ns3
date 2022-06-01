#include <stdint.h>

namespace ns3 {
    
    ReliableUdpHeader::ReliableUdpHeader(){
        m_sourcePort (0xfffd),
        m_destinationPort (0xfffd),
        m_seqNum(0),
        m_ackNum(0),
        m_signal(0),
        m_isRetransmit(0)
    }
    ReliableUdpHeader::~ReliableUdpHeader(){
        NS_LOG_FUNCTION(this);
    }

    void 
    UdpHeader::SetDestinationPort (uint16_t port)
    {
    m_destinationPort = port;
    }
    void 
    UdpHeader::SetSourcePort (uint16_t port)
    {
    m_sourcePort = port;
    }
    uint16_t 
    UdpHeader::GetSourcePort (void) const
    {
    return m_sourcePort;
    }
    uint16_t 
    UdpHeader::GetDestinationPort (void) const
    {
    return m_destinationPort;
    }
    
    TypeId 
     Header::GetTypeId(void){
        static TypeId tid = TypeId("ns3::ReliableUdpHeader")
        .SetParent<Header> ()
        .SetGroupName("Headers")
        .AddConstructor<ReliableUdpHeader>()
    }
    return tid;


    TypeId 
     ReliableUdpHeader::GetInstanceTypeId (void) const
    {
    return GetTypeId ();
    }

    void 
    ReliableUdpHeader::Print (std::ostream &os) const
    {
    os << "header length: " << GetSerializedSize ()
        << " " 
        << m_sourcePort << " > " << m_destinationPort
        << "AckNum "<< m_ackNum << "SeqNum" << m_seqNum << "Retransmit" << m_isRetransmit << "Signal" << m_signal
    ;
    }
    uint32_t 
    UdpHeader::GetSerializedSize (void) const
    {
        return 8; // ????

    }

    void
    UdpHeader::Serialize (Buffer::Iterator start) const
    {
    Buffer::Iterator i = start;
    
    i.WriteHtonU16 (m_sourcePort);
    i.WriteHtonU16 (m_destinationPort);
    if(m_seqNum == 0){
        i.WriteU16(0);
    }
    else{
        i.WriteU16 (m_seqNum);
    }
    
    if(m_ackNum == 0){
        i.WriteU16(0);
    }
    else{
        i.WriteU16 (m_ackNum);
    }
    if(m_signal == 0){
        i.WriteU16(0);
    }
    else{
        i.WriteU16 (m_signal);
    }
    if(m_isRetransmit == 0){
        i.WriteU16(0);
    }
    else{
        i.WriteU16 (m_isRetransmit);
    }

    
    
    
    }

    uint32_t
    UdpHeader::Deserialize (Buffer::Iterator start)
    {
    Buffer::Iterator i = start;
    m_sourcePort = i.ReadNtohU16 ();
    m_destinationPort = i.ReadNtohU16 ();
    
    m_seqNum = i.ReadU16 ();
    m_ackNum = i.ReadU16 ();
    m_signal = i.ReadU16 ();
    m_isRetransmit = i.ReadU16 ();
    
    
    
    return GetSerializedSize ();
    }

    


    void SetSeqNum (uint32_t seqNum){
        m_seqNum = seqNum
    }

    void SetAckNum (uint32_t ackNum){
        m_ackNum = ackNum
    
    }

    void SetSignal (uint8_t signal){
        m_signal = signal
        
    }

    void SetRetransmit (uint8_t isRetransmit){
        m_isRetransmit = isRetransmit
    }

    uint32_t GetSeqNum (){
        return m_seqNum;
    }

    uint32_t GetAckNum (){
        return m_ackNum;
    }

    uint8_t GetSignal (){
        return m_signal
    }

    uint8_t GetRetransmit (){
        return m_isRetransmit;
    }
}