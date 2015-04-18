#include <stdio.h>
#include <c2d3/chien2d.h>
#include <c2d3/chienaudio.h>

int main(int argc, char **argv)
{
    unsigned int musicas[3];
    if(C2D_Inicia(800, 600, C2D_JANELA, "Teste da lib", true))
    {
        unsigned int id = C2D_CarregaSpriteSet("resources/darkphoenix.png", 32, 42);
        if(id==0)
            printf("Erro ao carregar o sprite\n");
        unsigned int fonte = C2D_CarregaFonte("resources/DanceParty.ttf", "Dance Party 32", 32);
        if(fonte==0)
            printf("Erro ao carregar a fonte\n");
        unsigned int fonteJ = C2D_CarregaFonte("resources/keifont.ttf", "Kei Font 32", 32);
        if(fonteJ==0)
            printf("Erro ao carregar a fonte\n");
        musicas[0] = CA_CarregaMusica("resources/ingame_345935_RD2010_Heavy_Terror_Machine_alextheDJ.ogg");
        musicas[1] = CA_CarregaMusica("resources/menu_312254_Emergency_alextheDJ.ogg");
        musicas[2] = CA_CarregaMusica("resources/victory_328597_En_Bombe_Din Lomme_Grillpie.ogg");
        unsigned int cor=0;
        int soma=1;
        CA_TocaMusica(musicas[0], -1);
        const C2D_Botao *teclado = C2D_PegaTeclas();
        const C2D_Gamepad *gamepads = C2D_PegaGamepads();
        const C2D_Mouse *mouse = C2D_PegaMouse();
        int xteclado=100, yteclado=100;
        int xpad=200, ypad=100, xlstick=200, ylstick=300;
        int fpad = 0, fstick=0, fmouse=0;
        bool sai = false;
        int frame=0;
        while(!sai)
        {
            if(teclado[C2D_TENCERRA].pressionou)
                sai=true;
            if(teclado[C2D_TESC].pressionou)
                sai=true;
            if(C2D_GamepadEstaConectado(0))
                if(gamepads[0].botoes[C2D_GBOTAO_B].pressionou)
                    sai = true;
            if(teclado[C2D_T1].pressionou)
                CA_TocaMusica(musicas[0], -1);
            else if(teclado[C2D_T2].pressionando)
                CA_TocaMusica(musicas[1], -1);
            else if(teclado[C2D_T3].pressionando)
                CA_TocaMusica(musicas[2], -1);
            if(teclado[C2D_TCIMA].pressionando)
                yteclado--;
            else if(teclado[C2D_TBAIXO].pressionando)
                yteclado++;
            if(teclado[C2D_TESQUERDA].pressionando)
                xteclado--;
            else if(teclado[C2D_TDIREITA].pressionando)
                xteclado++;
            if(cor==255 && soma==1)
                soma=-1;
            else if(cor==0 && soma ==-1)
                soma=1;
            if(mouse->botoes[C2D_MESQUERDO].pressionou)
                if((fmouse=fmouse-1)<0)
                    fmouse=12;
            if(mouse->botoes[C2D_MDIREITO].pressionou)
                fmouse=(fmouse+1)%13;
            if(mouse->botoes[C2D_MMEIO].pressionou)
                fmouse=0;
            if(C2D_GamepadEstaConectado(0))
            {
                if(gamepads[0].botoes[C2D_GPAD_CIMA].pressionando)
                    ypad--;
                else if(gamepads[0].botoes[C2D_GPAD_BAIXO].pressionando)
                    ypad++;
                if(gamepads[0].botoes[C2D_GPAD_ESQUERDA].pressionando)
                    xpad--;
                else if(gamepads[0].botoes[C2D_GPAD_DIREITA].pressionando)
                    xpad++;
                if(gamepads[0].botoes[C2D_GBOTAO_Y].pressionou)
                    fpad = (fpad+1)%13;
                if(gamepads[0].botoes[C2D_GBOTAO_X].pressionou)
                    if((fpad = (fpad-1)) < 0)
                        fpad=12;
                if(gamepads[0].eixos[C2D_GLEIXOY] < -20)
                    ylstick--;
                if(gamepads[0].eixos[C2D_GLEIXOY] > 20)
                    ylstick++;
                if(gamepads[0].eixos[C2D_GLEIXOX] < -20)
                    xlstick--;
                if(gamepads[0].eixos[C2D_GLEIXOX] > 20)
                    xlstick++;
                if(gamepads[0].botoes[C2D_GBOTAO_R].pressionou)
                    fstick = (fstick+1)%13;
                if(gamepads[0].botoes[C2D_GBOTAO_L].pressionou)
                    if((fstick = (fstick-1)) < 0)
                        fstick=12;
                //printf("Leitura do trigger direito: %d\n", gamepads[0].eixos[C2D_GRTRIGGER]);
            }
            cor+=soma;
            C2D_TrocaCorLimpezaTela((Uint8)cor, (Uint8)cor, (Uint8)cor);
            C2D_LimpaTela();
            C2D_DesenhaSprite(id, cor/20, 0, 0);
            C2D_DesenhaSpriteEspecial(id, 0, 320, 240, C2D_FLIP_NENHUM, 1.0+255/100.0, 1.0+255/100.0, 0);
            C2D_DesenhaSpriteEspecial(id, 0, 320-16*cor/10.0, 240-21*cor/10, C2D_FLIP_NENHUM, 1.0+cor/10.0, 1.0+cor/10.0, (double)cor);
            C2D_DesenhaSprite(id, 0, 320, 240);
            C2D_DesenhaSprite(id, 10, xteclado, yteclado);
            C2D_DesenhaSprite(id, fmouse, mouse->x, mouse->y);
            if(C2D_GamepadEstaConectado(0))
            {
                C2D_DesenhaSprite(id, fpad, xpad, ypad);
                C2D_DesenhaSprite(id, fstick, xlstick, ylstick);
            }
            C2D_DesenhaTexto(fonte, 50, 50, "Hello World! Falamos Português em UTF-8!", 0, 255, 255, 255, 255);
            C2D_DesenhaTexto(fonteJ, 50, 550, "Chien2Dv3も日本語で書き込みます。", 0, 255, 0, 0, 255);
            char texto[C2D_TAM_TEXTO];
            sprintf(texto, "Frame: %d", frame++);
            C2D_DesenhaTexto(fonte, 50, 500, texto, 0, 128, 128, 255, 255);
            C2D_Sincroniza(60);
        }
        C2D_Encerra();
    }
    else
        printf("Erro ao inicializar a Chien2D\n");
}
