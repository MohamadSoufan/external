#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <cmath>

// default max message size, can be found in /proc/sys/fs/mqueue/msgsize_max
const std::size_t bufsize = 8192;

// 
const char * q0 = "/70";
const char * q1 = "/71"; 
const char * q2 = "/72"; 
const char * q3 = "/73"; 
const char * q4 = "/74"; 
const char * q5 = "/75"; 
const char * q6 = "/76";

int main(int argc, char *argv[]) {
    
    struct Node {
        double utemp;
        double dtemp;
        double ctemp_a;
        double ctemp_b;
        double ptemp;
        int pid;
    } node;

    double kill = 0xdeadbeefdeadbeef;
    double nokill = 0;
    double yeskill = 0;
    double excl = 0;

    // Clear queue if no args
    if (argc == 1)
    {
        mq_unlink(q0);
        mq_unlink(q1);
        mq_unlink(q2);
        mq_unlink(q3);
        mq_unlink(q4);
        mq_unlink(q5);
        mq_unlink(q6);
        std::cout << "Queues unlinked\n";
        return 0;
    }
    // Set the pid and initial temp
    else if (argc == 3)
    {
        std::string arg;
        arg = argv[1];
        if (arg.find_first_not_of("0123456789.") == std::string::npos)
            node.pid = atoi(argv[1]);  // Set pid
        else
        {
            std::cerr << "Usage: ./external <floating point temperature> <numeric pid>" << std::endl;
            return 1;
        }
        
        arg = argv[2];
        if (arg.find_first_not_of("0123456789.") == std::string::npos)
        {
            node.dtemp = atof(argv[2]);  // Set initial temp
            node.utemp = atof(argv[2]);
        }
        else
        {
            std::cerr << "Usage: ./external <floating point temperature> <numeric pid>" << std::endl;
            return 1;
        }

        if (node.pid > 6 || node.pid < 0)
        {
            std::cerr << "PID must be between 0-6" << std::endl;
            return 1;
        }
    }
    // Throw error and quit if invalid number of args
    else
    {
        std::cerr << "Usage: ./external <floating point temperature> <numeric pid>" << std::endl;
        return 1;
    }

    mqd_t mq, mqp, mqa, mqb;

    errno = 0;

    // Open own and child nodes mailboxes
    if (node.pid == 0)
        mq = mq_open(q0, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else if (node.pid == 1)
        mq = mq_open(q1, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else if (node.pid == 2)
        mq = mq_open(q2, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else if (node.pid == 3)
        mq = mq_open(q3, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else if (node.pid == 4)
        mq = mq_open(q4, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else if (node.pid == 5)
        mq = mq_open(q5, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    else
        mq = mq_open(q6, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    
    // Wait for all the mailboxes to be opened
    sleep(10);

    bool killall = false;
    if (errno == EEXIST)
    {
        std::cerr << "All PID must be unique, between 0-6" << std::endl;
        killall = true;
    }

    // Open child nodes mailboxes
    if (node.pid == 0)
    {
        mqa = mq_open(q1, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
        mqb = mq_open(q2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    }
    else if (node.pid == 1)
    {
        mqa = mq_open(q3, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
        mqb = mq_open(q4, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    }
    else if (node.pid == 2)
    {
        mqa = mq_open(q5, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
        mqb = mq_open(q6, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    }

    // Open parent nodes mailboxes
    if (node.pid == 1 || node.pid == 2)
        mqp = mq_open(q0, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    if (node.pid == 3 || node.pid == 4)
        mqp = mq_open(q1, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);
    if (node.pid == 5 || node.pid == 6)
        mqp = mq_open(q2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);

    if (killall)
    {
        mq_unlink(q0);
        mq_unlink(q1);
        mq_unlink(q2);
        mq_unlink(q3);
        mq_unlink(q4);
        mq_unlink(q5);
        mq_unlink(q6);
        system("pkill external");
        return 1;
    }
    // Run forever until the top temp becomes stable
    while (1)
    {
        // Logic for node 0
        if (node.pid == 0)
        {
            // Send downtemp to children
            mq_send(mqa, (const char *) &node.dtemp, 0x8, 0);
            mq_send(mqb, (const char *) &node.dtemp, 0x8, 0);

            // Get temp from children
            mq_receive(mq, (char *) &node.ctemp_a, 0x8000, 0);
            mq_receive(mq, (char *) &node.ctemp_b, 0x8000, 0);

            // Computer new uptemp
            node.utemp = (node.dtemp + node.ctemp_a + node.ctemp_b)/3.0;

            // Display temp
            printf("Process #%d: current temperature %f\n", node.pid, node.utemp);

            // Check if temps are close enough to quit program
            if (((node.utemp - node.dtemp)*(node.utemp - node.dtemp)) < 0.0001)
            {
                printf("Process #%d: final temperature %f\n", node.pid, node.utemp);
                mq_send(mqa, (const char *) &kill, 0x8, 0);
                mq_send(mqb, (const char *) &kill, 0x8, 0);
                mq_unlink(q0);
                return 0;
            }
            // Not done yet so set up temp to down temp
            else
                node.dtemp = node.utemp;
        }

        // Logic for nodes 1 and 2
        if (node.pid == 1 || node.pid == 2)
        {
            // Get parent temp
            mq_receive(mq, (char *) &node.ptemp, 0x8000, 0);

            // Look for unique kill command
            if (node.ptemp == 0xdeadbeefdeadbeef)
            {
                printf("Process #%d: final temperature %f\n", node.pid, node.utemp);
                mq_send(mqa, (const char *) &kill, 0x8, 0);
                mq_send(mqb, (const char *) &kill, 0x8, 0);
                
                if (node.pid == 1)
                    mq_unlink(q1);
                else
                    mq_unlink(q2);
                
                mq_close(mq);
                return 0;
            }

            // Find new dtemp
            node.dtemp = (node.utemp + node.ptemp)/2.0;

            // Send new downtemp
            mq_send(mqa, (const char *) &node.dtemp, 0x8, 0);
            mq_send(mqb, (const char *) &node.dtemp, 0x8, 0);
            
            // Get temps from children
            mq_receive(mq, (char *) &node.ctemp_a, 0x8000, 0);
            mq_receive(mq, (char *) &node.ctemp_b, 0x8000, 0);

            // Find new utemp
            node.utemp = (node.dtemp + node.ctemp_a + node.ctemp_b)/3.0;

            // Display temp
            printf("Process #%d: current temperature %f\n", node.pid, node.dtemp);

            // Send uptemp to parent
            mq_send(mqp, (const char *) &node.utemp, 0x8, 0);
        }

        // Logic for nodes 3, 4, 5, 6
        if (node.pid > 2)
        {
            // Get parent temp
            mq_receive(mq, (char *) &node.ptemp, 0x8000, 0);

            // Look for unique kill command
            if (node.ptemp == 0xdeadbeefdeadbeef)
            {
                printf("Process #%d: final temperature %f\n", node.pid, node.dtemp);
                if (node.pid == 3)
                    mq_unlink(q3);
                if (node.pid == 4)
                    mq_unlink(q4);
                if (node.pid == 5)
                    mq_unlink(q5);
                if (node.pid == 6)
                    mq_unlink(q6);
                mq_close(mq);
                return 0;
            }

            // Find new temp of node
            node.utemp = (node.dtemp + node.ptemp)/2.0;
            node.dtemp = node.utemp;

            // Send temp to parent
            mq_send(mqp, (const char *) &node.utemp, 0x8, 0);

            // Display temp
            printf("Process #%d: current temperature %f\n", node.pid, node.dtemp);
        }
    }
    return 0;
}