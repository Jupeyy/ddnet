/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "network.h"
#include <base/system.h>
#include <functional>

CNetClient::~CNetClient()
{
	Close();
}

bool CNetClient::Open(NETADDR BindAddr, int Flags)
{
	// open socket
	NETSOCKET Socket;
	Socket = net_udp_create(BindAddr);
	if(!Socket.type)
		return false;

	// init
	m_Socket = Socket;
	m_Connection.Init(m_Socket, false);
	net_init_mmsgs(&m_MMSGS);

	m_CanRecv = false;
	m_ThreadFinished = false;
	m_pRecvThread = new std::thread(std::bind(&CNetClient::RecvThread, this));

	return true;
}

int CNetClient::Close()
{
	// TODO: implement me
	m_ThreadFinished = true;

	net_udp_close(m_Socket);

	if(m_pRecvThread)
	{
		{
			std::unique_lock<std::mutex> Lock(m_RecvMutex);
			m_CanRecv = false;
			m_Cond.notify_all();
		}
		m_pRecvThread->join();
		delete m_pRecvThread;
	}

	return 0;
}

int CNetClient::Disconnect(const char *pReason)
{
	//dbg_msg("netclient", "disconnected. reason=\"%s\"", pReason);
	m_Connection.Disconnect(pReason);
	return 0;
}

int CNetClient::Update()
{
	m_Connection.Update();
	if(m_Connection.State() == NET_CONNSTATE_ERROR)
		Disconnect(m_Connection.ErrorString());
	return 0;
}

int CNetClient::Connect(NETADDR *pAddr)
{
	m_Connection.Connect(pAddr);
	return 0;
}

int CNetClient::ResetErrorString()
{
	m_Connection.ResetErrorString();
	return 0;
}

void CNetClient::RecvThread()
{
	while(!m_ThreadFinished)
	{
		if(net_udp_select(m_Socket, 100000))
		{
			std::unique_lock<std::mutex> Lock(m_RecvMutex);
			m_CanRecv = true;
			if(!m_ThreadFinished)
				m_Cond.wait(Lock);
		}
	}
}

int CNetClient::Recv(CNetChunk *pChunk)
{
	while(1)
	{
		if(m_CanRecv)
		{
			// check for a chunk
			if(m_RecvUnpacker.FetchChunk(pChunk))
			{
				return 1;
			}

			NETADDR Addr;
			unsigned char *pData;
			int Bytes = net_udp_recv(m_Socket, &Addr, m_RecvUnpacker.m_aBuffer, NET_MAX_PACKETSIZE, &m_MMSGS, &pData);

			// no more packets for now
			if(Bytes <= 0)
			{
				std::unique_lock<std::mutex> Lock(m_RecvMutex);
				m_CanRecv = false;
				m_Cond.notify_all();

				break;
			}

			bool Sixup = false;
			if(CNetBase::UnpackPacket(pData, Bytes, &m_RecvUnpacker.m_Data, Sixup) == 0)
			{
				if(m_RecvUnpacker.m_Data.m_Flags & NET_PACKETFLAG_CONNLESS)
				{
					pChunk->m_Flags = NETSENDFLAG_CONNLESS;
					pChunk->m_ClientID = -1;
					pChunk->m_Address = Addr;
					pChunk->m_DataSize = m_RecvUnpacker.m_Data.m_DataSize;
					pChunk->m_pData = m_RecvUnpacker.m_Data.m_aChunkData;
					if(m_RecvUnpacker.m_Data.m_Flags & NET_PACKETFLAG_EXTENDED)
					{
						pChunk->m_Flags |= NETSENDFLAG_EXTENDED;
						mem_copy(pChunk->m_aExtraData, m_RecvUnpacker.m_Data.m_aExtraData, sizeof(pChunk->m_aExtraData));
					}
					return 1;
				}
				else
				{
					if(m_Connection.State() != NET_CONNSTATE_OFFLINE && m_Connection.State() != NET_CONNSTATE_ERROR && net_addr_comp(m_Connection.PeerAddress(), &Addr) == 0 && m_Connection.Feed(&m_RecvUnpacker.m_Data, &Addr))
						m_RecvUnpacker.Start(&Addr, &m_Connection, 0);
				}
			}
		}
		else
			break;
	}

	return 0;
}

int CNetClient::Send(CNetChunk *pChunk)
{
	if(pChunk->m_DataSize >= NET_MAX_PAYLOAD)
	{
		dbg_msg("netclient", "chunk payload too big. %d. dropping chunk", pChunk->m_DataSize);
		return -1;
	}

	if(pChunk->m_Flags & NETSENDFLAG_CONNLESS)
	{
		// send connectionless packet
		CNetBase::SendPacketConnless(m_Socket, &pChunk->m_Address, pChunk->m_pData, pChunk->m_DataSize,
			pChunk->m_Flags & NETSENDFLAG_EXTENDED, pChunk->m_aExtraData);
	}
	else
	{
		int Flags = 0;
		dbg_assert(pChunk->m_ClientID == 0, "errornous client id");

		if(pChunk->m_Flags & NETSENDFLAG_VITAL)
			Flags = NET_CHUNKFLAG_VITAL;

		m_Connection.QueueChunk(Flags, pChunk->m_DataSize, pChunk->m_pData);

		if(pChunk->m_Flags & NETSENDFLAG_FLUSH)
			m_Connection.Flush();
	}
	return 0;
}

int CNetClient::State()
{
	if(m_Connection.State() == NET_CONNSTATE_ONLINE)
		return NETSTATE_ONLINE;
	if(m_Connection.State() == NET_CONNSTATE_OFFLINE)
		return NETSTATE_OFFLINE;
	return NETSTATE_CONNECTING;
}

int CNetClient::Flush()
{
	return m_Connection.Flush();
}

int CNetClient::GotProblems()
{
	if(time_get() - m_Connection.LastRecvTime() > time_freq())
		return 1;
	return 0;
}

const char *CNetClient::ErrorString()
{
	return m_Connection.ErrorString();
}
