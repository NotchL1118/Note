#include <iostream>
#include <string>
#include <string>
#include <stdlib.h>
#include <vector>
using namespace std;
#define PIECE_NUM 50 // �ڴ����
#define ORDER_NUM 10 // ָ���С
#define PAGE_NUM 5   // פ������С
#define BLOCK_TIME 3 // ����ʱ��
// �ݶ�ÿ������ ҳ��10��פ����5�飬5�����̣�Ҳ����Ҫ25��
// 0-19�ڴ��ָ�ϵͳ��20-50�ڴ��ָ��û�����,ÿ�������ķ�5��

// ָ��
typedef struct ORder
{
    char orderType; // ����i�����o����ѯq���޸�c
    int orderNum;   // Ҫִ�е�ҳ���
} ORDER;

// ҳ����
typedef struct PageTab
{
    int pageNum;  // ҳ��
    int blockNum; // ���
    int flag;     // 0:�����,1:���ڴ���,�Ƿ����ڴ���
    int access;   // ���һ�α����ʵ�ʱ�䣬LRU�㷨
} PAGETAB;

typedef struct Pcb
{
    int id;                  // ���̺�
    string name;             // ������
    int priority;            // ���ȼ�����ռʽ�����ȼ����ȣ����еĽ���ÿ��-3���������е�ÿ��+3
    int status;              // ����״̬ 0������ 1������ 2������
    int runTime;             // ����ʱ��
    int blockTime;           // ����ʱ��
    int curOrder;            // ָ�ָ��
    ORDER orders[ORDER_NUM]; // ָ�
    int startBlock;          // ��ʼ���
    vector<PAGETAB> pageTab; // ҳ��,���Ⱥͽ���ָ�����һ��
    int pageTabSize;         // ���ڴ��е�ҳ���С
} PCB;

int memory[PIECE_NUM];  // �ڴ�� -1������ 0��ϵͳ �������֣����̺�
vector<PCB> readyQueue; // ��������
vector<PCB> runProcess; // ���н���
vector<PCB> blockQueue; // ��������
Pcb P1, P2, P3, P4, P5; // ����
vector<string> REMIND;  // ��ʾ��Ϣ
int finishNum = 0;      // ����ɽ�����
int time = 0; // ʱ��Ƭ

void showTime()
{
    cout << "\033[35m------------------------------------------------------------------------------" << endl;
    cout << "\n==========================��ǰʱ��Ƭ  " << time << "\t===============================\n"
         << endl;
    cout << "-------------------------------------------------------------------------------\033[0m\n"
         << endl;
}
void showTime(string s)
{
    cout << "\033[35m\n\n------------------------------------------------------------------------------" << endl;
    cout << "\n==========================��ǰʱ��Ƭ:  " << time << "  " << s << "\t===============================\n"
         << endl;
    cout << "-------------------------------------------------------------------------------\033[0m\n"
         << endl;
}
// ��ʼ���ڴ�
void initMemory()
{
    int i = 0;
    for (; i <= 19; i++)
    {
        memory[i] = 0;
    }
    // 20-24 ������1��25-29������2��30-34������3��35-39������4��40-44������5
    int j = 0;
    for (; i < 45; i++)
    {
        if (i % 5 == 0)
            j++;
        memory[i] = j;
    }
    for (; i < PIECE_NUM; i++)
    {
        memory[i] = -1;
    }
}

// չʾ�ڴ�
void showMemory()
{
    cout << "\033[35m\n============�ڴ��============\033[0m\n"
         << endl;
    for (int i = 0; i < PIECE_NUM; i++)
    {
        printf("%3d", memory[i]);
        if (i % 10 == 9)
        {
            cout << endl;
        }
    }
    cout << endl;
}

// չʾ��������
void showReadyQueue()
{
    cout << "\033[32m\n==============================�������̱�====================================\033[0m\n"
         << endl;
    cout << "���̺�\t������\t���ȼ�\t״̬\t����ʱ��" << endl;
    for (int i = 0; i < readyQueue.size(); i++)
    {
        cout << readyQueue[i].id << "\t"
             << readyQueue[i].name << "\t"
             << readyQueue[i].priority << "\t";
        cout << "����\t";
        cout << readyQueue[i].runTime << endl;
    }
    cout << "\033[32m\n=================================================================================\033[0m\n";
}

// չʾ���н���
void showRunProcess()
{
    cout << "\033[37m\n==============================���н���====================================\033[0m\n"
         << endl;
    cout << "���̺�\t������\t���ȼ�\t״̬\t����ʱ��" << endl;
    for (int i = 0; i < runProcess.size(); i++)
    {
        cout << runProcess[i].id << "\t"
             << runProcess[i].name << "\t"
             << runProcess[i].priority << "\t";
        cout << "����\t";
        cout << runProcess[0].runTime << endl;
        cout << "\033[37m\n=================================================================================\033[0m\n";
        // չʾ���н��̵�ҳ��
        cout << "\033[37m\n==============================���н���ҳ��====================================\033[0m\n"
             << endl;
        cout << "ҳ��\t�Ƿ����ڴ�\t���\t�����ֶ�" << endl;
        for (int i = 0; i < runProcess[0].pageTab.size(); i++)
        {
            cout << runProcess[0].pageTab[i].pageNum << "\t";
            if (runProcess[0].pageTab[i].flag == 1)
                cout << "    ��\t\t";
            else
                cout << "    ��\t\t";
            if (runProcess[0].pageTab[i].blockNum != -1)
                cout << runProcess[0].pageTab[i].blockNum << "\t\t";
            else
                cout << "\t\t";
            if (runProcess[0].pageTab[i].access)
                cout << runProcess[0].pageTab[i].access << endl;
            else
                cout << "" << endl;
        }
        cout << "\033[37m\n=================================================================================\033[0m\n";
    }
}

// չʾ��������
void showBlockQueue()
{
    cout << "\033[31m\n==============================�������̱�====================================\033[0m\n"
         << endl;
    cout << "���̺�\t������\t���ȼ�\t״̬\t����ʱ��" << endl;
    for (int i = 0; i < blockQueue.size(); i++)
    {
        cout << blockQueue[i].id << "\t"
             << blockQueue[i].name << "\t"
             << blockQueue[i].priority << "\t";
        cout << "����\t";

        cout << blockQueue[i].blockTime << endl;
    }
    cout << "\033[31m\n=================================================================================\033[0m\n";
}

bool isShowStartTime = true;
void showTable()
{
    system("cls");
    if (time > 0 && isShowStartTime)
        showTime("��ʼ"), isShowStartTime = false;
    else
        showTime("����"), isShowStartTime = true;
    showReadyQueue();
    cout << endl;
    showRunProcess();
    cout << endl;
    showBlockQueue();
}

void showRemind()
{
    if (REMIND.empty())
        return;
    // չʾREMIND
    cout << "\033[35m\n==============================REMIND====================================\033[0m\n"
         << endl;
    for (auto c : REMIND)
    {
        cout << c << endl;
    }
    REMIND.clear();
    cout << "\033[35m\n=================================================================================\033[0m\n";
}

// ��ռʽ���ȼ�����
void prioritySchedule()
{
    // 1. �Ӿ����������ҵ����ȼ���ߵĽ���
    int maxPriority = -1;
    int maxPriorityIndex = 0;
    for (int i = 0; i < readyQueue.size(); i++)
    {
        if (readyQueue[i].priority > maxPriority)
        {
            maxPriority = readyQueue[i].priority;
            maxPriorityIndex = i;
        }
    }
    // 2. ���û�н��������л��߸ý��̵����ȼ������н��̵ĸߣ����ý��̴Ӿ����������Ƴ����������ж�����
    if (runProcess.size() == 0)
    {
        // 2.1 �����������еĽ��̷������ж����У�����һ��״̬
        runProcess.push_back(readyQueue[maxPriorityIndex]);
        runProcess[0].status = 1;
        readyQueue.erase(readyQueue.begin() + maxPriorityIndex);
    }
    else if (maxPriority > runProcess[0].priority)
    {
        // 2.1 �����ж����еĽ��̷�����������У�����һ��״̬
        runProcess[0].status = 0;
        readyQueue.push_back(runProcess[0]);
        runProcess.clear();
        // 2.2 �����������еĽ��̷������ж����У�����һ��״̬
        runProcess.push_back(readyQueue[maxPriorityIndex]);
        runProcess[0].status = 1;
        readyQueue.erase(readyQueue.begin() + maxPriorityIndex);
    }
}

// ��ȡ���н��̵�ָ��
void readOrder()
{
    int curOrder = runProcess[0].curOrder;
    int pageNum = runProcess[0].orders[curOrder].orderNum;
    int startBlock = runProcess[0].startBlock;
    char orderType = runProcess[0].orders[curOrder].orderType;
    runProcess[0].curOrder++;
    // ���ڴ��е�ҳ��access��1
    for (int i = 0; i < runProcess[0].pageTab.size(); i++)
    {
        // ������ڴ���
        if (runProcess[0].pageTab[i].flag == 1)
            runProcess[0].pageTab[i].access++;
    }
    bool isShouldClear = false;
    if (orderType == 'i' || orderType == 'o')
    {
        // ������������ָ���ʱ����Ҫ�����̷�������������
        runProcess[0].status = 2;
        runProcess[0].blockTime = BLOCK_TIME; // ����ʱ��
        blockQueue.push_back(runProcess[0]);
        // runProcess.clear();
        isShouldClear = true;
        string s = "����" + runProcess[0].name + "ִ����" + orderType + "ָ�������������";
        REMIND.push_back(s);
    }
    // ��ѯ�޸�
    // ��ҳ���ڴ��У�ֱ�Ӳ���
    if (runProcess[0].pageTab[pageNum - 1].flag == 1)
    {
        // ����������ʡ��ûд��ֱ�ӵ�ִ�����֮�����
        runProcess[0].pageTab[pageNum - 1].access = 1;
        if (runProcess[0].curOrder == ORDER_NUM)
        {
            // ����ִ�������һ�����ִ�����
            finishNum++;
            runProcess.clear();
            string s = "����" + runProcess[0].name + "ִ�����";
            REMIND.push_back(s);
        }
        return;
    }
    // ������˵����ҳ�����ڴ���
    // 1. ����ڴ����п��п飬ֱ�ӷ���
    if (runProcess[0].pageTabSize < PAGE_NUM)
    {
        // ҳ���л��п�λ�����ڴ��з���һ����
        runProcess[0].pageTab[pageNum - 1].blockNum = startBlock + runProcess[0].pageTabSize;
        runProcess[0].pageTab[pageNum - 1].flag = 1;
        runProcess[0].pageTab[pageNum - 1].access = 1;
        runProcess[0].pageTabSize++;
    }
    else
    {
        // 2. ����ڴ���û�п��п飬��Ҫ����ҳ���û�
        // 2.1 �ҵ�������δʹ�õ�ҳ
        int maxAccess = -0x3f3f3f;
        int maxAccessIndex = -1;
        for (int i = 0; i < runProcess[0].pageTab.size(); i++)
        {
            if (runProcess[0].pageTab[i].access > maxAccess)
            {
                maxAccess = runProcess[0].pageTab[i].access;
                maxAccessIndex = i;
            }
        }
        // 2.2 ���ﲻд�ش��̣�ֱ�ӽ����滻
        int t = runProcess[0].pageTab[maxAccessIndex].pageNum;
        runProcess[0].pageTab[pageNum - 1].access = 1;
        runProcess[0].pageTab[pageNum - 1].blockNum = runProcess[0].pageTab[maxAccessIndex].blockNum;
        runProcess[0].pageTab[pageNum - 1].flag = 1;
        runProcess[0].pageTab[maxAccessIndex].access = 0;
        runProcess[0].pageTab[maxAccessIndex].blockNum = -1;
        runProcess[0].pageTab[maxAccessIndex].flag = 0;
        string s = "����" + runProcess[0].name + "ִ����" + orderType + "ָ�����ҳ��ȱҳ������LRUҳ���û�";
        s += "��ҳ��" + to_string(t) + "���ڿ黻��ҳ��" + to_string(pageNum);
        REMIND.push_back(s);
    }
    if (isShouldClear)
        runProcess.clear();
    if (runProcess[0].curOrder == ORDER_NUM)
    {
        // ����ִ�����
        finishNum++;
        runProcess.clear();
        string s = "����" + runProcess[0].name + "ִ�����";
        REMIND.push_back(s);
    }
}

void check()
{
    for (int i = 0; i < blockQueue.size(); i++)
    {
        blockQueue[i].blockTime--;
        if (blockQueue[i].blockTime == 0)
        {
            // �������������Ƴ�
            blockQueue[i].status = 0;
            readyQueue.push_back(blockQueue[i]);
            blockQueue.erase(blockQueue.begin() + i);
            i--;
        }
    }
}

// ����ʱ��Ƭ
void run()
{
    while (true)
    {
        if (finishNum == 5)
        {
            printf("���н���ִ�����\n");
            system("pause");
            exit(0);
        }
        time++;
        // ������ռʽ���ȼ�����ѡ��һ������
        prioritySchedule();
        showTable();
        system("pause");
        // 1. ���н��̵����ȶ�-3,�������е����ȶ�+3
        runProcess[0].priority = runProcess[0].priority - 3 ? runProcess[0].priority - 3 : 0;
        runProcess[0].runTime++;
        for (int i = 0; i < readyQueue.size(); i++)
        {
            readyQueue[i].priority += 3;
        }
        // 2. ��ȡָ��
        readOrder();
        check();
        showTable();
        showRemind();
        system("pause");
    }
}

void initProcess()
{
    // 20-24 ������1��25-29������2��30-34������3��35-39������4��40-44������5
    // P1������ʾȱҳ�жϺ�ҳ���û���P2������ʾ�����л�
    P1.id = 0, P1.name = "P1", P1.priority = 80, P1.status = 0, P1.startBlock = 20, P1.curOrder = 0, P1.blockTime = 0;
    P2.id = 1, P2.name = "P2", P2.priority = 30, P2.status = 0, P2.startBlock = 25, P2.curOrder = 0, P2.blockTime = 0;
    P3.id = 2, P3.name = "P3", P3.priority = 10, P3.status = 0, P3.startBlock = 30, P3.curOrder = 0, P3.blockTime = 0;
    P4.id = 3, P4.name = "P4", P4.priority = 20, P4.status = 0, P4.startBlock = 35, P4.curOrder = 0, P4.blockTime = 0;
    P5.id = 4, P5.name = "P5", P5.priority = 15, P5.status = 0, P5.startBlock = 40, P5.curOrder = 0, P5.blockTime = 0;
    P1.orders[0].orderType = 'q', P1.orders[0].orderNum = 1;  // ��ѯҳ��1
    P1.orders[1].orderType = 'c', P1.orders[1].orderNum = 2;  // �޸�ҳ��2
    P1.orders[2].orderType = 'q', P1.orders[2].orderNum = 3;  // ��ѯҳ��3
    P1.orders[3].orderType = 'c', P1.orders[3].orderNum = 4;  // �޸�ҳ��4
    P1.orders[4].orderType = 'q', P1.orders[4].orderNum = 5;  // ��ѯҳ��5
    P1.orders[5].orderType = 'c', P1.orders[5].orderNum = 1;  // �޸�ҳ��1
    P1.orders[6].orderType = 'q', P1.orders[6].orderNum = 2;  // ��ѯҳ��2
    P1.orders[7].orderType = 'c', P1.orders[7].orderNum = 8;  // �޸�ҳ��8������ᷢ��ҳ���û�
    P1.orders[8].orderType = 'q', P1.orders[8].orderNum = 9;  // ��ѯҳ��9
    P1.orders[9].orderType = 'c', P1.orders[9].orderNum = 10; // �޸�ҳ��10
    // ��ʼ��ҳ��
    for (int i = 0; i < ORDER_NUM; i++)
    {
        PAGETAB p;
        p.pageNum = i + 1;
        p.blockNum = -1;
        p.flag = 0;
        p.access = 0;
        P1.pageTab.push_back(p);
    }
    cout << "DEBUG2" << endl;
    P2.orders[0].orderType = 'q', P2.orders[0].orderNum = 1; // ��ѯҳ��1
    P2.orders[1].orderType = 'i', P2.orders[1].orderNum = 2; // ���룬���﷢���жϣ�������������
    P2.orders[2].orderType = 'q', P2.orders[2].orderNum = 2; // ��ѯҳ��2
    P2.orders[3].orderType = 'o', P2.orders[3].orderNum = 4; // ���ҳ��4
    P2.orders[4].orderType = 'q', P2.orders[4].orderNum = 3; // ��ѯҳ��3
    P2.orders[5].orderType = 'c', P2.orders[5].orderNum = 4; // �޸�ҳ��4
    P2.orders[6].orderType = 'i', P2.orders[6].orderNum = 7; // ���룬�ж�
    P2.orders[7].orderType = 'c', P2.orders[7].orderNum = 5; // �޸�ҳ��5
    P2.orders[8].orderType = 'q', P2.orders[8].orderNum = 1; // ��ѯҳ��1
    P2.orders[9].orderType = 'c', P2.orders[9].orderNum = 2; // �޸�ҳ��2
    for (int i = 0; i < ORDER_NUM; i++)
    {
        PAGETAB p;
        p.pageNum = i + 1;
        p.blockNum = -1;
        p.flag = 0;
        p.access = 0;
        P2.pageTab.push_back(p);
    }
    P3.orders[0].orderType = 'q', P3.orders[0].orderNum = 1;
    P3.orders[1].orderType = 'c', P3.orders[1].orderNum = 2;
    P3.orders[2].orderType = 'q', P3.orders[2].orderNum = 3;
    P3.orders[3].orderType = 'c', P3.orders[3].orderNum = 4;
    P3.orders[4].orderType = 'q', P3.orders[4].orderNum = 5;
    P3.orders[5].orderType = 'c', P3.orders[5].orderNum = 1;
    P3.orders[6].orderType = 'q', P3.orders[6].orderNum = 2;
    P3.orders[7].orderType = 'c', P3.orders[7].orderNum = 6;
    P3.orders[8].orderType = 'q', P3.orders[8].orderNum = 7;
    P3.orders[9].orderType = 'c', P3.orders[9].orderNum = 3;
    for (int i = 0; i < ORDER_NUM; i++)
    {
        PAGETAB p;
        p.pageNum = i + 1;
        p.blockNum = -1;
        p.flag = 0;
        p.access = 0;
        P3.pageTab.push_back(p);
    }
    P4.orders[0].orderType = 'q', P4.orders[0].orderNum = 1;
    P4.orders[1].orderType = 'c', P4.orders[1].orderNum = 2;
    P4.orders[2].orderType = 'q', P4.orders[2].orderNum = 3;
    P4.orders[3].orderType = 'c', P4.orders[3].orderNum = 4;
    P4.orders[4].orderType = 'q', P4.orders[4].orderNum = 5;
    P4.orders[5].orderType = 'q', P4.orders[5].orderNum = 1;
    P4.orders[6].orderType = 'q', P4.orders[6].orderNum = 2;
    P4.orders[7].orderType = 'c', P4.orders[7].orderNum = 3;
    P4.orders[8].orderType = 'q', P4.orders[8].orderNum = 4;
    P4.orders[9].orderType = 'c', P4.orders[9].orderNum = 5;
    for (int i = 0; i < ORDER_NUM; i++)
    {
        PAGETAB p;
        p.pageNum = i + 1;
        p.blockNum = -1;
        p.flag = 0;
        p.access = 0;
        P4.pageTab.push_back(p);
    }
    P5.orders[0].orderType = 'q', P5.orders[0].orderNum = 1;
    P5.orders[1].orderType = 'c', P5.orders[1].orderNum = 2;
    P5.orders[2].orderType = 'q', P5.orders[2].orderNum = 3;
    P5.orders[3].orderType = 'c', P5.orders[3].orderNum = 4;
    P5.orders[4].orderType = 'q', P5.orders[4].orderNum = 5;
    P5.orders[5].orderType = 'q', P5.orders[5].orderNum = 1;
    P5.orders[6].orderType = 'q', P5.orders[6].orderNum = 2;
    P5.orders[7].orderType = 'c', P5.orders[7].orderNum = 3;
    P5.orders[8].orderType = 'q', P5.orders[8].orderNum = 4;
    P5.orders[9].orderType = 'c', P5.orders[9].orderNum = 5;
    for (int i = 0; i < ORDER_NUM; i++)
    {
        PAGETAB p;
        p.pageNum = i + 1;
        p.blockNum = -1;
        p.flag = 0;
        p.access = 0;
        P5.pageTab.push_back(p);
    }
}

// ��ʼ��
void init()
{
    // 1.��ʼ���ڴ�
    initMemory();
    // 2.��ʼ������
    initProcess();
    // 3.��������̶��Ž���������
    readyQueue.push_back(P1);
    readyQueue.push_back(P2);
    readyQueue.push_back(P3);
    readyQueue.push_back(P4);
    readyQueue.push_back(P5);
}
int main()
{
    bool isInit = false;
    while (true)
    {
        cout << "��ѡ���ʼ����������" << endl;
        cout << "1.��ʼ��(����һ��)" << endl;
        cout << "2.����" << endl;
        int choose;
        cin >> choose;
        switch (choose)
        {
        case 1:
            if (!isInit)
            {
                isInit = true;
                init();
                showTable();
                showMemory();
            }
            break;
        case 2:
            run();
            break;
        }
    }
    return 0;
}