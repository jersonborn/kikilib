//@Author Liu Yukang 
#include "EventService.h"
#include "EventManager.h"
#include "LogManager.h"

#include <poll.h>
#include <sys/epoll.h>

using namespace kikilib;

EventService::EventService(Socket& sock, EventManager* evMgr, int interestEvent)
	: _interestEvent(interestEvent), _eventState(0), _eventPriority(NORMAL_EVENT),
	_isConnected(true), _sock(sock), _pMyEvMgr(evMgr), _bufWritter(sock, this), _bufReader(sock)
{}

EventService::EventService(Socket&& sock, EventManager* evMgr, int interestEvent)
	: _interestEvent(interestEvent), _eventState(0), _eventPriority(NORMAL_EVENT),
	_isConnected(true), _sock(sock), _pMyEvMgr(evMgr), _bufWritter(sock, this), _bufReader(sock)
{}

void EventService::Close()
{
    if(_isConnected)
    {
        _isConnected = false;
        HandleCloseEvent();
        _pMyEvMgr->Remove(this);
    }

}

void EventService::ShutDownWrite()
{
	_sock.ShutdownWrite();
}

void EventService::SetInteresEv(int newInterestEv)
{
	_interestEvent = newInterestEv;
	_pMyEvMgr->Motify(this);
}

//���¼��������в���һ���¼�,�����̰߳�ȫ��
void EventService::Insert(EventService* ev)
{
	_pMyEvMgr->Insert(ev);
}

//���¼����������Ƴ�һ���¼�,�����̰߳�ȫ��
void EventService::Remove(EventService* ev)
{
	_pMyEvMgr->Remove(ev);
}

//���¼����������޸�һ���¼���������ע���¼�����,�����̰߳�ȫ��
void EventService::Motify(EventService* ev)
{
	_pMyEvMgr->Motify(ev);
}

//���EventManager����Ψһ������������
void* EventService::GetEvMgrCtx()
{
	return _pMyEvMgr->GetEvMgrCtx();
}

//�����¼����ʹ����¼�
void EventService::HandleEvent()
{
	if ((_eventState & EPOLLHUP) && !(_eventState & EPOLLIN))
	{
		Close();
	}

	if (_eventState & (EPOLLERR))
	{
        RecordLog(ERROR_DATA_INFORMATION, "error event in HandleEvent()!");
        HandleErrEvent();
        Close();
	}
	if (_eventState & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (!_bufReader.IsEmptyAfterRead())
		{
			HandleReadEvent();
		}
		else
		{
			Close();
		}
		/*else
		{
			RecordLog(ERROR_DATA_INFORMATION, "read fd error!");
			HandleErrEvent();
		}*/
	}
	if (_eventState & EPOLLOUT)
	{
		HandleWriteEvent();
	}
}

void EventService::HandleWriteEvent()
{
	_bufWritter.WriteBufToSock();
}

//дһ��int
bool EventService::WriteInt32(int num)
{
	return _bufWritter.SendInt32(num);
}

bool EventService::WriteBuf(std::string& content)
{
	return _bufWritter.Send(content);
}

bool EventService::WriteBuf(std::string&& content)
{
	return _bufWritter.Send(std::move(content));
}

//��ȡһ��int����������û�У��򷵻�false
bool EventService::ReadInt32(int& res)
{
	return _bufReader.ReadInt32(res);
}

std::string EventService::ReadBuf(size_t len)
{
	return _bufReader.Read(len);
}

//��ȡ����Ϊlen�����ݣ���û�г���Ϊlen�����ݣ��򷵻�false
bool EventService::ReadBuf(char* buf, size_t len)
{
	return _bufReader.Read(buf, len);
}

std::string EventService::ReadAll()
{
	return _bufReader.ReadAll();
}

//��һ�У�������\r\n��β,��û�У����ؿմ�
std::string EventService::ReadLineEndOfRN()
{
	return _bufReader.ReadLineEndOfRN();
}

//��һ�У�������\r��β,��û�У����ؿմ�
std::string EventService::ReadLineEndOfR()
{
	return _bufReader.ReadLineEndOfR();
}

//��һ�У�������\n��β,��û�У����ؿմ�
std::string EventService::ReadLineEndOfN()
{
	return _bufReader.ReadLineEndOfN();
}

void EventService::RunAt(Time time, std::function<void()>&& timerCb)
{
	_pMyEvMgr->RunAt(time, std::move(timerCb));
}

void EventService::RunAt(Time time, std::function<void()>& timerCb)
{
	_pMyEvMgr->RunAt(time, timerCb);
}

//timeʱ���ִ��timerCb����
void EventService::RunAfter(Time time, std::function<void()>&& timerCb)
{ 
	_pMyEvMgr->RunAfter(time, std::move(timerCb));
}

//timeʱ���ִ��timerCb����
void EventService::RunAfter(Time time, std::function<void()>& timerCb)
{
	_pMyEvMgr->RunAfter(time, timerCb);
}

//ÿ��timeʱ��ִ��timerCb����
void EventService::RunEvery(Time time, std::function<void()>&& timerCb)
{
	_pMyEvMgr->RunEvery(time, timerCb);
}

void EventService::RunEvery(Time time, std::function<void()>& timerCb)
{
	_pMyEvMgr->RunEvery(time, timerCb);
}

//ÿ��timeʱ��ִ��һ��timerCb����,ֱ��isContinue��������false
void EventService::RunEveryUntil(Time time, std::function<void()>& timerCb, std::function<bool()>& isContinue)
{
	_pMyEvMgr->RunEveryUntil(time, timerCb, isContinue);
}

void EventService::RunEveryUntil(Time time, std::function<void()>& timerCb, std::function<bool()>&& isContinue)
{
	_pMyEvMgr->RunEveryUntil(time, timerCb, isContinue);
}

void EventService::RunEveryUntil(Time time, std::function<void()>&& timerCb, std::function<bool()>& isContinue)
{
	_pMyEvMgr->RunEveryUntil(time, timerCb, isContinue);
}

void EventService::RunEveryUntil(Time time, std::function<void()>&& timerCb, std::function<bool()>&& isContinue)
{
	_pMyEvMgr->RunEveryUntil(time, timerCb, isContinue);
}

//���������Ѿ���ʱ����Ҫִ�еĺ���
void EventService::RunExpired()
{
	_pMyEvMgr->RunExpired();
}

//����������̳߳����Դﵽ�첽ִ�е�Ч��
void EventService::RunInThreadPool(std::function<void()>&& func)
{
    return _pMyEvMgr->RunInThreadPool(std::move(func));
}