#ifndef __JOFS_FILE_READER_H_
#define __JOFS_FILE_READER_H_
#include "j_includes.h"
#include "x_module_manager_def.h"
#include "x_time.h"
#include "x_file.h"
#include "x_lock.h"
#include "x_timer.h"
#include <pthread.h>

class CNvrFileReader : public J_FileReader
{
public:
	CNvrFileReader(const char *pResid);
	~CNvrFileReader();

    static int Maker(J_Obj *&pObj, const char *pResid)
	{
		pObj = new CNvrFileReader(pResid);
		return J_OK;
	}

public:
	virtual int GetContext(J_MediaContext *&mediaContext);
	virtual int GetPacket(char *pBuffer, J_StreamHeader &streamHeader);
	virtual int Pause();
	virtual int SetScale(float nScale = 1);
	virtual int SetTime(uint64_t s_time, uint64_t e_time);
	virtual int SetPosition(int nPos);
	virtual int GetMediaData(j_uint64_t beginTime, int nIval);

private:
	int ListRecord(uint64_t beginTime, uint64_t endTime);
	int OpenFile();
	void CloseFile();
	int CalcPosition(uint64_t timeStamp, uint32_t interval = 120);

	static void TimerThread(unsigned long ulUser)
	{
		CNvrFileReader *pThis = static_cast<CNvrFileReader *>((void *)ulUser);
		if (pThis != NULL)
			pThis->OnTimer();
	}
	void OnTimer();
	static void *WorkThread(void *pUser)
	{
		CNvrFileReader *pThis = (CNvrFileReader *)pUser;
			pThis->OnWork();
		return (void *)0;
	}
	void OnWork();

private:
	typedef std::list<std::string> RecordMap;
	RecordMap m_fileVec;
	typedef std::map<uint64_t, J_FrameHeader> FrameMap;
	FrameMap m_frameMap;
	FrameMap m_iFrameMap;
	std::string m_resid;
	J_OS::CTimer m_timer;
	J_OS::TLocker_t m_locker;

	CXFile m_file;
	FILE *m_pFileId;
	float m_nScale;
	bool m_bPaused;
	bool m_bGoNext;
	long m_fileEnd;
	J_StreamHeader m_nextHeader;
	volatile uint64_t m_nextTimeStamp;
	
	pthread_t m_thread;
	pthread_mutex_t m_mux;
	pthread_cond_t m_cond;
	int m_lastTime;
	CRingBuffer *m_buffer;
	bool m_bRun;
};

FILEREADER_BEGIN_MAKER(jofs)
	FILEREADER_ENTER_MAKER("jofs", CNvrFileReader::Maker)
FILEREADER_END_MAKER()

#endif //~__JOFS_FILE_READER_H_
