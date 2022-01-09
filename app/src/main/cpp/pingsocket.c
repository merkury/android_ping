//
// Created by David Obermann on 05.01.22.
//

#include "pingsocket.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <android/log.h>
#include <jni.h>


//from:  https://stackoverflow.com/a/37456455 which refers to:
// https://stackoverflow.com/a/20105379
//
//
//note, to allow root to use icmp sockets, run:
//sysctl -w net.ipv4.ping_group_range="0 0"
// on android this is done by: permission INTERNET in the manifest

int ping_it(const char *inaddr)
{
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    //alternative:
    //inet_aton(inaddr, &dst)
    //struct in_addr *dst
    //addr.sin_addr = *dst;

    if (inet_pton(AF_INET, inaddr, &(addr.sin_addr)) < 0)
    {
        __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "inet_pton errno %d %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    int sequence = 0;
    int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_ICMP);
    if (sock < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "socket errno %d %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234;//arbitrary id should be replaced by a random nr or uuid

    for (int i=0; i<10; i++) {
        unsigned char data[2048];
        int rc;
        struct timeval timeout = {3, 0}; //wait max 3 seconds for a reply
        fd_set read_set;
        socklen_t slen;
        struct icmphdr rcv_hdr;

        icmp_hdr.un.echo.sequence = sequence++;
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);

        const char* payload = "hello";
        memcpy(data + sizeof icmp_hdr, payload, strlen(payload)+1);

        rc = sendto(sock, data, sizeof icmp_hdr + strlen(payload)+1,
                    0, (struct sockaddr*)&addr, sizeof addr);
        if (rc <= 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "sendto errno %d %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "Sent ICMP");

        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);

        //wait for a reply with a timeout
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "Got no reply");
            continue;
        } else if (rc < 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "select errno %d %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }

        //we don't care about the sender address in this example..
        slen = 0;
        rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
        if (rc <= 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "revcfrom errno %d %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        } else if (rc < sizeof rcv_hdr) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "error got short ICMP packet, %d bytes\n", rc);
            return EXIT_FAILURE;
        }
        memcpy(&rcv_hdr, data, sizeof rcv_hdr);
        if (rcv_hdr.type == ICMP_ECHOREPLY) {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "ICMP Reply, id=0x%x, sequence =  0x%x\n",
                                icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "pingsocket", "Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
        }
    }
    close(sock);
    return 0;
}

jint Java_de_kreativsoft_hello_1cmake_MainActivity_pingJNI(JNIEnv *env, jobject thiz, jstring host)
{
    return ping_it((*env)->GetStringUTFChars(env, host, 0));
}

