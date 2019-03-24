#include"messaging.hpp"
#include<cstring>


WorkerThread:: WorkerThread()
{

}

 WorkerThread:: ~WorkerThread()
 {
 
 }

WorkerThread:: WorkerThread(const char* threadName)
{
   //const char* THREAD_NAME;
   int l = strlen(threadName );
    THREAD_NAME= new char[l];
    strcpy(THREAD_NAME,threadName); //why error putting  THREAD_NAME As constant


}

WorkerThread:: WorkerThread(const WorkerThread&)
{

}
WorkerThread&  WorkerThread:: operator=(const WorkerThread&)
{


}


bool WorkerThread::CreateThread()
{
    if (!m_thread)
        m_thread = new thread(&WorkerThread::Process, this);
    return true;
}
void WorkerThread::ExitThread()
{

}

void WorkerThread::PostMsg(UserData const*)
{

}
void WorkerThread::Process()
{

    m_timerExit = false;
    std::thread timerThread(&WorkerThread::TimerThread, this);

    while (1)
    {
        ThreadMsg* msg = 0;
        {
            // Wait for a message to be added to the queue
            std::unique_lock<std::mutex> lk(m_mutex);
            while (m_queue.empty())
                m_cv.wait(lk);

            if (m_queue.empty())
                continue;

            msg = m_queue.front();
            m_queue.pop();
        }

        switch (msg->id)
        {
            case MSG_POST_USER_DATA:
            {
                ASSERT_TRUE(msg->msg != NULL);

                // Convert the ThreadMsg void* data back to a UserData*
                const UserData* userData = static_cast<const UserData*>(msg->msg);

                cout << userData->msg.c_str() << " " << userData->year << " on " << THREAD_NAME << endl;

                // Delete dynamic data passed through message queue
                delete userData;
                delete msg;
                break;
            }

            case MSG_TIMER:
                cout << "Timer expired on " << THREAD_NAME << endl;
                delete msg;
                break;

            case MSG_EXIT_THREAD:
            {
                m_timerExit = true;
                timerThread.join();

                delete msg;
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_queue.empty())
                {
                    msg = m_queue.front();
                    m_queue.pop();
                    delete msg;
                }

                cout << "Exit thread on " << THREAD_NAME << endl;
                return;
            }

            default:
                ASSERT();
        }
    }
}

