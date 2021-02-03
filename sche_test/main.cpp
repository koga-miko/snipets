#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/times.h>
#include <vector>
using namespace std;

// timespec差分算出関数
inline void sub_timespec(struct timespec* res, const struct timespec* a, const struct timespec* b)
{
    if (a->tv_nsec >= b->tv_nsec) {
        res->tv_nsec = a->tv_nsec - b->tv_nsec;
        res->tv_sec = a->tv_sec - b->tv_sec;
    } else {
        res->tv_nsec = 1000000000 + a->tv_nsec - b->tv_nsec;
        res->tv_sec = a->tv_sec - b->tv_sec - 1;
    }
}

class PriorityThread
{
public:
    PriorityThread(const char* name, int policy, int priority) {
        this->name = name;
        this->policy = policy;
        this->priority = priority;
    }
    bool create() {
        int ret;
        pthread_attr_t attr;
        // スレッド属性オブジェクトの初期化
        ret = pthread_attr_init(&attr);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::create() : pthread_attr_init() ret=%d\n", ret);
            return false;
        }
        // スレッド属性スケジューリング継承：継承しない
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::create() : pthread_attr_setinheritsched() ret=%d\n", ret);
            return false;
        }
        // スレッド属性スケジューリングポリシーの設定
        ret = pthread_attr_setschedpolicy(&attr, this->policy);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::create() : pthread_attr_setschedpolicy(policy=%d) ret=%d\n", this->policy, ret);
            return false;
        }
        // スレッド属性スケジューリングパラメータの設定
        struct sched_param param;
        param.sched_priority = this->priority;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::create() : pthread_attr_setschedparam(priority=%d) ret=%d\n", this->priority, ret);
            return false;
        }
        // スレッド作成
        ret = pthread_create(&this->tid, &attr, PriorityThread::_run, (void*)this);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::create() : pthread_create() ret=%d\n", ret);
            return false;
        }
        // スレッド属性オブジェクトの破棄
        pthread_attr_destroy(&attr);

        return true;
    }
    bool join() {
        int ret;
        ret = pthread_join(this->tid, NULL);
        if (ret != 0) {
            fprintf(stderr, "PriorityThread::join() : pthread_join() ret=%d\n", ret);
            return false;
        }
        return true;
    }
    // 測定用の高負荷処理(面倒だったのでfor分繰り返しで)
    virtual void* run() {
        int64_t ans = 0;
        for (int64_t i = 0; i <= 1000000000; i++) ans += i;
    }
    // スレッド実行開始の制御のためmutexを使用
    static bool lockRun() {
        pthread_mutex_lock(&PriorityThread::g_thread_block);
    }
    static bool unlockRun() {
        pthread_mutex_unlock(&PriorityThread::g_thread_block);
    }
    const char* getName() {
        return name;
    }
    void printlog() {
        struct timespec elapsed;
        printf("  %12s", name);
        sub_timespec(&elapsed, &etime_moni, &stime_moni);
        printf(": time(%ld.%09ld, %ld.%09ld, %ld.%09ld)", stime_moni.tv_sec, stime_moni.tv_nsec,
            etime_moni.tv_sec, etime_moni.tv_nsec,elapsed.tv_sec, elapsed.tv_nsec);
        sub_timespec(&elapsed, &etime_tcpu, &stime_tcpu);
        printf(": cpu(%ld.%09ld, %ld.%09ld, %ld.%09ld)", stime_tcpu.tv_sec, stime_tcpu.tv_nsec,
            etime_tcpu.tv_sec, etime_tcpu.tv_nsec,elapsed.tv_sec, elapsed.tv_nsec);
        printf("\n");
    }
private:
    static void* _run(void* param) {
        PriorityThread* obj = ((PriorityThread*)param);
        // ロック解除待ち
        pthread_mutex_lock(&PriorityThread::g_thread_block);
        pthread_mutex_unlock(&PriorityThread::g_thread_block);
        // 処理開始時間測定
        clock_gettime(CLOCK_MONOTONIC, &obj->stime_moni);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &obj->stime_tcpu);
        // 処理実行
        void* ret = ((PriorityThread*)param)->run();
        // 処理終了時間測定
        clock_gettime(CLOCK_MONOTONIC, &obj->etime_moni);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &obj->etime_tcpu);

        return ret;
    }
private:
    static pthread_mutex_t g_thread_block;
    const char* name;
    int policy;
    int priority;
    pthread_t tid;
    struct timespec stime_moni;
    struct timespec stime_tcpu;
    struct timespec etime_moni;
    struct timespec etime_tcpu;
};
pthread_mutex_t PriorityThread::g_thread_block = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
    bool result;
    vector<PriorityThread*>  threads;

    // スレッド定義
    threads.push_back(new PriorityThread("Thread_0", SCHED_RR , 50));
    threads.push_back(new PriorityThread("Thread_1", SCHED_RR , 50));
    threads.push_back(new PriorityThread("Thread_2", SCHED_RR , 50));
    threads.push_back(new PriorityThread("Thread_3", SCHED_RR , 5));
    threads.push_back(new PriorityThread("Thread_4", SCHED_RR , 5));
    threads.push_back(new PriorityThread("Thread_5", SCHED_RR , 5));
    threads.push_back(new PriorityThread("Thread_6", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_7", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_8", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_9", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_A", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_B", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_C", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_D", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_E", SCHED_RR , 10));
    threads.push_back(new PriorityThread("Thread_F", SCHED_RR , 10));

    // スレッド実行開始をロック
    PriorityThread::lockRun();

    // スレッドの作成
    printf("Thread Create.\n");
    for (auto item : threads) {
        result = item->create();
        if (!result) {
            fprintf(stderr, "Create Error: name=%s\n", item->getName());
            exit(1);
        }
    }
    printf("Thread Create done.\n");
    // スレッド実行開始のロック解除
    sleep(1);
    printf("Thread Run.\n");
    PriorityThread::unlockRun();

    // スレッドの終了待ち
    for (auto item : threads) {
        result = item->join();
        if (!result) {
            fprintf(stderr, "Thread Join Error: name=%s\n", item->getName());
            exit(1);
        }
    }
    printf("Thread Join done.\n");

    // スレッド実行時間のログ出力
    for (auto item : threads) {
        item->printlog();
    }

    // スレッド情報の破棄
    for (auto item : threads) {
        delete item;
    }
    threads.clear();
}
