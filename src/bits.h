#ifndef BITS_H
#define BITS_H

/*********************************/

#define USER_MODE_AWAY         0x01
#define USER_MODE_INVISIBLE    0x02
#define USER_MODE_GETWALLOPS   0x04
#define USER_MODE_GETSRVMSGS   0x08
#define USER_MODE_OPER         0x10

/*********************************/

#define CHAN_TYPE_NORMAL        '#'
#define CHAN_TYPE_SPECIAL       '!'
#define CHAN_TYPE_LOCAL         '&'
#define CHAN_TYPE_SAFE          '+'

#define CHAN_MODE_ANON           0x1u /* a - anonymous */
#define CHAN_MODE_PRIVATE        0x2u /* p - private */
#define CHAN_MODE_SECRET         0x4u /* s - secret */
#define CHAN_MODE_INVITE         0x8u /* i - invite-only */
#define CHAN_MODE_TLOCKED       0x10u /* t - topic locked (op only) */
#define CHAN_MODE_NOEXTERN      0x20u /* n - no external msgs */
#define CHAN_MODE_MODERATED     0x40u /* m - moderated */
#define CHAN_MODE_USERLIM       0x80u /* l - user limit */
#define CHAN_MODE_LOCKED       0x100u /* k - keylocked */
#define CHAN_MODE_QUIET        0x200u /* q - quiet */
#define CHAN_MODE_REOP         0x400u /* r - reop */

/*********************************/

#define MEMBER_FLAG_CREATOR    0x01 /* O - creator */
#define MEMBER_FLAG_OPERATOR   0x02 /* o - op */
#define MEMBER_FLAG_VOICE      0x04 /* v - voice */
#define MEMBER_FLAG_HALFOP     0x08 /* h - half-op */

/*********************************/

#define CHAN_MODELIST "biklmnopstveIrS"       /* FIXME: verify */
#define USER_MODELIST "oiwszcrkfydnxbauglZCD" /* FIXME: verify */

/*********************************/

#define MAX_ARGS 15
#define MAX_CMD  510
#define MAX_LINE  512
#define MAX_NICK 9
#define MAX_CHAN_NAME  50
#define MAX_CHAN_TOPIC 140
#define MAX_CHAN_KEY   63
#define MAX_PASSWD     127
#define MAX_SVC_DIST   127
#define MAX_SVC_INFO   510
#define MAX_USER_NAME  127
#define MAX_USER_HOST  63
#define MAX_USER_ADDR  15 /* FIXME: IPv4 only */
#define MAX_PATH       1023

#define MAX_HANDLE MAX_NICK+1 + MAX_USER_NAME+1 + MAX_USER_HOST

/*********************************/

#endif
