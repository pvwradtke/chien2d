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

#ifndef CHIENAUDIO_H_INCLUDED
#define CHIENAUDIO_H_INCLUDED

#include <SDL2/SDL_mixer.h>

#define CA_TAM_PATH         2048	// Tamanho do path de um arquivo completo em bytes
#define CA_MAX_EFEITOS      512		// Número máximo de efeitos que podem ser carregados
#define CA_MAX_MUSICAS      16		// Número máximo de músicas que podem ser carregadas
#define CA_BUFFER_MIXER     2048	// Tamanho do buffer do mixer
#define CA_DIREITA			100		// Som completamente à direita
#define CA_ESQUERDA         -100	// Som completamente à esquerda
#define CA_CENTRO           0		// Som no centro (ambos canais)

//     A estrutura CA_Efeito possui campos relacionados aos efeitos no sistema.
//
// Data: 07/02/2015
typedef struct CA_Efeito
{
    // Quantos elementos usam esse efeito. Elimina quando chega a zero
    int usuarios;
    // O apelido efeito. Único no sistema
    char apelido[CA_TAM_PATH];
    // O tempo total do efeito
    long tempo;
	// Os dados do efeito lido do disco
    Mix_Chunk *efeito;
} CA_Efeito;

//     A estrutura CA_Efeito possui campos relacionados aos efeitos no sistema.
//
// Data: 07/02/2015
typedef struct CA_Musica
{
    // Quantos elementos usam essa musica. Elimina quando chega a zero
    int usuarios;
    // O apelido efeito. Único no sistema
    char apelido[CA_TAM_PATH];
    // O tempo total da música em milisegundos
    long tempo;
	// Os dados do efeito lido do disco
	Mix_Music *musica;
} CA_Musica;


// Função que ajusta o volume dos efeitos e músicas no sistema
bool CA_AjustaVolume(unsigned int musica, unsigned int efeitos);

// Funções para manipular efeitos sonoros

// função para carregar um efeito na memória
unsigned int CA_CarregaEfeito(const char *arquivo);
// Remove um efeitos da memória
void CA_RemoveEfeito(unsigned int id);
// Procura um efeito pelo nome do arquivo original
unsigned int CA_ProcuraEfeito(const char *apelido);
// Função para tocar um efeito
int CA_TocaEfeito(const unsigned int id, const int posicao);
// Função para tocar um efeito em loop
int CA_TocaEfeitoLoop(const unsigned int id, const int posicao, const int loop);
// Função para tocar um efeito posicional baseado na coordenada x na tela
int CA_TocaEfeitoTela(const unsigned int id, const int posicao);
// Função para cortar o efeito tocando em um canal
int CA_CortaEfeito(const int canal, const int tempo);
// Função para pausar a reprodução de efeitos sonoros
void CA_PausaEfeitos();
// Função para continuar a reprodução de efeitos sonoros (após pausa)
void CA_ContinuaEfeitos();

// Funções para manipular músicas

// função para carregar um efeito na memória
unsigned int CA_CarregaMusica(const char *arquivo);
// Remove uma música da memória
void CA_RemoveMusica(const unsigned int id);
// Procura uma música pelo nome do arquivo original
unsigned int CA_ProcuraMusica(const char *apelido);
// Função para tocar uma música
bool CA_TocaMusica(const unsigned int id, const int vezes);
// Função para pausar a reprodução da música
void CA_PausaMusica();
// Função para continuar a reprodução da música (após pausa)
void CA_ContinuaMusica();
// Função para encerrar a reprodução da música (fade)
void CA_FadeMusica(const unsigned int tempo);


#endif // CHIENAUDIO_H_INCLUDED
