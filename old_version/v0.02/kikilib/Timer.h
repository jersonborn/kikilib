//@Author Liu Yukang 
#pragma once
#include "Time.h"
#include "Socket.h"
#include "utils.h"

#include <map>
#include <vector>
#include <mutex>
#include <functional>

namespace kikilib
{
	//��ʱ��
	//ְ��
	//1������ ĳһʱ�̡�����ʱ����Ҫִ�еĺ��� ��ӳ���
	//2���ṩ��ʱ�¼��Ĳ����ӿ�
	//3�����಻��֤�̰߳�ȫ���̰߳�ȫ��EventManager����
	class Timer
	{
	public:
		Timer(Socket& timeSock) : _timeSock(timeSock) {}
		Timer(Socket&& timeSock) : _timeSock(timeSock) {}
		~Timer() {}

		DISALLOW_COPY_MOVE_AND_ASSIGN(Timer);

		//��ȡ�����Ѿ���ʱ����Ҫִ�еĺ���
		void GetExpiredTask(std::vector<std::function<void()>> &tasks);

		//��timeʱ����Ҫִ�к���cb
		void RunAt(Time time, std::function<void()>& cb);
		void RunAt(Time time, std::function<void()>&& cb);

	private:
		//��timefd��������ʱ�䣬time�Ǿ���ʱ��
		void ResetTimeOfTimefd(Time time);

		Socket _timeSock;

		//��ʱ���ص���������
		std::multimap<Time, std::function<void()>> _timerCbMap;
	};

}