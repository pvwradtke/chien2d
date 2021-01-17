#include <stdio.h>
#include <math.h>
#include <c2d3/chien2d.h>
#include <c2d3/chienaudio.h>

#define GRAU    0.017453293

int main(int argc, char **argv)
{
    if(C2D_Inicia(1920, 1080, C2D_JANELA, "Teste da lib", true))
    {
        const C2D_Botao *teclado = C2D_PegaTeclas();
        const C2D_Gamepad *gamepads = C2D_PegaGamepads();
        const C2D_Mouse *mouse = C2D_PegaMouse();
        unsigned int fundo = C2D_CarregaSpriteSet("resources/Sword.Art.Online.full.124015.png", 0, 0);
        if(fundo==0)
            printf("Erro ao carregar o sprite\n");
        bool sai = false;
        int frame=0;
        float angulos[1080];
        int angulo=0;
        memset(angulos, 0, sizeof(int)*1080);
        while(!sai)
        {
            C2D_LimpaTela();
            if(teclado[C2D_TENCERRA].pressionou)
                sai=true;
            if(teclado[C2D_TESC].pressionou)
                sai=true;
            if(C2D_GamepadEstaConectado(0))
                if(gamepads[0].botoes[C2D_GBOTAO_B].pressionou)
                    sai = true;

            for(int i=0;i<1080;i++)
            {
                int desloca = (int)(sin(angulos[1079-i]*GRAU)*60);
                //int desloca = (int)(sin((float)((angulo+i)%360)*GRAU)*20);
                if(desloca>=0)
                {
                    int largura=1920-desloca;
                    C2D_DesenhaParteSprite(fundo, 0, desloca, i, 0, i, largura, 1);
                    C2D_DesenhaParteSprite(fundo, 0, 0, i, 1920-desloca, i, desloca, 1);
                }
                else
                {
                    int largura=1920+desloca;
                    C2D_DesenhaParteSprite(fundo, 0, 0, i, -desloca, i, largura, 1);
                    C2D_DesenhaParteSprite(fundo, 0, largura, i, 0, i, -desloca, 1);

                }
            }
            angulo++;
            for(int i=1079;i>0;i--)
                angulos[i]=angulos[i-1];
            angulos[0]+=0.6;
            if(angulos[0]>360)
                angulos[0]-=360;
            //C2D_DesenhaParteSprite(fundo, 0, 0, 0, 100, 100, 300, 300);
            C2D_Sincroniza(60);
        }
        C2D_Encerra();
    }
    else
        printf("Erro ao inicializar a Chien2D - saindo!\n");
}
