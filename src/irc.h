#ifndef IRC_H
#define IRC_H

#include <stdint.h>

/*********************************/

#define RPL_WELCOME              1
#define RPL_YOURHOST             2
#define RPL_CREATED              3
#define RPL_MYINFO               4
#define RPL_BOUNCE               5

/*********************************/

#define RPL_TRACELINK          200
#define RPL_TRACECONNECTING    201
#define RPL_TRACEHANDSHAKE     202
#define RPL_TRACEUNKNOWN       203
#define RPL_TRACEOPERATOR      204
#define RPL_TRACEUSER          205
#define RPL_TRACESERVER        206
#define RPL_TRACESERVICE       207
#define RPL_TRACENEWTYPE       208
#define RPL_TRACECLASS         209
#define RPL_TRACERECONNECT     210
#define RPL_STATSLINKINFO      211
#define RPL_STATSCOMMANDS      212
#define RPL_STATSCLINE         213 /* reserved */
#define RPL_STATSNLINE         214 /* reserved */
#define RPL_STATSILINE         215 /* reserved */
#define RPL_STATSKLINE         216 /* reserved */
#define RPL_STATSQLINE         217 /* reserved */
#define RPL_STATSYLINE         218 /* reserved */
#define RPL_ENDOFSTATS         219

#define RPL_UMODEIS            221

#define RPL_SERVICEINFO        231 /* retired */
#define RPL_ENDOFSERVICES      232 /* retired */
#define RPL_SERVICE            233 /* retired */
#define RPL_SERVLIST           234
#define RPL_SERVLISTEND        235

#define RPL_STATSVLINE         240 /* reserved */
#define RPL_STATSLLINE         241 /* reserved */
#define RPL_STATSUPTIME        242
#define RPL_STATSOLINE         243
#define RPL_STATSHLINE         244 /* reserved */
#define RPL_STATSSLINE         245 /* reserved */
#define RPL_STATSPING          246 /* reserved */
#define RPL_STATSBLINE         247 /* reserved */

#define RPL_STATSDLINE         250 /* reserved */
#define RPL_LUSERCLIENT        251
#define RPL_LUSEROP            252
#define RPL_LUSERUNKNOWN       253
#define RPL_LUSERCHANNELS      254
#define RPL_LUSERME            255
#define RPL_ADMINME            256
#define RPL_ADMINLOC1          257
#define RPL_ADMINLOC2          258
#define RPL_ADMINEMAIL         259

#define RPL_TRACELOG           261
#define RPL_TRACEEND           262
#define RPL_TRYAGAIN           263

/*********************************/

#define RPL_NONE               300 /* retired */
#define RPL_AWAY               301
#define RPL_USERHOST           302
#define RPL_ISON               303

#define RPL_UNAWAY             305
#define RPL_NOWAWAY            306

#define RPL_WHOISUSER          311
#define RPL_WHOISSERVER        312
#define RPL_WHOISOPERATOR      313
#define RPL_WHOWASUSER         314
#define RPL_ENDOFWHO           315
#define RPL_WHOISCHANOP        316 /* retired */
#define RPL_WHOISIDLE          317
#define RPL_ENDOFWHOIS         318
#define RPL_WHOISCHANNELS      319

#define RPL_LISTSTART          321
#define RPL_LIST               322
#define RPL_LISTEND            323
#define RPL_CHANNELMODEIS      324
#define RPL_UNIQOPIS           325

#define RPL_NOTOPIC            331
#define RPL_TOPIC              332

#define RPL_INVITING           341
#define RPL_SUMMONING          342

#define RPL_INVITELIST         346
#define RPL_ENDOFINVITELIST    347
#define RPL_EXCEPTLIST         348
#define RPL_ENDOFEXCEPTLIST    349

#define RPL_VERSION            351
#define RPL_WHOREPLY           352
#define RPL_NAMREPLY           353

#define RPL_KILLDONE           361 /* retired */
#define RPL_CLOSING            362 /* retired */
#define RPL_CLOSEEND           363 /* retired */
#define RPL_LINKS              364
#define RPL_ENDOFLINKS         365
#define RPL_ENDOFNAMES         366
#define RPL_BANLIST            367
#define RPL_ENDOFBANLIST       368

#define RPL_ENDOFWHOWAS        369

#define RPL_INFO               371
#define RPL_MOTD               372
#define RPL_INFOSTART          373 /* retired */
#define RPL_ENDOFINFO          374
#define RPL_MOTDSTART          375
#define RPL_ENDOFMOTD          376

#define RPL_YOUREOPER          381
#define RPL_REHASHING          382
#define RPL_YOURESERVICE       383
#define RPL_MYPORTIS           384 /* retired */

#define RPL_TIME               391
#define RPL_USERSSTART         392
#define RPL_USERS              393
#define RPL_ENDOFUSERS         394
#define RPL_NOUSERS            395

/*********************************/

#define ERR_NOSUCHNICK         401
#define ERR_NOSUCHSERVER       402
#define ERR_NOSUCHCHANNEL      403
#define ERR_CANNOTSENDTOCHAN   404
#define ERR_TOOMANYCHANNELS    405
#define ERR_WASNOSUCHNICK      406
#define ERR_TOOMANYTARGETS     407
#define ERR_NOSUCHSERVICE      408
#define ERR_NOORIGIN           409

#define ERR_NORECIPIENT        411
#define ERR_NOTEXTTOSEND       412
#define ERR_NOTOPLEVEL         413
#define ERR_WILDTOPLEVEL       414
#define ERR_BADMASK            415

#define ERR_UNKNOWNCOMMAND     421
#define ERR_NOMOTD             422
#define ERR_NOADMININFO        423
#define ERR_FILEERROR          424

#define ERR_NONICKNAMEGIVEN    431
#define ERR_ERRONEUSNICKNAME   432
#define ERR_NICKNAMEINUSE      433

#define ERR_NICKCOLLISION      436
#define ERR_UNAVAILRESOURCE    437

#define ERR_USERNOTINCHANNEL   441
#define ERR_NOTONCHANNEL       442
#define ERR_USERONCHANNEL      443
#define ERR_NOLOGIN            444
#define ERR_SUMMONDISABLED     445
#define ERR_USERSDISABLED      446

#define ERR_NOTREGISTERED      451

#define ERR_NEEDMOREPARAMS     461
#define ERR_ALREADYREGISTRED   462
#define ERR_NOPERMFORHOST      463
#define ERR_PASSWDMISMATCH     464
#define ERR_YOUREBANNEDCREEP   465
#define ERR_YOUWILLBEBANNED    466
#define ERR_KEYSET             467

#define ERR_CHANNELISFULL      471
#define ERR_UNKNOWNMODE        472
#define ERR_INVITEONLYCHAN     473
#define ERR_BANNEDFROMCHAN     474
#define ERR_BADCHANNELKEY      475
#define ERR_BADCHANMASK        476
#define ERR_NOCHANMODES        477
#define ERR_BANLISTFULL        478

#define ERR_NOPRIVILEGES       481
#define ERR_CHANOPRIVSNEEDED   482
#define ERR_CANTKILLSERVER     483
#define ERR_RESTRICTED         484
#define ERR_UNIQOPPRIVSNEEDED  485

#define ERR_NOOPERHOST         491
#define ERR_NOSERVICEHOST      492

/*********************************/

#define ERR_UMODEUNKNOWNFLAG   501
#define ERR_USERSDONTMATCH     502

/*********************************/

#define USER_MODE_AWAY        0x01
#define USER_MODE_INVISIBLE   0x02
#define USER_MODE_GETWALLOPS  0x04
#define USER_MODE_OPER        0x08
#define USER_MODE_LOCALOPER   0x10

/*********************************/

#define CHAN_TYPE_NORMAL  '#'
#define CHAN_TYPE_SPECIAL '!'
#define CHAN_TYPE_LOCAL   '&'
#define CHAN_TYPE_SAFE    '+'

#define CHAN_FLAG_ANON       0x01 /* a - anonymous */
#define CHAN_FLAG_INVITE     0x02 /* i - invite-only */
#define CHAN_FLAG_MODERATED  0x04 /* m - moderated */
#define CHAN_FLAG_NOEXTERN   0x08 /* n - no external msgs */
#define CHAN_FLAG_QUIET      0x10 /* q - quiet */
#define CHAN_FLAG_SECRET     0x20 /* s - secret */
#define CHAN_FLAG_REOP       0x40 /* r - reop */
#define CHAN_FLAG_TLOCKED    0x80 /* t - topic locked (op only) */
#define CHAN_FLAG_USERLIM    0x10 /* l - user limit */

#define chan_special(c)  ((c) == CHAN_TYPE_SPECIAL)
#define chan_standard(c) (!chan_special(c))

/*********************************/

#define MEMBER_FLAG_CREATOR    0x01 /* O - creator */
#define MEMBER_FLAG_OPERATOR   0x02 /* o - op */
#define MEMBER_FLAG_VOICE      0x04 /* v - voice */
#define MEMBER_FLAG_HALFOP     0x08 /* h - half-op */

/*********************************/

#define MAX_ARGS 15
#define MAX_CMD  510
#define MAX_NICK 9
#define MAX_CHAN_NAME 50
#define MAX_CHAN_TOPIC 510
#define MAX_CHAN_KEY   63

typedef struct {
	char  raw[MAX_CMD+1];
	char  buffer[MAX_CMD+1];
	char *prefix;
	char *command;
	char *args[MAX_ARGS];
} msg_t;

typedef struct {
	int  sockfd;
	char nick[MAX_NICK+1];

	uint8_t mode;
} session_t;

typedef struct {
	char type;
	char name[MAX_CHAN_NAME+1];
	char topic[MAX_CHAN_TOPIC+1];

	char key[MAX_CHAN_KEY+1];
	size_t userlim;
	/* FIXME: ban/exclude/invite masks */

	uint8_t mode;
} channel_t;

typedef struct {
	session_t *sess;
	channel_t *chan;
	int32_t joined;

	uint8_t mode;
} member_t;

void irc_toupper(char *s, size_t len);
void irc_tolower(char *s, size_t len);

msg_t* irc_parse_msg(const char *line, size_t len);

int wildcard(const char *str, const char *pat);

#endif
