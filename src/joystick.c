#include <SDL2/SDL.h>
#include "abc_runstate.h"


int joystick_buttom(SDL_JoyButtonEvent *ev)
{
    //SDL_JOYBUTTONDOWN
    //SDL_JOYBUTTONUP
    SDL_Log("joy id:%d,button: %d,state: %d",ev->which,ev->button,ev->state);
    return 0;
}
int joystick_axis(SDL_JoyAxisEvent *ev){
    if(ev->type != SDL_JOYAXISMOTION){
        SDL_Log("错误");
        return 0;
    }
    SDL_Log("joy id:%d,axis: %d,value: %d",ev->which,ev->axis,ev->value);
    return 0;
}
int joystick_hat(SDL_JoyHatEvent *ev){
    SDL_Log("joy id:%d,hat: %d,value: %d",ev->which,ev->hat,ev->value);
    return 0;
}

int threadJoystick(void *pl){
    RUNSTATE *rs=(RUNSTATE *)pl;
    // 游戏杆有改变
    int joynum=0;
    int joyoldnum=0;
    while(rs->runing){
        SDL_Delay(500);
        joynum=SDL_NumJoysticks();
        if(joynum<0){
            SDL_Log("SDL_NumJoysticks() error:%s",SDL_GetError());
            continue;
        }
        if(joyoldnum!=joynum){
            SDL_Log("检测到%d个手柄",joynum);
            joyoldnum=joynum;
            if(joynum){
                for(int ix=0;ix<joynum;ix++){
                    SDL_Joystick *js=SDL_JoystickOpen(ix);
                    if(js==NULL){
                        SDL_Log("SDL_JoystickOpen() error:%s",SDL_GetError());
                        continue;
                    }
                    const char *name=SDL_JoystickName(js);
                    if(ix==0){
                        rs->joys=js;
                        if(name){
                            SDL_Log("使用手柄\033[33m%s\033[0m",name);
                        } else {
                            SDL_Log("使用手柄０");
                        }
                        continue;
                    } else {
                        if(name){

                            SDL_Log("连接手柄\033[33m%s\033[0m",name);
                        } else {
                            SDL_Log("连接手柄%d",ix);
                        }
                    }
                    SDL_JoystickClose(js);
                }
            } else {
                SDL_Log("没有连接任何手柄");
                //SDL_JoystickClose(rs->joys);
            }
        }

    }
    return 0;
}