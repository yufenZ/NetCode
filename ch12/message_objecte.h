//
//  message_objecte.h
//  pingclient
//
//  Created by yufenZ on 2020/5/12.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#ifndef message_objecte_h
#define message_objecte_h

#include <sys/types.h>
typedef struct {
    u_int32_t type;
    char data[1024];
} messageObject;

#define MSG_PING   1
#define MSG_PONG   2
#define MSG_TYPE1 11
#define MSG_TYPE2 21

#endif /* message_objecte_h */
