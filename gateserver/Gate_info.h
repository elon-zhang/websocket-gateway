#ifndef GATE_INFO_H_
#define GATE_INFO_H_
extern int Glo_Now;


//#define GAME_USER_MAX 6
enum UserDeskState//玩家在牌桌中的状态
{
    U_STATE_SIDE = 5,         //旁观
    U_STATE_IN_DESK = 10,       //座在桌子上
    U_STATE_OVER = 52           //不在桌子上
};

#define MAX_MSGBUFF_LEN (5120*1024)//最大消息缓存5M

#endif // !GATE_INFO_H_
