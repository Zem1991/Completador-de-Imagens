/*============================================================================*/
/* Trabalho final - Completador de imagens									  */
/*----------------------------------------------------------------------------*/
/* Autores:		Felipe Lopes Zem, Klaus Mettegang Diener					  */
/* Universidade Tecnológica Federal do Paraná                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
#include "pdi.h"

#define INPUT_IMAGEM_ORIGINAL "IMAGEM ORIGINAL.bmp"
#define INPUT_IMAGEM_BINARIZADA "IMAGEM BINARIZADA.bmp"

void borraPraDentro();
void geraTextura();
int contaPixelsBinarizados(Imagem* img_bi);
int borraPraDentro_definePixel(Imagem* img_in, Imagem* img_bi, Imagem* img_out, int row, int col);
int geraTextura_definePixel(Imagem* img_in, Imagem* img_bi, Imagem* img_out, int row, int col);
float geraTextura_comparaVizinhanca(Imagem* img_in, Imagem* img_bi, int rowCurrent, int colCurrent, int rowCandidate, int colCandidate);
float geraTextura_scoreUmPixel(Imagem* img_in, int rowCurrent, int colCurrent, int rowCandidate, int colCandidate);

int main() {
	/*
		VERSÃO QUE PREENCHE COM PIXELS DE VIZINHANÇA SIMILAR (vulgo, gera texturas).
	1.	Carrega duas imagens: img_original e img_binarizada (branco = aonde tem que completar).
	2.	Prepara uma outra imagem, img_final, para ir preenchendo com os valores finais.
	3.	Prepara mais uma imagem, img_bina_aux, para marcar quais pixels já foram trabalhados ao final de cada iteração.
	4.	Quantos pixels de img_binarizada são brancos = substituições a efetuar ao todo.
	5.	Itera enquanto houverem substituições a efetuar:
	5.1.	Percorre cada pixel de img_original:
	5.1.1.	Para cada pixel preto de img_binarizada faz:
	5.1.1.1.	Aplica a mesma cor encontrada em img_original na respecitva posição de img_final.
	5.1.1.2.	Só isso mesmo.
	5.1.2.	Para cada pixel branco de img_binarizada com ao menos um pixel vizinho preto faz:
	5.1.2.1.	Itera DE NOVO toda a img_original:
	5.1.2.1.1.	Procura um pixel preto com vizinhos similares aos do pixel branco, e:
	5.1.2.1.1.A.	Se houver um pixel preto com vizinhos bons o suficiente, usa-se o mesmo valor desse pixel preto.
	5.1.2.1.1.B.	Senão, gera um valor matemágico para este pixel branco.
	5.1.2.2.	Aplica a cor definida na respecitva posição de img_final.
	5.1.2.3.	Decrementa o contador de substituições.
	5.1.2.4.	Marca com preto em img_bina_aux.
	5.1.3.	Contnua percorrendo até não terem mais pixels brancos em img_binarizada que possam ser tratados.
	5.2.	Copia img_bina_aux sobre img_binarizada, para ignorar nas proximas iterações os pixels já tratados.
	5.3.	Copia img_final sobre img_original, para que futuras iterações façam uso dos pixels gerados.
	6.	???
	7.	PROFIT.
	*/

	Imagem* img_original = abreImagem(INPUT_IMAGEM_ORIGINAL, 3);
	Imagem* img_binarizada = abreImagem(INPUT_IMAGEM_BINARIZADA, 1);
	Imagem* img_final = criaImagem(img_original->largura, img_original->altura, img_original->n_canais);
	Imagem* img_bina_aux = criaImagem(img_binarizada->largura, img_binarizada->altura, img_binarizada->n_canais);
	copiaConteudo(img_binarizada, img_bina_aux);

	int channel, row = 0, col = 0;
	int substituicoes = contaPixelsBinarizados(img_binarizada);

	int iteracaoAtual = 1;

	while (substituicoes > 0) {
		printf("ITERAÇÃO %d - substituicoes remanescentes = %d\n", iteracaoAtual, substituicoes);
		for (row = 0; row < img_original->altura; row++) {
			for (col = 0; col < img_original->largura; col++) {
				if (geraTextura_definePixel(img_original, img_binarizada, img_final, row, col) == 1) {
					substituicoes--;
					img_bina_aux->dados[0][row][col] = -1;
				}
			}
		}
		//copiaConteudo(img_final, img_original);
		copiaConteudo(img_bina_aux, img_binarizada);
		
		if (iteracaoAtual == 1) salvaImagem(img_final, "IMAGEM COMPLETADA 01.bmp");
		if (iteracaoAtual == 2) salvaImagem(img_final, "IMAGEM COMPLETADA 02.bmp");
		if (iteracaoAtual == 3) salvaImagem(img_final, "IMAGEM COMPLETADA 03.bmp");
		if (iteracaoAtual == 4) salvaImagem(img_final, "IMAGEM COMPLETADA 04.bmp");
		if (iteracaoAtual == 5) salvaImagem(img_final, "IMAGEM COMPLETADA 05.bmp");
		if (iteracaoAtual == 6) salvaImagem(img_final, "IMAGEM COMPLETADA 06.bmp");
		if (iteracaoAtual == 7) salvaImagem(img_final, "IMAGEM COMPLETADA 07.bmp");
		if (iteracaoAtual == 8) salvaImagem(img_final, "IMAGEM COMPLETADA 08.bmp");
		if (iteracaoAtual == 9) salvaImagem(img_final, "IMAGEM COMPLETADA 09.bmp");
		if (iteracaoAtual == 10) salvaImagem(img_final, "IMAGEM COMPLETADA 10.bmp");
		if (iteracaoAtual == 11) salvaImagem(img_final, "IMAGEM COMPLETADA 11.bmp");

		iteracaoAtual++;
	}

	salvaImagem(img_final, "IMAGEM COMPLETADA FINAL.bmp");

	system("pause");
	//getChar();
}

void borraPraDentro() {
	/*
	VERSÃO QUE PREENCHE COM AS CORES DAS BORDAS (vulgo, borra pra dentro).
	1.	Carrega duas imagens: img_original e img_binarizada (branco = aonde tem que completar).
	2.	Prepara uma outra imagem, img_final, para ir preenchendo com os valores finais.
	3.	Prepara mais uma imagem, img_bina_aux, para marcar quais pixels já foram trabalhados ao final de cada iteração.
	4.	Quantos pixels de img_binarizada são brancos = substituições a efetuar ao todo.
	5.	Itera enquanto houverem substituições a efetuar:
	5.1.	Percorre cada pixel de img_original:
	5.1.1.	Para cada pixel preto de img_binarizada faz:
	5.1.1.1.	Aplica a mesma cor encontrada em img_original na respecitva posição de img_final.
	5.1.1.2.	Só isso mesmo.
	5.1.2.	Para cada pixel branco de img_binarizada com ao menos um pixel vizinho preto faz:
	5.1.2.1.	Pega os valores dos pixels adjacentes em img_original que também são pretos na img_binarizada.
	5.1.2.2.	Matemágicas para definir qual será a cor a ser utilizada.
	5.1.2.3.	Aplica a cor definida na respecitva posição de img_final.
	5.1.2.4.	Decrementa o contador de substituições.
	5.1.2.5.	Marca com preto em img_bina_aux.
	5.1.3.	Contnua percorrendo até não terem mais pixels brancos em img_binarizada que possam ser tratados.
	5.2.	Copia img_bina_aux sobre img_binarizada, para ignorar nas proximas iterações os pixels já tratados.
	5.3.	Copia img_final sobre img_original, para que futuras iterações façam uso dos pixels gerados.
	6.	???
	7.	PROFIT.
	*/

	Imagem* img_original = abreImagem(INPUT_IMAGEM_ORIGINAL, 3);
	Imagem* img_binarizada = abreImagem(INPUT_IMAGEM_BINARIZADA, 1);
	Imagem* img_final = criaImagem(img_original->largura, img_original->altura, img_original->n_canais);
	Imagem* img_bina_aux = criaImagem(img_binarizada->largura, img_binarizada->altura, img_binarizada->n_canais);
	copiaConteudo(img_binarizada, img_bina_aux);

	int channel, row = 0, col = 0;
	int substituicoes = contaPixelsBinarizados(img_binarizada);

	int iteracaoAtual = 1;

	while (substituicoes > 0) {
		printf("ITERAÇÃO %d - substituicoes remanescentes = %d\n", iteracaoAtual, substituicoes);
		for (row = 0; row < img_original->altura; row++) {
			for (col = 0; col < img_original->largura; col++) {
				if (borraPraDentro_definePixel(img_original, img_binarizada, img_final, row, col) == 1) {
					substituicoes--;
					img_bina_aux->dados[0][row][col] = -1;
				}
			}
		}
		copiaConteudo(img_final, img_original);
		copiaConteudo(img_bina_aux, img_binarizada);
		iteracaoAtual++;
	}

	salvaImagem(img_final, "IMAGEM COMPLETADA.bmp");
}

void geraTextura() {
	//INSERIR AQUI O CODEGO DO METODO QUE GERA TEXTURAS QUANDO TERMINAR DE IMPLEMENTAR ELE
	//AI PODEMOS FAZER UM "MENU" PRA ESCOLHER QUAL DOS METODOS EXECUTAR
}

int contaPixelsBinarizados(Imagem* img_bi) {
	int row, col;
	int count = 0;
	for (row = 0; row < img_bi->altura; row++) {
		for (col = 0; col < img_bi->largura; col++) {
			if (img_bi->dados[0][row][col] == 1)
				count++;
		}
	}
	return count;
}

//	Retorna 1 se houve substituição do píxel, conforme a imagem binarizada.
//	Retorna 0 caso contrário.
int borraPraDentro_definePixel(Imagem* img_in, Imagem* img_bi, Imagem* img_out, int row, int col) {
	int channel;
	if (img_bi->dados[0][row][col] == 0) {
		//Se a posição atual já tiver uma cor em img_in, então é só repetir o valor desta em img_out.
		for (channel = 0; channel < img_in->n_canais; channel++) {
			img_out->dados[channel][row][col] = img_in->dados[channel][row][col];
		}
		return 0;
	}
	else if (img_bi->dados[0][row][col] == -1) {
		//Se entrou aqui é porque esta posição já foi trabalhada em uma iteração anterior.
		return 0;
	}
	else {
		//Senão uma cor é gerada para a posição atual (desde que isso seja possível de se fazer neste momento).
		int rowAux, colAux;
		for (channel = 0; channel < img_in->n_canais; channel++) {
			float count = 0;
			float value = 0;
			for (rowAux = row - 1; rowAux <= row + 1; rowAux++) {
				for (colAux = col - 1; colAux <= col + 1; colAux++) {
					//VIZINHANÇA-8
					if (img_bi->dados[0][rowAux][colAux] != 1) {
						//VIZINHANÇA-4
						//if (img_bi->dados[0][rowAux][colAux] != 1 && (row == rowAux || col == colAux)) {
							//OBS:	cada pixel da vizinhança está sendo somado com o mesmo peso dos demais!
						value = value + img_in->dados[channel][rowAux][colAux];
						count++;
					}
				}
			}
			if (count != 0) {
				printf("VALUE = %f, COUNT = %f, DIVISAO = %f\n", value, count, (value / count));
				value = value / count;		//OBS:	ver OBS anterior!
				img_out->dados[channel][row][col] = value;
			}
			else {
				//Não existiu nenhum pixel vizinho preto. Logo nada pode ser feito neste momento.
				//E nem precisa percorrer os demais canais para confirmar isto.
				return 0;
			}
		}
		//Existiu ao menos um pixel vizinho preto relativo a posição atual em img_bi.
		//O valor dele e dos demais vizinhos pretos, nas respectivas posições de img_in,
		//foram computados e aplicados em img_out.
		return 1;
	}
}

//	Retorna 1 se houve substituição do píxel, conforme a imagem binarizada.
//	Retorna 0 caso contrário.
int geraTextura_definePixel(Imagem* img_in, Imagem* img_bi, Imagem* img_out, int row, int col) {
	int channel = 0;
	if (img_bi->dados[0][row][col] == 0) {
		//Se a posição atual já tiver uma cor em img_in, então é só repetir o valor desta em img_out.
		for (channel; channel < img_in->n_canais; channel++) {
			img_out->dados[channel][row][col] = img_in->dados[channel][row][col];
		}
		return 0;
	}
	else if (img_bi->dados[0][row][col] == -1) {
		//Se entrou aqui é porque esta posição já foi trabalhada em uma iteração anterior.
		return 0;
	}
	else {
		//Se entrou aqui então finalmente encontramos um pixel que deve ser substituido.
		//Percorremos novamente img_original, dessa vez para procurar o pixel que melhor se encaixe nesta posição.
		int rowCandidate, colCandidate;
		float menorScore = 666, scoreAtual = 0;
		int rowScore = -1, colScore = -1;
		for (rowCandidate = 0; rowCandidate < img_in->altura; rowCandidate++) {
			for (colCandidate = 0; colCandidate < img_in->largura; colCandidate++) {
				scoreAtual = geraTextura_comparaVizinhanca(img_in, img_bi, row, col, rowCandidate, colCandidate);
				if (scoreAtual < menorScore && scoreAtual >= 0) {
					menorScore = scoreAtual;
					rowScore = rowCandidate;
					colScore = colCandidate;
				}
			}
		}

		if (rowScore == -1 || colScore == -1) {
			//Se entrar aqui então não haviam vizinhos válidos NO MOMENTO em que isto executou.
			return 0;
		}
		else {
			for (channel; channel < img_in->n_canais; channel++) {
				img_out->dados[channel][row][col] = img_in->dados[channel][rowScore][colScore];
			}
			/*printf("UTILIZANDO A COR DO PIXEL DA POSIÇÃO: x=%d , y=%d \n", colScore, rowScore);
			printf("APLICANDO A MESMA NA POSIÇÃO: x=%d , y=%d \n", col, row);
			printf("COR: R=%f , G=%f , B=%f \n",
			img_in->dados[0][rowScore][colScore] * 255,
			img_in->dados[1][rowScore][colScore] * 255,
			img_in->dados[2][rowScore][colScore] * 255);
			printf("SCORE ATUAL: menor score encontrado = %d \n", menorScore);*/
			return 1;
		}
	}
}

//Retorna 0 ou maior (vulgo valor do score de toda a vizinhança) se a posição sendo avaliada pode ser utilizada na substituição.
//Retorna -1 caso contrário.
float geraTextura_comparaVizinhanca(Imagem* img_in, Imagem* img_bi, int rowCurrent, int colCurrent, int rowCandidate, int colCandidate) {
	//Antes de mais nada, a posição sendo avaliada deve constar como preto na imagem binarizada, indicando que é um valor válido para a substituição.
	if (img_bi->dados[0][rowCandidate][colCandidate] == 0) {
		//Percorremos a vizinahnça-8 de cada pixel (o que está sendo substituido e o que está sendo avaliado).
		//Nos baseamos em todos os vizinhos que estão pretos na imagem binarizada.
		int rowMod, colMod;		//Usados para a verificação da vizinhança-8 (valores logo abaixo e logo acima das posições).
		int channel;			//Cada canal da imagem original.
		int teveVizinho = 0;			//Maior que zero indica que haviam vizinhos válidos para a posição atual.
		float score = 0;
		for (rowMod = -1; rowMod <= 1; rowMod++) {
			for (colMod = -1; colMod <= 1; colMod++) {
				if ((((rowCandidate + rowMod) >= 0 && (rowCandidate + rowMod) < img_in->altura)) &&
					(((colCandidate + colMod) >= 0 && (colCandidate + colMod) < img_in->largura))) {
					//Para cada vizinho preto válido do pixel a ser substituido, compara as vizinhanças dele com as do pixel sendo avaliado.
					if (img_bi->dados[0][rowCurrent + rowMod][colCurrent + colMod] == 0) {
						//countVizinhosSobrando++;
						teveVizinho++;
						//countCanaisIdenticos = 0;
						//Obtém o valor do score.
						score += geraTextura_scoreUmPixel(img_in, rowCurrent + rowMod, colCurrent + colMod, rowCandidate + rowMod, colCandidate + colMod);
						/*for (channel = 0; channel < img_in->n_canais; channel++) {
						if (img_in->dados[channel][rowCurrent + rowMod][colCurrent + colMod] == img_in->dados[channel][rowCandidate + rowMod][colCandidate + colMod]) {
						countCanaisIdenticos++;
						}
						}
						if (countCanaisIdenticos == img_in->n_canais) {
						countVizinhosSobrando--;
						}*/
					}
				}
			}
		}
		//O score de todos os vizinhos válidos do pixel avaliado devem ter sido comparados e somados.
		//Se for o caso, então esta posição pode ser usada como o novo valor do pixel.
		if (teveVizinho > 0) {
			return score;
		}
		//Mas se não houveram vizinhos válidos, então retorna -1 indicando que nada pode ser feito agora.
		else {
			return -1;
		}
	}
	//Se a posição é branca ou se é uma cor que foi gerada numa iteração anterior, nada acontece.
	else {
		return -1;
	}
}

float geraTextura_scoreUmPixel(Imagem* img_in, int rowCurrent, int colCurrent, int rowCandidate, int colCandidate) {
	float score = 0;
	float scoreAux = 0;
	int channel = 0;
	for (channel; channel < img_in->n_canais; channel++) {
		scoreAux = img_in->dados[channel][rowCurrent][colCurrent] - img_in->dados[channel][rowCandidate][colCandidate];
		if (scoreAux < 0) scoreAux *= -1;
		score += scoreAux;
	}
	return score;
}


/*
		//Senão uma cor é copiada para a posição atual (desde que isso seja possível de se fazer neste momento).
		//E isso é feito percorrendo novamente toda a imagem, procurando pelo melhor candidato para cópia.
		int rowImg, colImg;		//O pixel que está sendo verificado neste momento para a comparação de vizinhança.
		int rowMod, colMod;		//Determina qual vizinho está sendo verificado (tanto o pra substituir quanto o verificado).
		int countAux = 0;		//Número de vizinhos idênticos para a posição verificada neste momento.
		int rowSubs = -1, colSubs = -1;		//As coordenadas do pixel identificado como o melhor candidato até agora.
		int count = 0;						//Número de vizinhos idênticos encontrados até agora.
		int channel = 0;

		//"count < 8" embutido no for abaixo
		//TODO remover esta gambiarra (ou não)
		for (rowImg = 0; rowImg < img_in->altura && count < 8; rowImg++) {
			for (colImg = 0; colImg < img_in->largura; colImg++) {
				for (channel = 0; channel < img_in->n_canais; channel++) {
					rowMod = -1;
					colMod = -1;
					int rowAux, colAux;
					for (rowAux = rowImg + rowMod; rowAux <= rowImg + 1; rowAux++) {
						for (colAux = colImg + colMod; colAux <= colImg + 1; colAux++) {
							if ((rowAux >= 0 && rowAux <= img_in->altura) && (colAux >= 0 && colAux <= img_in->largura)) {
								//VIZINHANÇA-8
								if (img_bi->dados[0][rowAux][colAux] != 1) {
									//VIZINHANÇA-4
									//if (img_bi->dados[0][rowAux][colAux] != 1 && (row == rowAux || col == colAux)) {
									//Não deve verificar a própria posição!
									if (row != rowAux && col != colAux) {
										if (img_in->dados[channel][rowAux][colAux] == img_in->dados[channel][row + rowMod][col + colMod]) {
											countAux++;
											if (countAux > count) {
												count = countAux;
												rowSubs = rowImg;
												colSubs = colImg;
											}
										}
										else {
											//Se entrar aqui, é porque não encontrou um valor exato para o vizinho.
											return 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (count != 0) {
			img_out->dados[channel][row][col] = img_out->dados[channel][rowSubs][colSubs];
			return 1;
		}
		else {
			//Não encontrou nenhum pixel candidato a ser copiado para a posição atual.
			return 0;
		}
	}
}


//GRADIENTE EM ESCALA DE CINZA
//img_out->dados[channel][row][col] = iteracaoAtual * 0.025F;

//CAMADAS DE CORES
//img_out->dados[channel][row][col] = (iteracaoAtual%(channel+1)) == 0 ? iteracaoAtual * 0.05F : 0;

//CORES DIFERENTES PARA CADA NUMERO DE VIZINHOS
/*switch (count) {
case 1:
img_out->dados[0][row][col] = 1;
img_out->dados[1][row][col] = 0;
img_out->dados[2][row][col] = 0;
break;
case 2:
img_out->dados[0][row][col] = 1;
img_out->dados[1][row][col] = 1;
img_out->dados[2][row][col] = 0;
break;
case 3:
img_out->dados[0][row][col] = 0;
img_out->dados[1][row][col] = 1;
img_out->dados[2][row][col] = 0;
break;
case 4:
img_out->dados[0][row][col] = 0;
img_out->dados[1][row][col] = 1;
img_out->dados[2][row][col] = 1;
break;
case 5:
img_out->dados[0][row][col] = 0;
img_out->dados[1][row][col] = 0;
img_out->dados[2][row][col] = 1;
break;
case 6:
img_out->dados[0][row][col] = 1;
img_out->dados[1][row][col] = 0;
img_out->dados[2][row][col] = 1;
break;
case 7:
img_out->dados[0][row][col] = 0.3F;
img_out->dados[1][row][col] = 0.3F;
img_out->dados[2][row][col] = 0.3F;
break;
case 8:
img_out->dados[0][row][col] = 0.6F;
img_out->dados[1][row][col] = 0.6F;
img_out->dados[2][row][col] = 0.6F;
break;
case 9:
img_out->dados[0][row][col] = 1;
img_out->dados[1][row][col] = 1;
img_out->dados[2][row][col] = 1;
break;
}*/