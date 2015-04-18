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

#include <stdio.h>
#include <string.h>
#include <c2d3/chien2d.h>
#include <c2d3/chienaudio.h>

// Variável que indica o renderer do sistema (tela lógica)
extern SDL_Renderer *renderer;
// A largura da tela
int lTela=-1;
// A música sendo tocada no momento, nula no início
unsigned int musica = 0;
// Os volumes dos efeitos e música
unsigned int volMusica = 100;
unsigned int volEfeitos = 100;
// O vetor de efeitos sonoros
CA_Efeito efeitos[CA_MAX_EFEITOS];
// O vetor de músicas
CA_Musica musicas[CA_MAX_MUSICAS];

// Função de uso exclusivo da biblioteca. O programador não deve NUNCA utilizá-la
//
// Data: 07/02/2015

void CA_Inicia()
{
	// Volume no máximo inicialmente
	volMusica = 100;
	volEfeitos = 100;
	// Não está tocando nenhuma música
	musica=0;
    // Zera as estruturas de música e efeitos
	for(int i=0;i<CA_MAX_EFEITOS;i++)
    {
        efeitos[i].usuarios = 0;
        strcpy(efeitos[i].apelido, "");
		efeitos[i].efeito= 0;
		efeitos[0].tempo=0;
    }
	for(int i=0;i<CA_MAX_MUSICAS;i++)
    {
        musicas[i].usuarios = 0;
        strcpy(musicas[i].apelido, "");
		musicas[i].musica = 0;
		musicas[i].tempo=0;
    }
	// Ajusta o volume no máximo (default)
	CA_AjustaVolume(100, 100);
	int w,h;
	SDL_RenderGetLogicalSize(renderer, &w, &h);
	lTela = w;

}

// Função que encerra a Chien Audio
//
// Data: 07/02/2015

void CA_Encerra()
{
    // Encerra a música atual
    if(Mix_PlayingMusic())
    {
        Mix_HaltMusic();
        musica = 0;
    }
    // Pára todos os efeitos sonoros
    Mix_HaltChannel(-1);
    // Apaga todos os efeitos
	for(int i=0;i<CA_MAX_EFEITOS;i++)
        CA_RemoveEfeito(i+1);
    // Apaga as músicas
	for(int i=0;i<CA_MAX_MUSICAS;i++)
        CA_RemoveMusica(i+1);
}

// Função que ajusta o volume dos efeitos e músicas no sistema
//
// Data: 07/02/2015

bool CA_AjustaVolume(const unsigned int musica, const unsigned int efeitos)
{
	// Calcula o valor dos volumes entre 0 e 100
	if(musica > 100)
		volMusica = 100;
	else
		volMusica = musica;
	if(efeitos > 100)
		volEfeitos = 100;
	else
		volEfeitos = efeitos;
	// Ajusta o volume dos canais
	Mix_Volume(-1,(volEfeitos*MIX_MAX_VOLUME)/100);
	Mix_VolumeMusic((volMusica*MIX_MAX_VOLUME)/100);
	return true;
}

// Funções para manipular efeitos sonoros

// Função para carregar um efeito sonoro na memória
//
// Data: 07/02/2015

unsigned int CA_CarregaEfeito(const char *arquivo)
{
    // Verifica se o efeito não existe já
	unsigned int idx =  CA_ProcuraEfeito(arquivo);
    // Se já existe um efeito com o apelido, retorna o índice associado e aumenta
    // o número de usuários do mesmo
    if(idx != 0)
    {
        efeitos[idx-1].usuarios++;
        return idx;
    }

    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
	for(int i=0;i<CA_MAX_EFEITOS;i++)
        // O efeito é vago (não tem audio associado?
		if(efeitos[i].efeito == 0)
        {
            idx=i+1;
            break;
        }

    // Testa se ainda tem espaço na lista
    //
    // ATENÇÃO: não ter espaço na memória não é relacionado a este teste. Apenas
    // testa-se aqui se existe um índice vago na lista. Se não existe, idx==0
    if(idx==0)
	{
		printf("Estourou o limite de efeitos sonoros carregados no sistema. \n");
        // Retorna um índice inválido para indicar que o método falhou
        return 0;
	}

    // Se chegou até aqui, idx contém o identificador correto
    // Calcula a posição no array (sem incrementar 1)
    idx=idx-1;
    // Carrega o efeito propriamente dito
    efeitos[idx].efeito = Mix_LoadWAV(arquivo);
    // Se falhou, volta
    if(efeitos[idx].efeito == 0)
    {
        printf("Erro ao carregar o arquivo %s.\n", arquivo);
        return 0;
    }
    // Indica o apelido
    strcpy(efeitos[idx].apelido, arquivo);
    printf("Carregou o efeito %s.\n", arquivo);
    // Indica que tem um usuário
    efeitos[idx].usuarios++;
    // REtorna o índice real
    return idx+1;
}

// Remove um efeito da memória
//
// Data: 07/02/2015

void CA_RemoveEfeito(const unsigned int id)
{
    // O efeito é válido?
	if(id > CA_MAX_EFEITOS || id == 0)
        return;
    // Só remove efetivamente se tiver apenas um usuário
    if(efeitos[id-1].usuarios > 1)
        efeitos[id-1].usuarios--;
	else
    {
		// Just in case, testa antes de remover
		if(efeitos[id-1].efeito != 0)
			Mix_FreeChunk(efeitos[id-1].efeito);
        efeitos[id-1].efeito = 0;
		// Indica apelido vazio
        strcpy(efeitos[id-1].apelido, "");
		// Zera o número de usuários
		efeitos[id-1].usuarios = 0;
    }
}

// Procura um efeito pelo nome do arquivo original
//
// Data: 07/02/2015

unsigned int CA_ProcuraEfeito(const char *apelido)
{
    // Índice de efeito inválido, caso não encontre
    unsigned int idx=0;
	for(int i=0;i<CA_MAX_EFEITOS;i++)
        if(strcmp(efeitos[i].apelido, apelido)==0)
        {
            // O índice é a posição atual + 1
            idx=i+1;
            // Encerra a busca
            break;
        }
    return idx;
}

// Função para tocar um efeito. -100 é um som à esquerda e 100 à direita. 0 para centro.
// Se funcionar, retorna o canal em que o som está tocando. Caso contrário, retorna 0
//
// Data: 07/02/2015

int CA_TocaEfeito(const unsigned int id, const int posicao)
{
    // O efeito é válido?
	if(id > CA_MAX_EFEITOS || id == 0)
        return 0;
    // Verifica se o efeito existe
    if(efeitos[id-1].efeito != 0)
	{
		int posicaoFinal = posicao;
		// Ajusta o volume, se necessário
		if(posicaoFinal<CA_ESQUERDA)
			posicaoFinal = CA_ESQUERDA;
		else if(posicaoFinal>CA_DIREITA)
			posicaoFinal = CA_DIREITA;
		// Calcula o volume para cada canal. Por default, consideramos que ambos estão no máximo

		int esquerda, direita;
		// 254 é o volume máximo de um canal. Assim, centro indica 127 em CADA canal. Som em um canal só indica 254 apenas em um canal
		esquerda = ((100-posicaoFinal)*254)/200;
		direita = ((100+posicaoFinal)*254)/200;
        int canal = Mix_PlayChannel(-1,efeitos[id-1].efeito,0);
        Mix_SetPanning(canal, esquerda, direita);
		return canal;
	}
    return 0;
}

// Função para tocar um efeito em loop
//
// Data: 07/02/2015
//
int CA_TocaEfeitoLoop(const unsigned int id, const int posicao, const int loop)
{
    // O efeito é válido?
	if(id > CA_MAX_EFEITOS || id == 0)
        return 0;
    // Verifica se o efeito existe
    if(efeitos[id-1].efeito != 0)
	{
		// Ajusta o volume, se necessário
		int posicaoFinal = posicao;
		if(posicaoFinal<CA_ESQUERDA)
			posicaoFinal = CA_ESQUERDA;
		else if(posicaoFinal>CA_DIREITA)
			posicaoFinal = CA_DIREITA;
		// Calcula o volume para cada canal. Por default, consideramos que ambos estão no máximo

		int esquerda, direita;
		// 254 é o volume máximo de um canal. Assim, centro indica 127 em CADA canal. Som em um canal só indica 254 apenas em um canal
		esquerda = ((100-posicaoFinal)*254)/200;
		direita = ((100+posicaoFinal)*254)/200;
        int canal = Mix_PlayChannel(-1,efeitos[id-1].efeito,loop);
        Mix_SetPanning(canal, esquerda, direita);
		return canal;
	}
    return 0;

}


// Função para tocar um efeito posicional baseado na coordenada x na tela

// Se funcionar, retorna o canal em que o som está tocando. Caso contrário, retorna 0
//
// Data: 02/02/2008

int CA_TocaEfeitoTela(const unsigned int id, const int posicao)
{
    // O efeito é válido?
	if(id > CA_MAX_EFEITOS || id == 0)
        return 0;
    // Verifica se o efeito existe
    if(efeitos[id-1].efeito != 0)
	{

		// Ajusta a posição se necessário
		int posicaoFinal = posicao;
		if(posicaoFinal<0)
			posicaoFinal=0;
		else if(posicaoFinal > lTela)
			posicaoFinal = lTela;
		// Calcula o volume para cada canal. Por default, consideramos que ambos estão no máximo
		int esquerda, direita;
		posicaoFinal = (200*posicaoFinal)/lTela - 100;
		// 254 é o volume máximo de um canal. Assim, centro indica 127 em CADA canal. Som em um canal só indica 254 apenas em um canal
		esquerda = ((100-posicaoFinal)*254)/200;
		direita = ((100+posicaoFinal)*254)/200;
        int canal = Mix_PlayChannel(-1,efeitos[id-1].efeito,0);
        Mix_SetPanning(canal, esquerda, direita);
		return canal;
	}
	return 0;
}

// Função para cortar o efeito tocando em um canal
//
// Data: 30/01/2011
//
int CA_CortaEfeito(const int canal, const int tempo)
{
	return Mix_FadeOutChannel(canal, tempo);
}


// Função para pausar a reprodução de efeitos sonoros
//
// Data: 02/02/2008

void CA_PausaEfeitos()
{
	Mix_Pause(-1);
}

// Função para continuar a reprodução de efeitos sonoros (após pausa)
//
// Data: 02/02/2008

void CA_ContinuaEfeitos()
{
	Mix_Resume(-1);
}

// Funções para manipular músicas

// função para carregar um efeito na memória
//
// Data: 02/02/2008

unsigned int CA_CarregaMusica(const char *arquivo)
{
    // Verifica se a música não existe já
	unsigned int idx =  CA_ProcuraMusica(arquivo);
    // Se já existe um efeito com o apelido, retorna o índice associado e aumenta
    // o número de usuários do mesmo
    if(idx != 0)
    {
		musicas[idx-1].usuarios++;
        return idx;
    }

    // Se não existe, procura o primeiro índice vago (idx ainda é igual a 0)
	for(int i=0;i<CA_MAX_MUSICAS;i++)
        // O efeito é vago (não tem audio associado?
		if(musicas[i].musica == 0)
        {
            idx=i+1;
            break;
        }

    // Testa se ainda tem espaço na lista
    //
    // ATENÇÃO: não ter espaço na memória não é relacionado a este teste. Apenas
    // testa-se aqui se existe um índice vago na lista. Se não existe, idx==0
    if(idx==0)
	{
		printf("Estourou o limite de músicas carregadas no sistema. \n");
        // Retorna um índice inválido para indicar que o método falhou
        return 0;
	}

    // Se chegou até aqui, idx contém o identificador correto
    // Calcula a posição no array (sem incrementar 1)
    idx=idx-1;
    // Carrega o efeito propriamente dito
	musicas[idx].musica = Mix_LoadMUS(arquivo);
    // Se falhou, volta
    if(musicas[idx].musica == 0)
    {
        printf("Erro ao carregar o arquivo %s.\n", arquivo);
        return 0;
    }
    // Indica o apelido
    strcpy(musicas[idx].apelido, arquivo);
    printf("Carregou a música %s.\n", arquivo);
    // Indica que tem um usuário
    musicas[idx].usuarios++;
    // REtorna o índice real
    return idx+1;
}

// Remove uma música da memória
//
// Data: 02/02/2008

void CA_RemoveMusica(const unsigned int id)
{
    // O efeito é válido?
	if(id > CA_MAX_MUSICAS || id == 0)
        return;
    // Só remove efetivamente se tiver apenas um usuário
    if(musicas[id-1].usuarios > 1)
        musicas[id-1].usuarios--;
	else
    {
		// Just in case, testa antes de remover
		if(musicas[id-1].musica != 0)
			Mix_FreeMusic(musicas[id-1].musica);
        musicas[id-1].musica = 0;
		// Indica apelido vazio
        strcpy(musicas[id-1].apelido, "");
		// Zera o número de usuários
		musicas[id-1].usuarios = 0;
    }

}

// Procura uma música pelo nome do arquivo original
//
// Data: 02/02/2008

unsigned int CA_ProcuraMusica(const char *apelido)
{
    // Índice de música inválida, caso não encontre
    unsigned int idx=0;
	for(int i=0;i<CA_MAX_MUSICAS;i++)
        if(strcmp(musicas[i].apelido, apelido)==0)
        {
            // O índice é a posição atual + 1
            idx=i+1;
            // Encerra a busca
            break;
        }
    return idx;

}

// Função para tocar uma música
//
// Data: 02/02/2008

bool CA_TocaMusica(const unsigned int id, const int vezes)
{
    // O efeito é válido?
	if(id > CA_MAX_MUSICAS || id == 0)
        return 0;
    // Verifica se a música existe
    if(musicas[id-1].musica != 0)
	{
		// Ajusta as vezes se necessário no caso de loop infinito incorreto
		int vezesFinal = vezes;
		if(vezesFinal < -1)
			vezesFinal=-1;
		if(Mix_PlayMusic(musicas[id-1].musica, vezesFinal))
			return false;
		return true;
	}
    return false;

}

// Função para pausar a reprodução da música
//
// Data: 02/02/2008

void CA_PausaMusica()
{
	Mix_PauseMusic();
}

// Função para continuar a reprodução da música (após pausa)
//
// Data: 02/02/2008

void CA_ContinuaMusica()
{
	Mix_ResumeMusic();
}

// Função para encerrar a reprodução da música (fade)
//
// Data: 02/02/2008

void CA_FadeMusica(const unsigned int tempo)
{
	Mix_FadeOutMusic(tempo);
}
