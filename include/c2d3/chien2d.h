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

#ifndef CHIEN2D_H_INCLUDED
#define CHIEN2D_H_INCLUDED

// Inclui os arquivos de cabeçalho da SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// Caso esteja no Windows ...
#if defined(_MSC_VER)
	#include <windows.h>
#endif

#define C2D_TAM_PATH				2048	// Tamanho do path de um arquivo completo em bytes
#define C2D_MAX_SPRITESET			4096	// Número máximo de sprite sets que podem ser carregados
#define C2D_MAX_FONTES				64		// Número máximo de fontes que podem ser carregadas
#define C2D_MAX_TEXTOS              16      // O número máximo de textos que a lib pode desenhar na tela usando buffering
#define  C2D_TAM_TEXTO              256     // O texto não pode ter mais que 256 bytes. Se tiver mais, trunca.

// enum para os tipos de display (janela ou tela cheia)
typedef enum {C2D_JANELA, C2D_TELA_CHEIA} c2d_tipo_tela_t;

// enum do tipo de flip de sprites
typedef enum {C2D_FLIP_NENHUM, C2D_FLIP_HORIZONTAL, C2D_FLIP_VERTICAL, C2D_FLIP_AMBOS} c2d_flip;

// Constanes para sincronização
typedef enum {C2D_FPS_MINIMO = 1, C2D_FPS_PADRAO = 60, C2D_FPS_MAXIMO = 255} c2d_fps;

typedef enum { C2D_BLEND_NENHUM, C2D_BLEND_ALPHA, C2D_BLEND_ADITIVO, C2D_BLEND_MODULACAO_COR} c2d_blend;

// Constantes de teclado
typedef enum {
    C2D_TCIMA, C2D_TBAIXO, C2D_TDIREITA, C2D_TESQUERDA,
    C2D_TESC, C2D_TF1, C2D_TF2, C2D_TF3, C2D_TF4, C2D_TF5, C2D_TF6, C2D_TF7, C2D_TF8, C2D_TF9, C2D_TF10, C2D_TF11, C2D_TF12,
    C2D_TA, C2D_TB, C2D_TC, C2D_TD, C2D_TE, C2D_TF, C2D_TG, C2D_TH, C2D_TI, C2D_TJ, C2D_TK, C2D_TL, C2D_TM, C2D_TN, C2D_TO,
    C2D_TP, C2D_TQ, C2D_TR, C2D_TS, C2D_TT, C2D_TU, C2D_TV, C2D_TW, C2D_TX, C2D_TY, C2D_TZ,
    C2D_T0, C2D_T1, C2D_T2, C2D_T3, C2D_T4, C2D_T5, C2D_T6, C2D_T7, C2D_T8, C2D_T9,
    C2D_TNUM0, C2D_TNUM1, C2D_TNUM2, C2D_TNUM3, C2D_TNUM4, C2D_TNUM5, C2D_TNUM6, C2D_TNUM7, C2D_TNUM8, C2D_TNUM9,
    C2D_TENTER, C2D_TESPACO, C2D_TLALT, C2D_TRALT, C2D_TLCTRL, C2D_TRCTRL, C2D_TLSHIFT, C2D_TRSHIFT,
    C2D_THOME, C2D_TEND, C2D_TINSERT, C2D_TDELETE, C2D_TPAGEUP, C2D_TPAGEDOWN,
    C2D_TENCERRA, C2D_MAX_TECLAS} c2d_teclas;

// Constantes dos botões do mouse
typedef enum {C2D_MESQUERDO, C2D_MDIREITO, C2D_MMEIO, C2D_MAX_MBOTOES} c2d_botoesmouse;

// Constantes dos botões do joystick
typedef enum {
    C2D_GBOTAO_A, C2D_GBOTAO_B, C2D_GBOTAO_X, C2D_GBOTAO_Y, C2D_GBOTAO_L, C2D_GBOTAO_R, C2D_GBOTAO_START,
    C2D_GBOTAO_LS, C2D_GBOTAO_RS, C2D_GPAD_CIMA, C2D_GPAD_BAIXO,C2D_GPAD_ESQUERDA, C2D_GPAD_DIREITA, C2D_GMAX_BOTOES} c2d_botoesgamepad;

typedef enum { C2D_GAMEPAD0, C2D_GAMEPAD1, C2D_GAMEPAD2, C2D_GAMEPAD3, C2D_GAMEPAD4, C2D_GAMEPAD5, C2D_GAMEPAD6, C2D_GAMEPAD7, C2D_GAMEPAD8, C2D_MAX_GAMEPADS } c2d_gamepads;

typedef enum {C2D_GLEIXOX, C2D_GLEIXOY, C2D_GREIXOX, C2D_GREIXOY, C2D_GLTRIGGER, C2D_GRTRIGGER, C2D_GMAX_EIXOS} c2d_eixosgamepad;

// Constantes do alinhamento de texto
typedef enum {C2D_TEXTO_ESQUERDA, C2D_TEXTO_DIREITA, C2D_TEXTO_CENTRALIZADO} c2d_alinhamento_texto;


//     A estrutura C2D_SpriteSet possui campos relacionados aos sprites no sistema.
//
// Data: 25/01/2015
typedef struct C2D_SpriteSet
{
    // Quantos elementos usam esse sprite set. Elimina quando chega a zero
    unsigned int usuarios;
    // O apelido do sprite set. Único no sistema
    char apelido[C2D_TAM_PATH];
    // O tamanho total da imagem do sprite set
    unsigned int largura, altura;
    // As dimensões dos sprites
    unsigned int spLargura, spAltura;
    // As dimensões da matriz de sprites
    unsigned int matrizX, matrizY;
    // O bounding box do sprite
    unsigned int bbX, bbY, bbLargura, bbAltura;
    // O centro do sprite
    unsigned int xcentro, ycentro;
	// A textura SDL2
	SDL_Texture *textura;
} C2D_SpriteSet;

//     A estrutura C2D_Fonte possui campos relacionados às fontes do sistema.
//
// Data: 25/01/2015
typedef struct C2D_Fonte
{
    // Quantos elementos usam esse azulejo. Elimina quando chega a zero
    int usuarios;
    // O apelido da fonte. Único no sistema
    char apelido[C2D_TAM_PATH];
    // Qual o tamanho quadrado (em pixels) da fonte
    int tamanho;
    // A fonte ttf
    TTF_Font *ttf;
} C2D_Fonte;

//     A estrutura Botao possui campos relacionados a teclas/botoes do sistema de teclado
//
// Data: 14/02/2015
typedef struct C2D_Botao
{
	// Indica se o botão acabou de ser pressionado
	bool pressionou;
	// Indica se o botão está sendo mantido pressionado
	bool pressionando;
	// Indica quando o botão foi liberado
	bool liberou;
} C2D_Botao;

//     A estrutura C2D_Mouse contém as informações do mouse
//
// Data: 14/02/2015
typedef struct C2D_Mouse
{
	// Indica as coordenadas atuais do mouse
	int x, y;
	// Indica os estados dos botões do mouse
	C2D_Botao botoes[C2D_MAX_MBOTOES];
}C2D_Mouse;

//     A estrutura C2D_Gamepad contém as informações de um gamepad estilo XBox
//
// Data: ??/02/2015
typedef struct C2D_Gamepad
{
    bool inicializado;
	SDL_GameController *gamepad;
	unsigned int instanceID;
	C2D_Botao botoes[C2D_GMAX_BOTOES];
	int eixos[C2D_GMAX_EIXOS];
} C2D_Gamepad;

// Estrutura que guarda um texto no buffer de textos. A ideia é um pseudo garbage collector, cada vez que um texto é desenhado, ele
// é armazenado na estrutura no texto que há mais frames não foi desenhado. Para funcionar, a função de sincronização deve,
// obrigatoriamente, atualizar este valor. Para o programador, é transparente, é como se ele estivesse desenhando textos em tempo real.
//
// Data: 13/02/2015
typedef struct C2D_Texto
{
    // indica há quantos frames atrás o texto foi usado. Vai ser zero se foi utizado no frame atual, 1 no frame anterior e assim por diante.
    // Idealmente, a função de sincronização deveria apagar um texto que ficou muito tempo sem ser desenhado, mas por enquanto fica sem porque
    // pode ser paranóia
    unsigned int framesAtras;
    // A funte utilizada para desenhar o texto
    unsigned int idFonte;
    // O texto em si
    char texto[C2D_TAM_TEXTO];
    // A textura do texto
    SDL_Texture *textura;
    // As dimensões do texto
    int largura, altura;
    // As coordenadas de deslocamento (para centralizar, colocar a esquerda, etc
    int xdesl, ydesl;
    // As cores do texto
    uint8_t r, g, b, a;
}C2D_Texto;

// Função que inicia a Chien2D 2
bool C2D_Inicia(const unsigned int largura, const unsigned int altura, const c2d_tipo_tela_t modoVideo, const char *tituloJanela, bool suaviza);
// função que indica as dimensões lógicas da tela
bool C2D_DimensoesTela(unsigned int *largura, unsigned int *altura);
// Função que encerra a Chien2D 2
void C2D_Encerra();
// Função para sincronizar o vídeo a n fps
void C2D_Sincroniza(const Uint8 fps);
// Função para limpar a tela
void C2D_LimpaTela();
// Função para escolher a cor de limpeza da tela
void C2D_TrocaCorLimpezaTela(const Uint8 r, const Uint8 g, const Uint8 b);


// Procura um spriteset pelo nome do arquivo original
unsigned int C2D_ProcuraSpriteSet(const char *apelido);
// função para carregar um sprite set na memória
unsigned int C2D_CarregaSpriteSet(const char *arquivo, const unsigned int largura, const unsigned int altura);

// Troca as propriedades do spriteset (centro para rotação e bounding box). Por default, o centro é normalizado e o bounding box é o sprite inteiro
bool C2D_AlteraPropriedadesDoSprite(const unsigned int id, const unsigned int xcentro, const unsigned int ycentro, const unsigned int bbX, const unsigned int bbY,
                                    const unsigned int bbLargura, const unsigned int bbAltura);

// Altera o alpha do sprite
void C2D_AlteraAlphaDoSprite(const unsigned int id, const Uint8 alpha);
// restaura o alpha do sprite
void C2D_RestauraAlphaDoSprite(const unsigned int id);

// Altera o blend do sprite
void C2D_AlteraBlendDoSprite(const unsigned int id, const int modo);
// restaura o blend original do sprite
void C2D_RestauraBlendDoSprite(const unsigned int id);

// Altera o alpha do sprite
void C2D_AlteraCorDoSprite(const unsigned int id, const Uint8 r, const Uint8 g, const Uint8 b);
// restaura o alpha do sprite
void C2D_RestauraCorDoSprite(const unsigned int id);


// Remove um sprite set da memória
void C2D_RemoveSpriteSet(const unsigned int id);

// Função básica para desenhar um sprite
bool C2D_DesenhaSprite(const unsigned int id, const unsigned int indice, const int x, const int y);

// Função para desenhar um sprite com efeitos básicos
bool C2D_DesenhaSpriteEspecial(const unsigned int id, const unsigned int indice, const int x, const int y, const c2d_flip flip, const double escalax, const double escalay, const double angulo);
/*
// Função para verificar a colisão entre sprites
bool C2D2_ColidiuSprites(const unsigned int id1, const unsigned int indice1, const int x1, const int y1,
					     const unsigned int id2, const unsigned int indice2, const int x2, const int y2);
// Função para verificar a colisão entre quadrados de referência
bool C2D2_ColidiuQuadrados(const int x1b, const int y1b, const unsigned int l1b, const unsigned int a1b,
                           const int x2b, const int y2b, const unsigned int l2b, const unsigned int a2b);
// função para retornar as diemnsões de um sprites
bool C2D2_DimensoesSprite(const unsigned int id, unsigned int *largura, unsigned int *altura);
*/

// Função para recuperar o tempo do sistema
unsigned long C2D_TempoSistema();
// Função de pausa (wrapper)
void C2D_Pausa(const Uint32 pausa);

// Funções comuns de entrada

// Função para recuperar as teclas
const C2D_Botao* C2D_PegaTeclas();
// Função para recuperar o mouse
const C2D_Mouse* C2D_PegaMouse();
// Função para recuperar os gamepads
const C2D_Gamepad* C2D_PegaGamepads();
// Função para recuperar o nome de um gamepad
const char* C2D_PegaNomeGamepad(const unsigned int numGamepad);
// Função para verificar se um gamepad está conectado
bool C2D_GamepadEstaConectado(const unsigned int numGamepad);

// função comuns para carregar uma fonte na memória
unsigned int C2D_CarregaFonte(const char *arquivo, const char *apelido, const unsigned int tamanho);
// Remove uma fonte da memória
void C2D_RemoveFonte(const unsigned int id);
// Procura uma fonte pelo nome do arquivo original
unsigned int C2D_ProcuraFonte(const char *apelido);
// Desenha uma fonte só com uma cor de frente. O texto é armazenado no buffer de textos e, se usado em consecutivos frames, ele é redesenhado automaticamente, assumindo que n"ao existem dois textos iguais com a mesma fonte
// O texto deve ser UTF8!
void C2D_DesenhaTexto(const unsigned int idFonte, const int x, const int y, const char *textoUTF8, const unsigned int alinhamento, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
// Função para pegar as dimensões de um texto
bool C2D_DimensoesTexto(unsigned int idFonte, const char *textoUTF8, int *largura, int *altura);

#endif // CHIEN2D_H_INCLUDED
