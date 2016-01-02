/*
Copyright 2015, Paulo Vinicius Wolski Radtke (pvwradtke@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <math.h>
#include <assert.h>
#include <c2d3/chien2d.h>

typedef unsigned int C2D_MapaParaSDL[2];

// A função externa para iniciar o módulo de áudio
void CA_Inicia();
// A função externa para encerrar o módulo de áudio
void CA_Encerra();
// A função interna que adiciona um gamepad. Não deve ser exposta ao programador.
bool C2D_AdicionaGamepadEvento(const unsigned int id);
// A função interna que remove um gamepad. Não deve ser exposta ao programador.
void C2D_RemoveGamepadEvento(const unsigned int id);


// Área de variáveis globais da Chien2D

// indica se a Chien2D 2 já foi inicializada. Por default, não foi ainda.
bool inicializado = false;
// A tela principal (dividida na tela física, Window, renderer e na tela lógica, alvo)
SDL_Window *screen;
SDL_Renderer *renderer;
SDL_Texture *alvo;
// O vetor com os sprite sets
C2D_SpriteSet sprites[C2D_MAX_SPRITESET];
// Finalmente, o vetor com as fontes
C2D_Fonte fontes[C2D_MAX_FONTES];
// Indica se o sistema está em shutdown ou não
bool c_shutdown = false;
// Cores de limpeza da tela
Uint8 limpaR=0, limpaG=0, limpaB=0;

// O vetor de teclas utilizadas
static C2D_Botao teclas[C2D_MAX_TECLAS] = {0};
// O mouse do sistema
static C2D_Mouse mouse = {0};
// Variável do Joystick
static C2D_Gamepad gamepads[C2D_MAX_GAMEPADS] = {0};
// O buffer de textos
static C2D_Texto textos[C2D_MAX_TEXTOS] = {0};

// Mapeamento das teclas da SDL para as teclas da C2D
static C2D_MapaParaSDL mapaTeclas[C2D_MAX_TECLAS-1]={
    {C2D_TCIMA, SDLK_UP}, {C2D_TBAIXO, SDLK_DOWN}, {C2D_TDIREITA, SDLK_RIGHT}, {C2D_TESQUERDA, SDLK_LEFT},
    {C2D_TESC, SDLK_ESCAPE}, {C2D_TF1, SDLK_F1}, {C2D_TF2, SDLK_F2}, {C2D_TF3, SDLK_F3}, {C2D_TF4, SDLK_F4}, {C2D_TF5, SDLK_F5}, {C2D_TF6, SDLK_F6},
    {C2D_TF7, SDLK_F7}, {C2D_TF8, SDLK_F8}, {C2D_TF9, SDLK_F9}, {C2D_TF10, SDLK_F10}, {C2D_TF11, SDLK_F11}, {C2D_TF12, SDLK_F12},
    {C2D_TA, SDLK_a}, {C2D_TB, SDLK_b}, {C2D_TC, SDLK_c}, {C2D_TD, SDLK_d}, {C2D_TE, SDLK_e}, {C2D_TF, SDLK_f}, {C2D_TG, SDLK_g},
    {C2D_TH, SDLK_h}, {C2D_TI, SDLK_i}, {C2D_TJ, SDLK_j}, {C2D_TK, SDLK_k}, {C2D_TL, SDLK_l}, {C2D_TM, SDLK_m}, {C2D_TN, SDLK_n}, {C2D_TO, SDLK_o},
    {C2D_TP, SDLK_p}, {C2D_TQ, SDLK_q}, {C2D_TR, SDLK_r}, {C2D_TS, SDLK_s}, {C2D_TT, SDLK_t}, {C2D_TU, SDLK_u}, {C2D_TV, SDLK_v}, {C2D_TW, SDLK_w},
    {C2D_TX, SDLK_x}, {C2D_TY, SDLK_y}, {C2D_TZ, SDLK_z},
    {C2D_T0, SDLK_0}, {C2D_T1, SDLK_1}, {C2D_T2, SDLK_2}, {C2D_T3, SDLK_3}, {C2D_T4, SDLK_4}, {C2D_T5, SDLK_5},
    {C2D_T6, SDLK_6}, {C2D_T7, SDLK_7}, {C2D_T8, SDLK_8}, {C2D_T9, SDLK_9},
    {C2D_TENTER, SDLK_RETURN}, {C2D_TESPACO, SDLK_SPACE}, {C2D_TLALT, SDLK_LALT}, {C2D_TRALT, SDLK_RALT},
    {C2D_TLCTRL, SDLK_LCTRL}, {C2D_TRCTRL, SDLK_RCTRL}, {C2D_TLSHIFT, SDLK_LSHIFT}, {C2D_TRSHIFT, SDLK_RSHIFT},
    {C2D_THOME, SDLK_HOME}, {C2D_TEND, SDLK_END}, {C2D_TINSERT, SDLK_INSERT}, {C2D_TDELETE, SDLK_DELETE}, {C2D_TPAGEUP, SDLK_PAGEUP}, {C2D_TPAGEDOWN, SDLK_PAGEDOWN},
};

// Mapeamento dos botões do mouse da SDL para os botões da C2D
static C2D_MapaParaSDL mapaBotoesMouse[C2D_MAX_MBOTOES]={
    {C2D_MESQUERDO, SDL_BUTTON_LEFT}, {C2D_MDIREITO, SDL_BUTTON_RIGHT}, {C2D_MMEIO, SDL_BUTTON_MIDDLE}
};

// Mapeamento dos botões do gamepad (inclusive direcional) para os botões do gamepad da C2D
static C2D_MapaParaSDL mapaBotoesGamepad[C2D_GMAX_BOTOES] = {
    {C2D_GBOTAO_A, SDL_CONTROLLER_BUTTON_A}, {C2D_GBOTAO_B, SDL_CONTROLLER_BUTTON_B}, {C2D_GBOTAO_X, SDL_CONTROLLER_BUTTON_X},
    {C2D_GBOTAO_Y, SDL_CONTROLLER_BUTTON_Y}, {C2D_GBOTAO_L, SDL_CONTROLLER_BUTTON_LEFTSHOULDER}, {C2D_GBOTAO_R, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    {C2D_GBOTAO_START, SDL_CONTROLLER_BUTTON_START}, {C2D_GBOTAO_LS, SDL_CONTROLLER_BUTTON_LEFTSTICK}, {C2D_GBOTAO_RS, SDL_CONTROLLER_BUTTON_RIGHTSTICK},
    {C2D_GPAD_CIMA, SDL_CONTROLLER_BUTTON_DPAD_UP}, {C2D_GPAD_BAIXO, SDL_CONTROLLER_BUTTON_DPAD_DOWN}, {C2D_GPAD_ESQUERDA, SDL_CONTROLLER_BUTTON_DPAD_LEFT},
    {C2D_GPAD_DIREITA, SDL_CONTROLLER_BUTTON_DPAD_RIGHT}
};

// Mapeamento dos eixos dos botões
static C2D_MapaParaSDL mapaEixosGamepad[C2D_GMAX_EIXOS] = {
    {C2D_GLEIXOX, SDL_CONTROLLER_AXIS_LEFTX}, {C2D_GLEIXOY, SDL_CONTROLLER_AXIS_LEFTY}, {C2D_GREIXOX, SDL_CONTROLLER_AXIS_RIGHTX},
    {C2D_GREIXOY, SDL_CONTROLLER_AXIS_RIGHTY}, {C2D_GLTRIGGER, SDL_CONTROLLER_AXIS_TRIGGERLEFT}, {C2D_GRTRIGGER, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}
};

// Mapeamentos de controle, extraídos de https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
// Useful regex to update this list and format it to C code: s/^\(.*\)$/"\1"/
static char mapa_controles[] =
"# Windows - DINPUT\n"
"8f0e1200000000000000504944564944,Acme,platform:Windows,x:b2,a:b0,b:b1,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b5,rightshoulder:b6,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a2,\n"
"341a3608000000000000504944564944,Afterglow PS3 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Windows,\n"
"ffff0000000000000000504944564944,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,platform:Windows,\n"
"6d0416c2000000000000504944564944,Generic DirectInput Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Windows,\n"
"6d0419c2000000000000504944564944,Logitech F710 Gamepad,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Windows,\n"
"88880803000000000000504944564944,PS3 Controller,a:b2,b:b1,back:b8,dpdown:h0.8,dpleft:h0.4,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b9,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:b7,rightx:a3,righty:a4,start:b11,x:b0,y:b3,platform:Windows,\n"
"4c056802000000000000504944564944,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,platform:Windows,\n"
"25090500000000000000504944564944,PS3 DualShock,a:b2,b:b1,back:b9,dpdown:h0.8,dpleft:h0.4,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b6,leftstick:b10,lefttrigger:b4,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b11,righttrigger:b5,rightx:a2,righty:a3,start:b8,x:b0,y:b3,platform:Windows,\n"
"4c05c405000000000000504944564944,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,platform:Windows,\n"
"6d0418c2000000000000504944564944,Logitech RumblePad 2 USB,platform:Windows,x:b0,a:b1,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"36280100000000000000504944564944,OUYA Controller,platform:Windows,a:b0,b:b3,y:b2,x:b1,start:b14,guide:b15,leftstick:b6,rightstick:b7,leftshoulder:b4,rightshoulder:b5,dpup:b8,dpleft:b10,dpdown:b9,dpright:b11,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:b12,righttrigger:b13,\n"
"4f0400b3000000000000504944564944,Thrustmaster Firestorm Dual Power,a:b0,b:b2,y:b3,x:b1,start:b10,guide:b8,back:b9,leftstick:b11,rightstick:b12,leftshoulder:b4,rightshoulder:b6,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b5,righttrigger:b7,platform:Windows,\n"
"00f00300000000000000504944564944,RetroUSB.com RetroPad,a:b1,b:b5,x:b0,y:b4,back:b2,start:b3,leftshoulder:b6,rightshoulder:b7,leftx:a0,lefty:a1,platform:Windows,\n"
"00f0f100000000000000504944564944,RetroUSB.com Super RetroPort,a:b1,b:b5,x:b0,y:b4,back:b2,start:b3,leftshoulder:b6,rightshoulder:b7,leftx:a0,lefty:a1,platform:Windows,\n"
"28040140000000000000504944564944,GamePad Pro USB,platform:Windows,a:b1,b:b2,x:b0,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,leftx:a0,lefty:a1,lefttrigger:b6,righttrigger:b7,\n"
"ff113133000000000000504944564944,SVEN X-PAD,platform:Windows,a:b2,b:b3,y:b1,x:b0,start:b5,back:b4,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a4,lefttrigger:b8,righttrigger:b9,\n"
"8f0e0300000000000000504944564944,Piranha xtreme,platform:Windows,x:b3,a:b2,b:b1,y:b0,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b6,lefttrigger:b4,rightshoulder:b7,righttrigger:b5,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a2,\n"
"8f0e0d31000000000000504944564944,Multilaser JS071 USB,platform:Windows,a:b1,b:b2,y:b3,x:b0,start:b9,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,\n"
"10080300000000000000504944564944,PS2 USB,platform:Windows,a:b2,b:b1,y:b0,x:b3,start:b9,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a4,righty:a2,lefttrigger:b4,righttrigger:b5,\n"
"79000600000000000000504944564944,G-Shark GS-GP702,a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a4,lefttrigger:b6,righttrigger:b7,platform:Windows,\n"
"4b12014d000000000000504944564944,NYKO AIRFLO,a:b0,b:b1,x:b2,y:b3,back:b8,guide:b10,start:b9,leftstick:a0,rightstick:a2,leftshoulder:a3,rightshoulder:b5,dpup:h0.1,dpdown:h0.0,dpleft:h0.8,dpright:h0.2,leftx:h0.6,lefty:h0.12,rightx:h0.9,righty:h0.4,lefttrigger:b6,righttrigger:b7,platform:Windows,\n"
"d6206dca000000000000504944564944,PowerA Pro Ex,a:b1,b:b2,x:b0,y:b3,back:b8,guide:b12,start:b9,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.0,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,platform:Windows,\n"
"a3060cff000000000000504944564944,Saitek P2500,a:b2,b:b3,y:b1,x:b0,start:b4,guide:b10,back:b5,leftstick:b8,rightstick:b9,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,platform:Windows,\n"
"8f0e0300000000000000504944564944,Trust GTX 28,a:b2,b:b1,y:b0,x:b3,start:b9,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,platform:Windows,\n"
"4f0415b3000000000000504944564944,Thrustmaster Dual Analog 3.2,platform:Windows,x:b1,a:b0,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b5,rightshoulder:b6,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"6f0e1e01000000000000504944564944,Rock Candy Gamepad for PS3,platform:Windows,a:b1,b:b2,x:b0,y:b3,back:b8,start:b9,guide:b12,leftshoulder:b4,rightshoulder:b5,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,\n"
"83056020000000000000504944564944,iBuffalo USB 2-axis 8-button Gamepad,a:b1,b:b0,y:b2,x:b3,start:b7,back:b6,leftshoulder:b4,rightshoulder:b5,leftx:a0,lefty:a1,platform:Windows,\n"
"\n"
"# OS X\n"
"0500000047532047616d657061640000,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,platform:Mac OS X,\n"
"6d0400000000000016c2000000000000,Logitech F310 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Mac OS X,\n"
"6d0400000000000018c2000000000000,Logitech F510 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Mac OS X,\n"
"6d040000000000001fc2000000000000,Logitech F710 Gamepad (XInput),a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,platform:Mac OS X,\n"
"6d0400000000000019c2000000000000,Logitech Wireless Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Mac OS X,\n"
"4c050000000000006802000000000000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,platform:Mac OS X,\n"
"4c05000000000000c405000000000000,PS4 Controller,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,Platform:Mac OS X,\n"
"5e040000000000008e02000000000000,X360 Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,platform:Mac OS X,\n"
"891600000000000000fd000000000000,Razer Onza Tournament,a:b0,b:b1,y:b3,x:b2,start:b8,guide:b10,back:b9,leftstick:b6,rightstick:b7,leftshoulder:b4,rightshoulder:b5,dpup:b11,dpleft:b13,dpdown:b12,dpright:b14,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,platform:Mac OS X,\n"
"4f0400000000000000b3000000000000,Thrustmaster Firestorm Dual Power,a:b0,b:b2,y:b3,x:b1,start:b10,guide:b8,back:b9,leftstick:b11,rightstick:,leftshoulder:b4,rightshoulder:b6,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b5,righttrigger:b7,platform:Mac OS X,\n"
"8f0e0000000000000300000000000000,Piranha xtreme,platform:Mac OS X,x:b3,a:b2,b:b1,y:b0,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b6,lefttrigger:b4,rightshoulder:b7,righttrigger:b5,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a2,\n"
"0d0f0000000000004d00000000000000,HORI Gem Pad 3,platform:Mac OS X,a:b1,b:b2,y:b3,x:b0,start:b9,guide:b12,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,\n"
"79000000000000000600000000000000,G-Shark GP-702,a:b2,b:b1,x:b3,y:b0,back:b8,start:b9,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:b6,righttrigger:b7,platform:Mac OS X,\n"
"4f0400000000000015b3000000000000,Thrustmaster Dual Analog 3.2,platform:Mac OS X,x:b1,a:b0,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b5,rightshoulder:b6,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"AD1B00000000000001F9000000000000,Gamestop BB-070 X360 Controller,a:b0,b:b1,back:b9,dpdown:b12,dpleft:b13,dpright:b14,dpup:b11,guide:b10,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,start:b8,x:b2,y:b3,platform:Mac OS X,\n"
"050000005769696d6f74652028303000,Wii Remote,a:b4,b:b5,y:b9,x:b10,start:b6,guide:b8,back:b7,dpup:b2,dpleft:b0,dpdown:b3,dpright:b1,leftx:a0,lefty:a1,lefttrigger:b12,righttrigger:,leftshoulder:b11,platform:Mac OS X,\n"
"\n"
"# Linux\n"
"0500000047532047616d657061640000,GameStop Gamepad,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,platform:Linux,\n"
"03000000ba2200002010000001010000,Jess Technology USB Game Controller,a:b2,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,righttrigger:b7,rightx:a3,righty:a2,start:b9,x:b3,y:b0,platform:Linux,\n"
"030000006d04000019c2000010010000,Logitech Cordless RumblePad 2,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Linux,\n"
"030000006d0400001dc2000014400000,Logitech F310 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000006d0400001ec2000020200000,Logitech F510 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000006d04000019c2000011010000,Logitech F710 Gamepad (DInput),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Linux,\n"
"030000006d0400001fc2000005030000,Logitech F710 Gamepad (XInput),a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000004c0500006802000011010000,PS3 Controller,a:b14,b:b13,back:b0,dpdown:b6,dpleft:b7,dpright:b5,dpup:b4,guide:b16,leftshoulder:b10,leftstick:b1,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b11,rightstick:b2,righttrigger:b9,rightx:a2,righty:a3,start:b3,x:b15,y:b12,platform:Linux,\n"
"030000004c050000c405000011010000,Sony DualShock 4,a:b1,b:b2,y:b3,x:b0,start:b9,guide:b12,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a5,lefttrigger:b6,righttrigger:b7,platform:Linux,\n"
"030000006f0e00003001000001010000,EA Sports PS3 Controller,platform:Linux,a:b1,b:b2,y:b3,x:b0,start:b9,guide:b12,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,\n"
"03000000de280000ff11000001000000,Valve Streaming Gamepad,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000005e0400008e02000014010000,X360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000005e0400008e02000010010000,X360 Controller,a:b0,b:b1,back:b6,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"030000005e0400001907000000010000,X360 Wireless Controller,a:b0,b:b1,back:b6,dpdown:b14,dpleft:b11,dpright:b12,dpup:b13,guide:b8,leftshoulder:b4,leftstick:b9,lefttrigger:a2,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b10,righttrigger:a5,rightx:a3,righty:a4,start:b7,x:b2,y:b3,platform:Linux,\n"
"03000000100800000100000010010000,Twin USB PS2 Adapter,a:b2,b:b1,y:b0,x:b3,start:b9,guide:,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a3,righty:a2,lefttrigger:b4,righttrigger:b5,platform:Linux,\n"
"03000000a306000023f6000011010000,Saitek Cyborg V.1 Game Pad,a:b1,b:b2,y:b3,x:b0,start:b9,guide:b12,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a4,lefttrigger:b6,righttrigger:b7,platform:Linux,\n"
"030000004f04000020b3000010010000,Thrustmaster 2 in 1 DT,a:b0,b:b2,y:b3,x:b1,start:b9,guide:,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b6,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b5,righttrigger:b7,platform:Linux,\n"
"030000004f04000023b3000000010000,Thrustmaster Dual Trigger 3-in-1,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.0,dpdown:h0.4,dpright:h0.0,dpright:h0.2,dpup:h0.0,dpup:h0.1,leftshoulder:h0.0,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a5,\n"
"030000008f0e00000300000010010000,GreenAsia Inc.    USB Joystick     ,platform:Linux,x:b3,a:b2,b:b1,y:b0,back:b8,start:b9,dpleft:h0.8,dpdown:h0.0,dpdown:h0.4,dpright:h0.0,dpright:h0.2,dpup:h0.0,dpup:h0.1,leftshoulder:h0.0,leftshoulder:b6,lefttrigger:b4,rightshoulder:b7,righttrigger:b5,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a2,\n"
"030000008f0e00001200000010010000,GreenAsia Inc.      USB  Joystick  ,platform:Linux,x:b2,a:b0,b:b1,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b5,rightshoulder:b6,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a2,\n"
"030000005e0400009102000007010000,X360 Wireless Controller,a:b0,b:b1,y:b3,x:b2,start:b7,guide:b8,back:b6,leftstick:b9,rightstick:b10,leftshoulder:b4,rightshoulder:b5,dpup:b13,dpleft:b11,dpdown:b14,dpright:b12,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,platform:Linux,\n"
"030000006d04000016c2000010010000,Logitech Logitech Dual Action,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.0,dpdown:h0.4,dpright:h0.0,dpright:h0.2,dpup:h0.0,dpup:h0.1,leftshoulder:h0.0,dpup:h0.1,leftshoulder:h0.0,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"03000000260900008888000000010000,GameCube {WiseGroup USB box},a:b0,b:b2,y:b3,x:b1,start:b7,leftshoulder:,rightshoulder:b6,dpup:h0.1,dpleft:h0.8,rightstick:,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,platform:Linux,\n"
"030000006d04000011c2000010010000,Logitech WingMan Cordless RumblePad,a:b0,b:b1,y:b4,x:b3,start:b8,guide:b5,back:b2,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:b9,righttrigger:b10,platform:Linux,\n"
"030000006d04000018c2000010010000,Logitech Logitech RumblePad 2 USB,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"05000000d6200000ad0d000001000000,Moga Pro,platform:Linux,a:b0,b:b1,y:b3,x:b2,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a5,righttrigger:a4,\n"
"030000004f04000009d0000000010000,Thrustmaster Run N Drive Wireless PS3,platform:Linux,a:b1,b:b2,x:b0,y:b3,start:b9,guide:b12,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,\n"
"030000004f04000008d0000000010000,Thrustmaster Run N Drive  Wireless,platform:Linux,a:b1,b:b2,x:b0,y:b3,start:b9,back:b8,leftstick:b10,rightstick:b11,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a5,lefttrigger:b6,righttrigger:b7,\n"
"0300000000f000000300000000010000,RetroUSB.com RetroPad,a:b1,b:b5,x:b0,y:b4,back:b2,start:b3,leftshoulder:b6,rightshoulder:b7,leftx:a0,lefty:a1,platform:Linux,\n"
"0300000000f00000f100000000010000,RetroUSB.com Super RetroPort,a:b1,b:b5,x:b0,y:b4,back:b2,start:b3,leftshoulder:b6,rightshoulder:b7,leftx:a0,lefty:a1,platform:Linux,\n"
"030000006f0e00001f01000000010000,Generic X-Box pad,platform:Linux,x:b2,a:b0,b:b1,y:b3,back:b6,guide:b8,start:b7,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:a2,rightshoulder:b5,righttrigger:a5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,\n"
"03000000280400000140000000010000,Gravis GamePad Pro USB ,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,start:b9,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftx:a0,lefty:a1,\n"
"030000005e0400008902000021010000,Microsoft X-Box pad v2 (US),platform:Linux,x:b3,a:b0,b:b1,y:b4,back:b6,start:b7,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b5,lefttrigger:a2,rightshoulder:b2,righttrigger:a5,leftstick:b8,rightstick:b9,leftx:a0,lefty:a1,rightx:a3,righty:a4,\n"
"030000006f0e00001e01000011010000,Rock Candy Gamepad for PS3,platform:Linux,a:b1,b:b2,x:b0,y:b3,back:b8,start:b9,guide:b12,leftshoulder:b4,rightshoulder:b5,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,\n"
"03000000250900000500000000010000,Sony PS2 pad with SmartJoy adapter,platform:Linux,a:b2,b:b1,y:b0,x:b3,start:b8,back:b9,leftstick:b10,rightstick:b11,leftshoulder:b6,rightshoulder:b7,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b4,righttrigger:b5,\n"
"030000008916000000fd000024010000,Razer Onza Tournament,a:b0,b:b1,y:b3,x:b2,start:b7,guide:b8,back:b6,leftstick:b9,rightstick:b10,leftshoulder:b4,rightshoulder:b5,dpup:b13,dpleft:b11,dpdown:b14,dpright:b12,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,platform:Linux,\n"
"030000004f04000000b3000010010000,Thrustmaster Firestorm Dual Power,a:b0,b:b2,y:b3,x:b1,start:b10,guide:b8,back:b9,leftstick:b11,rightstick:b12,leftshoulder:b4,rightshoulder:b6,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b5,righttrigger:b7,platform:Linux,\n"
"03000000ad1b000001f5000033050000,Hori Pad EX Turbo 2,a:b0,b:b1,y:b3,x:b2,start:b7,guide:b8,back:b6,leftstick:b9,rightstick:b10,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,platform:Linux,\n"
"050000004c050000c405000000010000,PS4 Controller (Bluetooth),a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,platform:Linux,\n"
"060000004c0500006802000000010000,PS3 Controller (Bluetooth),a:b14,b:b13,y:b12,x:b15,start:b3,guide:b16,back:b0,leftstick:b1,rightstick:b2,leftshoulder:b10,rightshoulder:b11,dpup:b4,dpleft:b7,dpdown:b6,dpright:b5,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b8,righttrigger:b9,platform:Linux,\n"
"03000000790000000600000010010000,DragonRise Inc.   Generic   USB  Joystick  ,platform:Linux,x:b3,a:b2,b:b1,y:b0,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a3,righty:a4,\n"
"03000000666600000488000000010000,Super Joy Box 5 Pro,platform:Linux,a:b2,b:b1,x:b3,y:b0,back:b9,start:b8,leftshoulder:b6,rightshoulder:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b4,righttrigger:b5,dpup:b12,dpleft:b15,dpdown:b14,dpright:b13,\n"
"05000000362800000100000002010000,OUYA Game Controller,a:b0,b:b3,dpdown:b9,dpleft:b10,dpright:b11,dpup:b8,guide:b14,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,platform:Linux,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,x:b1,y:b2,\n"
"05000000362800000100000003010000,OUYA Game Controller,a:b0,b:b3,dpdown:b9,dpleft:b10,dpright:b11,dpup:b8,guide:b14,leftshoulder:b4,leftstick:b6,lefttrigger:a2,leftx:a0,lefty:a1,platform:Linux,rightshoulder:b5,rightstick:b7,righttrigger:a5,rightx:a3,righty:a4,x:b1,y:b2,\n"
"030000008916000001fd000024010000,Razer Onza Classic Edition,platform:Linux,x:b2,a:b0,b:b1,y:b3,back:b6,guide:b8,start:b7,dpleft:b11,dpdown:b14,dpright:b12,dpup:b13,leftshoulder:b4,lefttrigger:a2,rightshoulder:b5,righttrigger:a5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,\n"
"030000005e040000d102000001010000,Microsoft X-Box One pad,platform:Linux,x:b2,a:b0,b:b1,y:b3,back:b6,guide:b8,start:b7,dpleft:h0.8,dpdown:h0.0,dpdown:h0.4,dpright:h0.0,dpright:h0.2,dpup:h0.0,dpup:h0.1,leftshoulder:h0.0,leftshoulder:b4,lefttrigger:a2,rightshoulder:b5,righttrigger:a5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,\n"
"03000000790000001100000010010000,RetroLink Saturn Classic Controller,platform:Linux,x:b3,a:b0,b:b1,y:b4,back:b5,guide:b2,start:b8,leftshoulder:b6,rightshoulder:b7,leftx:a0,lefty:a1,\n"
"050000007e0500003003000001000000,Nintendo Wii U Pro Controller,platform:Linux,a:b0,b:b1,x:b3,y:b2,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,leftstick:b11,rightstick:b12,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:b6,righttrigger:b7,dpup:b13,dpleft:b15,dpdown:b14,dpright:b16,\n"
"030000005e0400008e02000004010000,Microsoft X-Box 360 pad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b6,start:b7,guide:b8,leftshoulder:b4,rightshoulder:b5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,\n"
"030000000d0f00002200000011010000,HORI CO.,LTD. REAL ARCADE Pro.V3,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,guide:b12,start:b9,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,\n"
"030000000d0f00001000000011010000,HORI CO.,LTD. FIGHTING STICK 3,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,guide:b12,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7\n"
"03000000f0250000c183000010010000,Goodbetterbest Ltd USB Controller,platform:Linux,x:b0,a:b1,b:b2,y:b3,back:b8,guide:b12,start:b9,dpleft:h0.8,dpdown:h0.0,dpdown:h0.4,dpright:h0.0,dpright:h0.2,dpup:h0.0,dpup:h0.1,leftshoulder:h0.0,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
"0000000058626f782047616d65706100,Xbox Gamepad (userspace driver),platform:Linux,a:b0,b:b1,x:b2,y:b3,start:b7,back:b6,guide:b8,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftshoulder:b4,rightshoulder:b5,lefttrigger:a5,righttrigger:a4,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
;

// Poling das entradas. Tata todos os eventos relacionados a dispositivos de entrada do sistema
// Essa função é restrita ao sistema e não deve ser chamada pelo programador
void C2D_AtualizaEntradas();

// Apaga uma posição no vetor de textos
// Essa função é restrita ao sistema e não deve ser chamada pelo programador
//
void C2D_ApagaPosicaoTexto(const unsigned int idx);

// Procura uma posição livre no buffer de textos
// Essa função é restrita ao sistema e não deve ser chamada pelo programador
//
int C2D_ProcuraELiberaPosicaoTexto();


// Função que inicia a Chien2D 2
bool C2D_Inicia(const unsigned int largura, const unsigned int altura, const c2d_tipo_tela_t modoVideo, const char *tituloJanela, bool suaviza)
{
    if(inicializado)
        return false;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_EVENTS))
    {
        printf("Não conseguiu inicializar o modo de vídeo %dx%d com o'suporte necessário (vídeo, áudio e controles)", largura, altura);
        return false;
    }
    SDL_GameControllerEventState(true);
    // Determina a largura do desktop atual (usa isso como resolução alvo)
    unsigned int larguraFisica = largura;
    unsigned int alturaFisica = altura;
    SDL_DisplayMode mode;
    if(SDL_GetDesktopDisplayMode(0, &mode) != 0)
        return false;
    if((unsigned int)mode.w <= largura || (unsigned int)mode.h <= altura)
    {
        larguraFisica = mode.w;
        alturaFisica = mode.h;
    }
    // Escolhe os flags para janela e tela cheia
    Uint32 flags = 0;
    if(modoVideo == C2D_TELA_CHEIA)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP  ;
    else if(modoVideo == C2D_JANELA)
        flags |= SDL_WINDOW_RESIZABLE;
    else
        return false;
    // Cria a janela principal (física)
   	screen = SDL_CreateWindow(tituloJanela,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        larguraFisica, alturaFisica, flags);
    // Desabilita o screen saver
    SDL_DisableScreenSaver();
    // Desabilita o cursor do mouse
    SDL_ShowCursor(SDL_DISABLE);
    // Inicializa o renderer com a tela lógica (onde fazemos os desenhos)
	renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if(suaviza)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "");
	SDL_RenderSetLogicalSize(renderer, largura, altura);
	// Cria a textura alvo
	alvo = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, largura, altura);
	// O próximo desenho vai ser na textura alvo
	SDL_SetRenderTarget(renderer, alvo);
    // Inicializa o sistema de áudio
	Mix_Init( MIX_INIT_OGG);
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
	CA_Inicia();
	// Inicia o sistema de fontes
	TTF_Init();
    // Limpa os sprites (garante que está tudo vazio)
    for(int spr=0;spr<C2D_MAX_SPRITESET;spr++)
    {
        sprites[spr].apelido[0]=0;
        sprites[spr].textura=0;
    }
    // Inicializa o mapeamento dos gamepads
    SDL_RWops* mapa = SDL_RWFromConstMem(mapa_controles, sizeof(mapa_controles));
    if(mapa != 0)
    {

        if(SDL_GameControllerAddMappingsFromRW(mapa, 1) != -1)
            printf("Carregou mapeamento dos gamepads.\n");
        else
            printf("Falhou ao carregar o mapeamento dos gamepads.\n");
        //mapa->close(mapa);
    }
    else
        printf("Não conseguiu criar o arquivo do mapeamento dos gamepads.\n");
    // Finalmente, desabilita o screen saver
    SDL_DisableScreenSaver();
    return true;
}

// Função que indica as dimensões da tela
//
// Data: 25/01/2015
//
bool C2D_DimensoesTela(int *largura, int *altura)
{
	if(inicializado)
	{
		if(SDL_GetRendererOutputSize(renderer, largura, altura))
            return false;
        else
            return true;
	}
	else
		return false;
}

// Função que encerra a Chien2D
//
// Data: 25/01/2015
void C2D_Encerra()
{
    // Testa por via das dúvidas para não ter problemas
    if(!inicializado)
        return;
    // Indica que está encerrando
    c_shutdown = true;

    // Apaga os sprite sets
    for(int i=0;i<C2D_MAX_SPRITESET;i++)
        C2D_RemoveSpriteSet(i+1);

    // Apaga as fontes
    for(int i=0;i<C2D_MAX_FONTES;i++)
        C2D_RemoveFonte(i+1);

    // Apaga o buffer de textos
    for(unsigned int i=0; i<C2D_MAX_TEXTOS;i++)
        C2D_ApagaPosicaoTexto(i);

    printf("Encerrando a SDL.\n");
    // Encerra o sistema
    Mix_Quit();
    CA_Encerra();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
    inicializado=false;
}

// Função para sincronizar o vídeo
//
// Data: 25/01/2015
//
void C2D_Sincroniza(const Uint8 fps)
{
    // Tira a textura alvo
    SDL_SetRenderTarget(renderer, 0);
    SDL_RenderClear(renderer);
    // Desenha o alvo na tela
    SDL_RenderCopy(renderer, alvo, 0, 0);
    // Inicializa e pega o tempo atual (só faz isso a primeira vez)
    static Uint32 tempoAnterior=0;
	// Verifica se o parâmetro é válido
	if (fps > 0)
    {
        // -- Espera 1000/fps entre cada quadro --
        // Variável para o tempo atual
        int espera=(1000/fps)-SDL_GetTicks()+tempoAnterior;
        if(espera > 0)
           SDL_Delay(espera);
        tempoAnterior=SDL_GetTicks();
    }
    // Atualiza a tela
    SDL_RenderPresent(renderer);
    // Atualiza as entradas do sistema
    C2D_AtualizaEntradas();
    // Atualiza o contador de frames dos textos
    for(unsigned int i=0; i<C2D_MAX_TEXTOS;i++)
        if(textos[i].textura!=0)
            textos[i].framesAtras++;
    // Enfim, volta a usar o alvo para desenho
    SDL_SetRenderTarget(renderer, alvo);
}

// Função para limpar a tela
//
// Data: 25/01/2015
//
void C2D_LimpaTela()
{
    SDL_SetRenderDrawColor(renderer, limpaR, limpaG, limpaB, 255);
    SDL_RenderClear(renderer);
}

// Função para escolher a cor de limpeza da tela
//
// Data: 25/01/2015
//
void C2D_TrocaCorLimpezaTela(const Uint8 r, const Uint8 g, const Uint8 b)
{
    limpaR = r;
    limpaG = g;
    limpaB = b;
}


// Procura um spriteset pelo nome do arquivo original
//
// Data: 25/01/2015
//
unsigned int C2D_ProcuraSpriteSet(const char *apelido)
{
    // Índice de spriteset inválido, caso não encontre
    unsigned int idx=0;
    for(int i=0;i<C2D_MAX_SPRITESET;i++)
        if(strcmp(sprites[i].apelido, apelido)==0)
        {
            // O índice é a posição atual + 1
            idx=i+1;
            // Encerra a busca
            break;
        }
    return idx;
}

// função para carregar um sprite set na memória
//
// Data: 25/01/2015
//
unsigned int C2D_CarregaSpriteSet(const char *arquivo, const unsigned int largura, const unsigned int altura)
{
    // Verifica se o spriteset não existe já
    unsigned int idx = C2D_ProcuraSpriteSet(arquivo);
    // Se já existe um sprite com o nome do arquivo, retorna o índice associado
    if(idx != 0)
    {
		sprites[idx-1].usuarios=sprites[idx-1].usuarios+1;
        return idx;
    }
    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
    for(int i=0;i<C2D_MAX_SPRITESET;i++)
        // O spriteset é vago (não tem imagem associada?
        if(sprites[i].textura == 0)
        {
            idx=i+1;
            break;
        }
    // Testa se ainda tem espaço na lista
    //
    if(idx==0)
        return 0;

    // Se chegou até aqui, idx contém o identificador correto
    // Calcula a posição no array (sem incrementar 1)
    idx-=1;
    // Indica o caminho do arquivo no apelido
    strcpy(sprites[idx].apelido,arquivo);

    // Carrega a imagem no spriteset
    SDL_Surface *imagem = IMG_Load(arquivo);
    if (imagem == 0)
    {
		printf("A função C2D2_CarregaSPriteset falhou ao carregar o arquivo %s. Erro: %s.\n",
                arquivo, SDL_GetError());
        sprites[idx].apelido[0] = '\0';
        return 0;
    }
	// Indica o tamanho da imagem carregada
	sprites[idx].largura = imagem->w;
	sprites[idx].altura = imagem->h;

    sprites[idx].textura = SDL_CreateTextureFromSurface(renderer, imagem);
    SDL_FreeSurface(imagem);
    if (sprites[idx].textura == 0)
    {
		printf("A função C2D2_CarregaSPriteset falhou ao converter o arquivo %s para uma textura. Erro: %s.\n",
                arquivo, SDL_GetError());
        sprites[idx].apelido[0] = '\0';
        return 0;
    }

	// Verifica se pode usar como imagem dos sprites
	if(largura!=0 && altura!=0)
	{
		if(sprites[idx].largura%largura!=0 && sprites[idx].altura%altura!=0)
		{
			printf("A imagem %s não pode ser usada como fonte de sprites de tamanho %ix%i\n.",
				arquivo, largura, altura);
	        SDL_DestroyTexture(sprites[idx].textura);
	        sprites[idx].textura = 0;
			sprites[idx].apelido[0] = '\0';
			return 0;
		}
		else
		{
			sprites[idx].spLargura=largura;
			sprites[idx].spAltura=altura;
			sprites[idx].matrizX = sprites[idx].largura/largura;
			sprites[idx].matrizY = sprites[idx].altura/altura;
		}
	}
	else
	{
		sprites[idx].spLargura=sprites[idx].largura;
		sprites[idx].spAltura=sprites[idx].altura;
		sprites[idx].matrizX = 1;
		sprites[idx].matrizY = 1;
	}
	// Inicializa o centro e o bounding box do sprite (usa idx+1 para o índice correto no sistema)
	C2D_AlteraPropriedadesDoSprite(idx+1, largura/2, altura/2, 0, 0, largura, altura);
	// Indica que tem um usuário
    sprites[idx].usuarios = 1;
    return idx+1;
}

// Troca as propriedades do spriteset (centro para rotação e bounding box). Por default, o centro é normalizado e o bounding box é o sprite inteiro
bool C2D_AlteraPropriedadesDoSprite(const unsigned int id, const unsigned int xcentro, const unsigned int ycentro, const unsigned int bbX, const unsigned int bbY,
                                    const unsigned int bbLargura, const unsigned int bbAltura)
{
    if(id == 0 || id > C2D_MAX_SPRITESET)
        return false;
    int idx = id-1;
    if(sprites[idx].textura != 0)
    {
        sprites[idx].xcentro = xcentro;
        sprites[idx].ycentro = ycentro;
        sprites[idx].bbX = bbX;
        sprites[idx].bbY = bbY;
        sprites[idx].bbLargura = bbLargura;
        sprites[idx].bbAltura = bbAltura;
        return true;
    }
    else
        return false;
}

// Altera o alpha do sprite
//
// Data: 18/04/2015
//
void C2D_AlteraAlphaDoSprite(const unsigned int id, const Uint8 alpha)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
        SDL_SetTextureAlphaMod(sprites[idx].textura, alpha);
}

// restaura o alpha do sprite
//
// Data: 18/04/2015
//
void C2D_RestauraAlphaDoSprite(const unsigned int id)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
        SDL_SetTextureAlphaMod(sprites[idx].textura, 255);
}

// Altera o blend do sprite
//
// Data: 18/04/2015
//
void C2D_AlteraBlendDoSprite(const unsigned int id, const int modo)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
    {
        switch(modo)
        {
        case C2D_BLEND_NENHUM:
            SDL_SetTextureBlendMode(sprites[idx].textura, SDL_BLENDMODE_NONE);
            break;
        case C2D_BLEND_ALPHA:
            SDL_SetTextureBlendMode(sprites[idx].textura, SDL_BLENDMODE_BLEND);
            break;
        case C2D_BLEND_ADITIVO:
            SDL_SetTextureBlendMode(sprites[idx].textura, SDL_BLENDMODE_ADD);
            break;
        case C2D_BLEND_MODULACAO_COR:
            SDL_SetTextureBlendMode(sprites[idx].textura, SDL_BLENDMODE_MOD);
            break;
        }
    }
}

// restaura o blend original do sprite
//
// Data: 18/04/2015
//
void C2D_RestauraBlendDoSprite(const unsigned int id)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
        SDL_SetTextureBlendMode(sprites[idx].textura, SDL_BLENDMODE_NONE);
}

// Altera a cor do sprite
//
// Data: 18/04/2015
//
void C2D_AlteraCorDoSprite(const unsigned int id, const Uint8 r, const Uint8 g, const Uint8 b)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
        SDL_SetTextureColorMod(sprites[idx].textura, r, g, b);
}

// restaura a cor do sprite
//
// Data: 18/04/2015
//
void C2D_RestauraCorDoSprite(const unsigned int id)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Altera o blend
    if(sprites[idx].textura)
        SDL_SetTextureColorMod(sprites[idx].textura, 255, 255, 255);
}

// Remove um sprite set da memória
//
// Data: 25/01/2015
//
void C2D_RemoveSpriteSet(const unsigned int id)
{
    // O identificador é válido?
    if(id > C2D_MAX_SPRITESET || id == 0)
        return;
    unsigned int idx = id-1; // O índice verdadeiro dentro do vetor
    // Só apaga se o número de usuários for um ou se estiver em shutdown
    if(sprites[idx].usuarios > 1 && !c_shutdown)
    {
        sprites[idx].usuarios -= 1;
        return;
    }
    // Se cair aqui, é porquê não tem mais usuários
    sprites[idx].usuarios = 0;
	// tem a textura devidamente associada?
	if(sprites[idx].textura)
	{
	    SDL_DestroyTexture(sprites[idx].textura);
	    sprites[idx].textura = 0;
	}
	sprites[idx].apelido[0] = 0;
}

// Função básica para desenhar um sprite
//
// Data: 25/01/2015
//
bool C2D_DesenhaSprite(const unsigned int id, const unsigned int indice, const int x, const int y)
{
    static SDL_Rect clip, destino;

	// O id é válido?
    if(id == 0)
        return false;
    if(id > C2D_MAX_SPRITESET)
        return false;
    int idx = id-1;
    // O sprite existe mesmo, mesmo?
    if(sprites[idx].textura==0 || sprites[idx].apelido[0]==0)
            return false;
    // O índice é válido?
    if(indice >= (unsigned int)(sprites[idx].matrizX * sprites[idx].matrizY) || sprites[idx].textura==0)
        return false;
    // Calcula a região dentro do spriteset
    clip.x = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
    clip.y = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
    clip.w = sprites[idx].spLargura;
    clip.h = sprites[idx].spAltura;
    // Calcula o destino na tela
    destino.x = x;
    destino.y = y;
    destino.w = sprites[idx].spLargura;
    destino.h = sprites[idx].spAltura;
    SDL_RenderCopy(renderer, sprites[idx].textura, &clip, &destino);
	return true;

}

// Função para desenhar um sprite com efeitos básicos
//
// Data: 25/01/2015
//
bool C2D_DesenhaSpriteEspecial(const unsigned int id, const unsigned int indice, const int x, const int y, const c2d_flip flip, const double escalax, const double escalay, const double angulo)
{
    static SDL_Rect clip, destino;
    static SDL_RendererFlip sdlflip;
    static SDL_Point centro;

	// O id é válido?
    if(id == 0)
        return false;
    if(id > C2D_MAX_SPRITESET)
        return false;
    int idx = id-1;
    // O índice é válido?
    if(indice >= (unsigned int)(sprites[idx].matrizX * sprites[idx].matrizY) || sprites[idx].textura==0)
        return false;
    // Calcula a região dentro do spriteset
    clip.x = (indice%sprites[idx].matrizX)*sprites[idx].spLargura;
    clip.y = (indice/sprites[idx].matrizX)*sprites[idx].spAltura;
    clip.w = sprites[idx].spLargura;
    clip.h = sprites[idx].spAltura;
    // Calcula o destino na tela
    destino.x = x;
    destino.y = y;
    destino.w = (int)(sprites[idx].spLargura * escalax);
    destino.h = (int)(sprites[idx].spAltura * escalay);
    // Enfim, o centro
    centro.x = (int)(sprites[idx].xcentro*escalax);
    centro.y = (int)(sprites[idx].ycentro*escalay);
    switch(flip)
    {
        case C2D_FLIP_HORIZONTAL:
            sdlflip = SDL_FLIP_HORIZONTAL;
            break;
        case C2D_FLIP_VERTICAL:
            sdlflip = SDL_FLIP_VERTICAL;
            break;
        case C2D_FLIP_AMBOS:
            sdlflip = (SDL_RendererFlip)(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);
            break;
        default:
            sdlflip = SDL_FLIP_NONE;
    }
    SDL_RenderCopyEx(renderer, sprites[idx].textura, &clip, &destino, angulo, &centro, sdlflip);
	return true;
}


// Função para recuperar o tempo do sistema
//
// Data: 07/02/2015
//
unsigned long C2D_TempoSistema()
{
	return SDL_GetTicks();
}

// Função para dar uma pausa na aplicação.
//
// Data: 07/02/2015
//
void C2D_Pausa(const Uint32 pausa)
{
	SDL_Delay(pausa);
}

// Funções inline auxiliares. Não devem ser chamadas pelo programador de jogos

// Funções inline auxiliar para pressionar um botão
//
// Data: 15/02/2015
//
inline void C2D_PressionaBotao(C2D_Botao *botao)
{
	botao->pressionou=true;
	botao->liberou=false;
	botao->pressionando=true;
}
inline void C2D_LiberaBotao(C2D_Botao *botao)
{
	botao->pressionando=false;
	botao->liberou=true;
	botao->pressionou=false;
}
//
// função inline para processar uma tecla geral
//
// Data: 15/02/2015
//
inline void C2D_ProcessaTecla(C2D_Botao *tecla, Uint32 evento)
{
    switch(evento)
    {
        case SDL_KEYDOWN:
            tecla->pressionou=true;
            tecla->liberou=false;
            tecla->pressionando=true;
            break;
        case SDL_KEYUP:
            tecla->pressionando=false;
            tecla->liberou=true;
            tecla->pressionou=false;
            break;
    }
}

// Função inline para achar um gamepad pelo id do joystick
//
// Data: 15/02/2015
//
inline const int C2D_EncontraGamepadIDJoystick(const unsigned int id)
{
    int indice = -1;
    for(int i=0;i<=C2D_MAX_GAMEPADS;i++)
        if(gamepads[i].inicializado && id == gamepads[i].instanceID)
        {
            indice = i;
            break;
        }
    return indice;
}
// Função inline para achar o mapeamento de um elemento da SDL na Chien2D
//
// Data: 15/92/1015
//
inline const int C2D_AchaMapaSDL(const C2D_MapaParaSDL* mapa, const unsigned int sdl, const int tamanho)
{
    for(int i=0;i<tamanho;i++)
        if(mapa[i][1] == sdl)
            return mapa[i][0];
    return -1;
}

//---------------------------------------------------------
// Poling das entradas. TRata todos os eventos relacionados a dispositivos de
// entrada do sistema
//
// Data: 15/02/2015
//
void C2D_AtualizaEntradas()
{
    // Remove o pressionamento das teclas do passo anterior
	for(int i=0;i<C2D_MAX_TECLAS;i++)
		teclas[i].pressionou = false;
	// Remove o liberamento das teclas do passo anterior
	for(int i=0;i<C2D_MAX_TECLAS;i++)
		teclas[i].liberou = false;
	// Remove o pressionamento do mouse do passo anterior
	for(int i=0;i<C2D_MAX_MBOTOES;i++)
		mouse.botoes[i].pressionou = false;
	// Remove o liberamento do mpouse do passo anterior
	for(int i=0;i<C2D_MAX_MBOTOES;i++)
		mouse.botoes[i].liberou = false;
	for(int i = 0;i < C2D_MAX_GAMEPADS; ++i)
	{
		for(int j = 0;j < C2D_GMAX_BOTOES; ++j)
		{
			gamepads[i].botoes[j].liberou = false;
			gamepads[i].botoes[j].pressionou = false;
		}
	}

    // A estrutura que recebe os eventos
    SDL_Event event;
    // Enquanto houverem eventos a processar ...
    while(SDL_PollEvent( &event ))
    {
        // Trata de acordo com o tipo de evento
        switch( event.type )
        {
            case SDL_CONTROLLERDEVICEADDED:
                C2D_AdicionaGamepadEvento(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                printf("Vai remover\n");
                C2D_RemoveGamepadEvento(event.cdevice.which);
                break;

            case SDL_QUIT:
                C2D_PressionaBotao(&teclas[C2D_TENCERRA]);
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                int teclaC2D = C2D_AchaMapaSDL(mapaTeclas, event.key.keysym.sym, C2D_MAX_TECLAS-1);
                if(teclaC2D != -1)
                    C2D_ProcessaTecla(&teclas[teclaC2D], event.type);
                break;
            }
            case SDL_MOUSEMOTION:
                mouse.x=event.motion.x;
                mouse.y=event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                mouse.x=event.button.x;
                mouse.y=event.button.y;
                int botaoC2D = C2D_AchaMapaSDL(mapaBotoesMouse, event.button.button, C2D_MAX_MBOTOES);
                if(-1 != botaoC2D)
                {
                    if(event.type == SDL_MOUSEBUTTONDOWN)
                        C2D_PressionaBotao(&mouse.botoes[botaoC2D]);
                    else
                        C2D_LiberaBotao(&mouse.botoes[botaoC2D]);
                }
                break;
            }

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            {
                int indice=C2D_EncontraGamepadIDJoystick(event.cbutton.which);
                int botaoC2D = C2D_AchaMapaSDL(mapaBotoesGamepad, event.cbutton.button, C2D_GMAX_BOTOES);
                if(-1 != botaoC2D)
                {
                    if(event.type == SDL_CONTROLLERBUTTONDOWN)
                        C2D_PressionaBotao(&gamepads[indice].botoes[botaoC2D]);
                    else
                        C2D_LiberaBotao(&gamepads[indice].botoes[botaoC2D]);
                }
                break;
            }

            case SDL_CONTROLLERAXISMOTION:
            {
                int indice=C2D_EncontraGamepadIDJoystick(event.caxis.which);
                int leitura = event.caxis.value;
                // normaliza para um valor entre -100 e 100
                if(leitura >= 0)
                    leitura = (leitura*100)/32767;
                else
                    leitura = (leitura*100)/32768;
                int eixo = C2D_AchaMapaSDL(mapaEixosGamepad, event.caxis.axis, C2D_GMAX_EIXOS);
                gamepads[indice].eixos[eixo]=leitura;
                break;
            }

            default:
                break;
        }
     }
}

// Função para recuperar as teclas
//
// Data: 15/02/2015
//
const C2D_Botao* C2D_PegaTeclas()
{
	return teclas;
}

// Função para recuperar o mouse
//
// Data: 15/02/2015
//
const C2D_Mouse* C2D_PegaMouse()
{
	return &mouse;
}

// Função para recuperar o mouse
//
// Data: 15/02/2015
//
const C2D_Gamepad* C2D_PegaGamepads()
{
	return gamepads;
}

// Função para recuperar o nome de um gamepad
//
// Data: 15/02/2015
//
const char* C2D_PegaNomeGamepad(const unsigned int numGamepad)
{
    if(numGamepad >= C2D_MAX_GAMEPADS)
        return 0;
    if(gamepads[numGamepad].gamepad == 0)
        return 0;
    if(!C2D_GamepadEstaConectado(numGamepad))
        return 0;
    return SDL_GameControllerName(gamepads[numGamepad].gamepad);
}

// Função para verificar se um gamepad está conectado
//
// Data: 15/02/2015
//
bool C2D_GamepadEstaConectado(const unsigned int numGamepad)
{
    if(numGamepad >= C2D_MAX_GAMEPADS)
        return false;
    //if(SDL_GameControllerGetAttached(gamepads[numGamepad].gamepad))
    if(gamepads[numGamepad].inicializado)
        return true;
    else
        return false;
}

// A função interna que adiciona um gamepad. Não deve ser exposta ao programador.
//
// Data: 07/03/2015
//
bool C2D_AdicionaGamepadEvento(const unsigned int id)
{
    // É um controle válido?
    if (SDL_IsGameController(id))
    {
        //
        int gpIndex = -1;
        for(int i=0;i<C2D_MAX_GAMEPADS;i++)
            if(!gamepads[i].inicializado)
            {
                gpIndex=i;
                break;
            }
        if(gpIndex==-1)
            return false;
        gamepads[gpIndex].gamepad = SDL_GameControllerOpen(id);
        SDL_Joystick *joy = SDL_GameControllerGetJoystick(gamepads[gpIndex].gamepad);
        gamepads[gpIndex].instanceID = SDL_JoystickInstanceID( joy );
        gamepads[gpIndex].inicializado=true;
        printf("Encontrou controle %d: %s\n", gpIndex, C2D_PegaNomeGamepad(gpIndex));
    }
    else
        return false;
    return true;
}

// A função interna que remove um gamepad. Não deve ser exposta ao programador.
//
// Data: 07/03/2015
//
void C2D_RemoveGamepadEvento(const unsigned int id)
{
    // Procura o gamepad pelo id do joystick
    int gpIndex=-1;
    for(int i=0;i<C2D_MAX_GAMEPADS;i++)
        if(gamepads[i].inicializado && gamepads[i].instanceID == id)
        {
            gpIndex=i;
            break;
        }
    if(gpIndex==-1)
        return;
    // Fecha o gamepad e coloca apaga os dados da estrutura
    SDL_GameControllerClose(gamepads[gpIndex].gamepad);
    gamepads[gpIndex].gamepad = 0;
    gamepads[gpIndex].inicializado = false;
    printf("Removeu o controle %d.\n", gpIndex);
}


// função para carregar uma fonte na memória
//
// Data: 07/03/2015
//
unsigned int C2D_CarregaFonte(const char *arquivo, const char *apelido, const unsigned int tamanho)
{
    // Verifica se a fonte existe
    unsigned int idx = C2D_ProcuraFonte(arquivo);
    // Se já existe uma fonte com o apelido, retorna o índice associado
    if(idx != 0)
    {
        fontes[idx-1].usuarios = fontes[idx-1].usuarios+1;
        return idx;
    }

    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
    for(int i=0;i<C2D_MAX_FONTES;i++)
        // A fonte é vaga (não tem imagem associada?
		if(fontes[i].ttf == 0)
        {
            idx=i+1;
            break;
        }

    // ATENÇÃO: não ter espaço na memória não é relacionado a este teste. Apenas
    // testa-se aqui se existe um índice vago na lista. Se não existe, idx==0
    if(idx==0)

        // Retorna um índice inválido para indicar que o método falhou
        return 0;

    // Se chegou até aqui, idx contém o identificador correto

    // posição no array (sem incrementar 1)
    idx=idx-1;
    // Carrega a fonte, se falhar, retorna
    fontes[idx].ttf = TTF_OpenFont(arquivo, tamanho);
    if(!fontes[idx].ttf)
        return 0;
    // Indica o apelido
	strcpy(fontes[idx].apelido, apelido);
	fontes[idx].tamanho = tamanho;

    // Indica que tem um usuário
    fontes[idx].usuarios = 1;
    return idx+1;

}


// Remove uma fonte da memória
//
// Data: 24/04/2007
//
void C2D_RemoveFonte(const unsigned int id)
{
    // Está na faixa esperada para remover?
    if(id > C2D_MAX_FONTES || id==0)
        return;
    // Só remove se existe apenas um usuário da fonte ou se estiver em shutdown
    if(fontes[id-1].usuarios > 1 && !c_shutdown)
    {
        fontes[id-1].usuarios = fontes[id-1].usuarios-1;
        return;
    }

    if(fontes[id-1].ttf != 0)
    {
        TTF_CloseFont(fontes[id-1].ttf);
        fontes[id-1].ttf=0;
        fontes[id-1].apelido[0] = 0;
    }
    fontes[id-1].usuarios = 0;
}


// Procura uma fonte pelo nome do arquivo original
//
// Data: 24/04/2007
//
unsigned int C2D_ProcuraFonte(const char *apelido)
{
    for(int i=0;i<C2D_MAX_FONTES;i++)
		if(strcmp(fontes[i].apelido, apelido) == 0)
            // O índice é a posição atual + 1
            return i+1;
    return 0;
}

// Procura um texto no buffer de textos
//
// Data: 14/03/2015 (PI day! :D)
//
int C2D_ProcuraTextoExistente(const unsigned int idFonte, const char *texto)
{
    // Procura o texto, por default não existe
    for(int i=0;i<C2D_MAX_TEXTOS;i++)
    {
        if(idFonte == textos[i].idFonte)
            if(strcmp(textos[i].texto, texto)==0)
                return i;
    }
    // Se chegou aqui, é porque o texto é novo
    return -1;
}

// Apaga uma posição no vetor de textos
//
// Data: 14/03/2015 (PI day! :D)
//
void C2D_ApagaPosicaoTexto(const unsigned int idx)
{
    if(idx >= C2D_MAX_TEXTOS)
        return;
    if(textos[idx].textura != 0)
    {
        SDL_DestroyTexture(textos[idx].textura);
        textos[idx].texto[0]=0;
        textos[idx].framesAtras=0;
    }
}

// Procura uma posição livre no buffer de textos
//
// Data: 14/03/2015 (PI day! :D)
//
int C2D_ProcuraELiberaPosicaoTexto()
{
    // Procura uma posição livre (sem textura)
    for(int i=0;i<C2D_MAX_TEXTOS;i++)
    {
        // Se a posição anterior for mais velha, usa ela
        if(textos[i].textura == 0)
            return i;
    }
    // Se não tem uma posição livre, procura a mais antiga para liberar
    unsigned int idx=0;
    for(unsigned int i=1;i<C2D_MAX_TEXTOS;i++)
    {
        // Se a posição anterior for mais velha, usa ela
        if(textos[i].framesAtras > textos[idx].framesAtras)
            idx=i;
    }
    // Apaga o texto dessa posição
    C2D_ApagaPosicaoTexto(idx);
    // Retorna o ĩndice encontrado
    return idx;
}


// Desenha uma fonte só com uma cor de frente. O texto é armazenado no buffer de textos e, se usado em consecutivos frames, ele é redesenhado automaticamente.
// O texto deve ser UTF8!
//
// Data: 14/03/2015 (PI day! :D)
//
void C2D_DesenhaTexto(const unsigned int idFonte, const int x, const int y, const char *textoUTF8, const unsigned int alinhamento, uint8_t r, uint8_t g, uint8_t b,uint8_t a)
{
    static SDL_Rect dest;
    // Existe a fonte?
    if(idFonte > C2D_MAX_FONTES || idFonte == 0)
        return;
    if(fontes[idFonte-1].ttf == 0)
        return;
    // Testa se o texto entra na string
    if(strlen(textoUTF8) >= C2D_TAM_TEXTO)
        return;
    // O texto já existe?
    int idxTexto = C2D_ProcuraTextoExistente(idFonte, textoUTF8);
    // Se for negativo, tem que criar o texto
    if(idxTexto < 0)
    {
        idxTexto=C2D_ProcuraELiberaPosicaoTexto();
        //printf("Liberou o texto em %d para o texto '%s'\n", idxTexto, textoUTF8);
        SDL_Color cor= {r, g, b, a};
        SDL_Surface *desenhoTexto = 0;
        desenhoTexto = TTF_RenderUTF8_Blended(fontes[idFonte-1].ttf, textoUTF8, cor);
        if(desenhoTexto != 0)
        {
            // Se criou a textura, é porque dá pra desenhar o texto. Preenche TODOS os campos da estrutura!
            // frames atrás deve ser zero
            textos[idxTexto].framesAtras=0;
            textos[idxTexto].idFonte=idFonte;
            strcpy(textos[idxTexto].texto, textoUTF8);
            // Guarda a textura (pode ser que falhe ...)
            textos[idxTexto].textura = SDL_CreateTextureFromSurface(renderer, desenhoTexto);
            // Pega as dimensãoes do texto
            C2D_DimensoesTexto(idFonte, textoUTF8, &textos[idxTexto].largura, &textos[idxTexto].altura);
            // temporãrio, só para tocar o barco
            textos[idxTexto].xdesl=0;
            textos[idxTexto].ydesl=0;
            // Copia a cor do texto
            textos[idxTexto].r = r;
            textos[idxTexto].g = g;
            textos[idxTexto].b = b;
            textos[idxTexto].a = a;
            SDL_FreeSurface(desenhoTexto);
        }
    }
    //else
    //   printf("Reutilizou o texto em %d para o texto '%s'\n", idxTexto, textoUTF8);
    // Desenha a textura no renderer
    if(textos[idxTexto].textura != 0)
    {
        dest.x = x;
        dest.y = y;
        switch(alinhamento)
        {
        case C2D_TEXTO_DIREITA:
            dest.x-=textos[idxTexto].largura;
            break;
        case C2D_TEXTO_CENTRALIZADO:
            dest.x-=textos[idxTexto].largura/2;
            break;
        }
        dest.w = textos[idxTexto].largura;
        dest.h = textos[idxTexto].altura;
        SDL_RenderCopy(renderer, textos[idxTexto].textura, 0, &dest);
        // Zera ocontador de frames
        textos[idxTexto].framesAtras=0;
    }

}

// Função que calcula as dimensão de um texto UFT-8
// Data: 14/03/2015 (PI day! :D)
//
bool C2D_DimensoesTexto(unsigned int idFonte, const char *textoUTF8, int *largura, int *altura)
{
    if(idFonte > C2D_MAX_FONTES || idFonte == 0)
        return false;
    if(fontes[idFonte-1].ttf == 0)
        return false;
    if(TTF_SizeUTF8(fontes[idFonte-1].ttf, textoUTF8, largura, altura))
        return false;
    return true;
}
