#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <random>


using namespace std;

struct CustoInsercao {
    int noInserido;
    int arestaRemovida;
    double custo;
};

double **matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

bool comparaCusto(CustoInsercao, CustoInsercao); // Funcao de comparacao do sort
vector<int> constructSolution();                   //constroi uma solucao
vector<CustoInsercao>
criaRestrita(double, vector<CustoInsercao> &); //cria uma lista restrita com os "alpha" melhores custos

double deltaSwap(vector<int> &, int, int);     //calcula o delta de determinado movimento swap (troca de nos)
void movSwap(vector<int> &);            //executa um movimento swap
void melhorMov2Opt(vector<int> &);

void mov2Opt(vector<int> &, int, int);

double delta2Opt(vector<int> &, int,
                 int);      //calcula o delta de determinado movimento 2-opt (inverte o subtour que vai de no1 a no2 )
void melhorReinsertion(vector<int> &, int);

void movReinsertion(vector<int> &, int, int, int);          //executa um movimento reinsertion
double deltaReinsertion(vector<int> &, int, int, int);

double getCusto(vector<int> &);                      //custo total da solucao
vector<CustoInsercao>
listaDeCustos(vector<int> &, vector<int> &);   //cria uma lista de custos baseada nos candidatos restantes
void printData();

void rvnd(vector<int> &);
void remover(vector<int> &, int);                 //remover item atraves do valor
vector<int> perturbar(vector<int> &);
int gerarRandom(int, int);





int main(int argc, char **argv) {


    srand(time(NULL));
    readData(argc, argv, &dimension, &matrizAdj);
    printData();
    vector<int> solution = constructSolution();
    double custo1, custo2, delta;

    cout << "Solucao sem mov: " << endl;
    for (int i = 0; i <= dimension; i++) {
        cout << solution[i] << "\t";

    }
    custo1 = getCusto(solution);


    cout << endl << "Solucao com perturbacao: " << endl;
    vector<int> solution2 = perturbar(solution);

    for (int i = 0; i <= dimension; i++) {
        cout << solution2[i] << "\t";

    }


    custo2 = getCusto(solution2);
    delta = custo2 - custo1;
    cout << endl << "Custo 1: " << custo1 << "\t" << "Custo 2: " << custo2 << "\t" << "Delta: " << delta << endl;



    return 0;


}

bool comparaCusto(CustoInsercao x, CustoInsercao y) {
    return x.custo < y.custo;
}


vector<int> constructSolution() {


    vector<int> solucao = {1, 1};  //Solucao comeca na cidade 1
    vector<int> candidatos;
    int tamanhoSubtour = 3;
    //double custoTotal = matrizAdj[1][1];

    for (int i = 2; i <= dimension; i++) { //Populando a lista de candidatos
        int atual = i;
        candidatos.push_back(atual);
    }

    for (int i = 0; i < tamanhoSubtour; i++) { //Gerando subtour inicial

        int j = rand() % candidatos.size();
        solucao.insert(solucao.begin() + 1, candidatos[j]);
        candidatos.erase(candidatos.begin() + j);
    }

    while (!candidatos.empty()) {
        vector<CustoInsercao> custos = listaDeCustos(solucao, candidatos);
        sort(custos.begin(), custos.end(), comparaCusto);    //sorta baseado no custo de cada insercao
        vector<CustoInsercao> listaRestrita = criaRestrita(0.5, custos);
        int indiceAleatorio = rand() % listaRestrita.size();
        CustoInsercao escolhido = listaRestrita[indiceAleatorio];   //cidade escolhida, onde, e o custo

        solucao.insert(solucao.begin() + (escolhido.arestaRemovida + 1), escolhido.noInserido);
        remover(candidatos, escolhido.noInserido);

    }

    return solucao;


}

vector<CustoInsercao> listaDeCustos(vector<int> &lista, vector<int> &candidatos) {

    vector<CustoInsercao> custo((lista.size() - 1) *
                                candidatos.size()); //cria uma lista de custos com o tamanho sendo: quantidade de arestas * candidatos restantes

    for (int i = 0, j = 1, l = 0; i < lista.size() - 1; i++, j++) {
        for (auto k: candidatos) {
            custo[l].custo = matrizAdj[lista[i]][k] + matrizAdj[lista[j]][k] - matrizAdj[lista[i]][lista[j]];
            custo[l].noInserido = k;
            custo[l].arestaRemovida = i;
            l++;
        }
    }

    return custo;
}

vector<CustoInsercao> criaRestrita(double alpha, vector<CustoInsercao> &lista) {
    int size = lista.size();
    double quantos = size * alpha; //quantidade de membros vai estar na lista restrita, baseado na pcentagem
    vector<CustoInsercao> restrita;

    if (quantos < 1) {    //quando quantos < 1, nao existe lista restrita a ser criada.
        return lista;
    }

    for (int i = 0; i < quantos; i++) {
        restrita.push_back(lista[i]);
    }

    return restrita;

}

void remover(vector<int> &v, int item) {   //remove um item de um vetor por valor
    v.erase(remove(v.begin(), v.end(), item), v.end());
}

double getCusto(vector<int> &solucao) {
    double custo = 0.0;
    for (int i = 0, j = 1; i < solucao.size() - 1; i++, j++) {
        custo += matrizAdj[solucao[i]][solucao[j]];
    }
    return custo;
}

void movSwap(vector<int> &v) {
    /*int temp = v[no1];
    v[no1] = v[no2];
    v[no2] = temp;*/
    double menorDelta = 0;
    double delta;
    int x = 0, y = 0; //guarda as coordenadas do melhor swap

    for (int i = 1; i < v.size() -
                        1; i++) {//Ignora o primeiro e ultimo. Por consequencia, o penultimo tbm, pq ja vai ter realizado swap com todos.
        for (int j = i + 1; j < v.size() - 1; j++) {  //os swaps comecam a partir do item subsequente ao item "i"
            delta = deltaSwap(v, i, j);
            if (delta < menorDelta) {
                menorDelta = delta;
                x = i;
                y = j;
            }
        }
    }
    if (x != 0 && y != 0) {
        swap(v[x], v[y]);
    }

}

double deltaSwap(vector<int> &v, int no1, int no2) {
    double **m = matrizAdj; //diminui o tamanho da declaraçao ali embaixo
    double delta = 0;
    if (no1 == no2) return delta;
    else if (no2 == no1 +
                    1) {    //nesse caso, duas arestas sao removidas, e duas adicionadas. A que conecta os nos adjacentes permanece.
        delta = (m[v[no1]][v[no2 + 1]] + m[v[no2]][v[no1 - 1]]) - (m[v[no1]][v[no1 - 1]] + m[v[no2]][v[no2 + 1]]);
    } else {
        delta = (m[v[no1]][v[no2 - 1]] + m[v[no1]][v[no2 + 1]] + m[v[no2]][v[no1 - 1]] + m[v[no2]][v[no1 + 1]])
                - (m[v[no1]][v[no1 - 1]] + m[v[no1]][v[no1 + 1]] + m[v[no2]][v[no2 - 1]] + m[v[no2]][v[no2 + 1]]);
    }

    return delta;
}

void melhorMov2Opt(vector<int> &v) {
    double menorDelta = 0;
    double delta;
    int x = 0, y = 0; //guarda as coordenadas do melhor 2opt

    for (int i = 1; i < v.size() - 2; i++) {//O twopt precisa começar no maximo no antepenultimo elemento,
        for (int j = i + 1; j < v.size() - 1; j++) {  //e terminar no penultimo.
            delta = delta2Opt(v, i, j);
            if (delta < menorDelta) {
                menorDelta = delta;
                x = i;
                y = j;
            }
        }
    }
    if (x != 0 && y != 0) {
        mov2Opt(v, x, y);
    }
}

void mov2Opt(vector<int> &v, int no1, int no2) {
    vector<int> newLista(v.size());
    for (int i = 0; i < no1; i++) {   //Solucao antes da subsequencia a ser invertida
        newLista[i] = v[i];
    }

    for (int i = no1, j = no2; i <= no2; i++, j--) { //Inversao da subsequencia
        newLista[i] = v[j];
    }

    for (int i = no2 + 1; i < v.size(); i++) {   //Solucao depois da subsequencia a ser invertida
        newLista[i] = v[i];
    }
    v.swap(newLista);
}

double delta2Opt(vector<int> &v, int no1, int no2) {
    double **m = matrizAdj; //diminui o tamanho da declaraçao ali embaixo
    double delta = 0;
    if (no1 == no2) return delta;
    else {
        delta = (m[v[no1]][v[no2 + 1]] + m[v[no2]][v[no1 - 1]]) - (m[v[no1]][v[no1 - 1]] + m[v[no2]][v[no2 + 1]]);
    }

    return delta;
}

void melhorReinsertion(vector<int> &v, int qnt) {
    double menorDelta = 0;
    double delta;
    int x = 0, y = 0; //guarda as coordenadas da melhor reinsertion

    if (qnt == 1) {
        for (int i = 1; i < v.size() - 1; i++) {//Reinsertion pode ser de qualquer lugar.
            for (int j = 1; j < v.size() - 1; j++) {  //A qualquer lugar. (menos nas bordas)
                delta = deltaReinsertion(v, i, j, qnt);
                if (delta < menorDelta) {
                    menorDelta = delta;
                    x = i;
                    y = j;
                }
            }
        }
    } else {
        for (int i = 1; i < v.size() - 1; i++) {// or-2-op/or-3-opt precisa comecar no maximo no penultimo antes do size
            for (int j = 1; j < v.size() - (qnt + 1); j++) {  //E terminar no maximo qnt antes de size
                delta = deltaReinsertion(v, i, j, qnt);
                if (delta < menorDelta && abs((i - j)) >= qnt) {
                    menorDelta = delta;
                    x = i;
                    y = j;
                }
            }
        }

    }
    if (x != 0 && y != 0) {
        movReinsertion(v, x, y, qnt);
    }
}

void movReinsertion(vector<int> &v, int no1, int no2, int qnt) {


    for (int i = 1; i <= qnt; i++) {
        int valor = v[no1];
        v.erase(v.begin() + no1);
        v.insert(v.begin() + no2, valor);
    }


}

double deltaReinsertion(vector<int> &v, int no1, int no2, int quantidade) {
    double **m = matrizAdj; //diminui o tamanho da declaraçao ali embaixo
    double delta = 0;

    if (no1 == no2) return delta;

    else if (no1 < no2) {     //reinsercao em um indice maior do vetor
        //delta = (m[v[no1]][v[no2]] + m[v[no1]][v[no2 + 1]] + m[v[no1 - 1]][v[no1 + 1]]) -
        //        (m[v[no1]][v[no1 + 1]] + m[v[no1]][v[no1 - 1]] + m[v[no2]][v[no2 + 1]]);
        delta = (m[v[no1]][v[no2]] + m[v[no1 + (quantidade - 1)]][v[no2 + 1]] + m[v[no1 - 1]][v[no1 + quantidade]]) -
                (m[v[no1]][v[no1 - 1]] + m[v[no1 + (quantidade - 1)]][v[no1 + quantidade]] + m[v[no2]][v[no2 + 1]]);


    } else {                     //reinsercao em um indice menor
        //delta = (m[v[no1]][v[no2]] + m[v[no1]][v[no2 - 1]] + m[v[no1 - 1]][v[no1 + 1]]) -
        //        (m[v[no1]][v[no1 - 1]] + m[v[no1]][v[no1 + 1]] + m[v[no2]][v[no2 - 1]]);
        delta = (m[v[no1]][v[no2]] + m[v[no1 - (quantidade - 1)]][v[no2 - 1]] + m[v[no1 - quantidade]][v[no1 + 1]]) -
                (m[v[no1 - (quantidade - 1)]][v[no1 - quantidade]] + m[v[no1]][v[no1 + 1]] + m[v[no2]][v[no2 - 1]]);

    }


    return delta;
}

void rvnd(vector<int> &v){
    vector<int> copia = v;
    vector<int> listaDeVizinhanca = {0,1,2,3,4}; //Cada numero corresponde a um movimento
    double custoInicial = getCusto(v);
    double custoMovimento;
    while(!listaDeVizinhanca.empty()){
        int x = rand() % listaDeVizinhanca.size();
        switch(listaDeVizinhanca[x]){
            case 0:
                movSwap(copia);
                custoMovimento = getCusto(copia);
                if(custoMovimento < custoInicial){
                    v = copia;      //Movimento melhorou a solucao
                    custoInicial = custoMovimento;
                }
                else{
                    copia = v;      //solucao nao melhorou, volta a estaca zero
                    remover(listaDeVizinhanca, 0); //Remove o movimento da lista de vizinhancça

                }
                break;
            case 1:
                melhorMov2Opt(copia);
                custoMovimento = getCusto(copia);
                if(custoMovimento < custoInicial){
                    v = copia;      //Movimento melhorou a solucao
                    custoInicial = custoMovimento;
                }
                else{
                    copia = v;      //solucao nao melhorou, volta a estaca zero
                    remover(listaDeVizinhanca, 1); //Remove o movimento da lista de vizinhancça
                }
                break;

            case 2:
                melhorReinsertion(copia,1);
                custoMovimento = getCusto(copia);
                if(custoMovimento < custoInicial){
                    v = copia;      //Movimento melhorou a solucao
                    custoInicial = custoMovimento;
                }
                else{
                    copia = v;      //solucao nao melhorou, volta a estaca zero
                    remover(listaDeVizinhanca, 2); //Remove o movimento da lista de vizinhancça
                }
                break;
            case 3:
                melhorReinsertion(copia,2);
                custoMovimento = getCusto(copia);
                if(custoMovimento < custoInicial){
                    v = copia;      //Movimento melhorou a solucao
                    custoInicial = custoMovimento;
                }
                else{
                    copia = v;      //solucao nao melhorou, volta a estaca zero
                    remover(listaDeVizinhanca, 3); //Remove o movimento da lista de vizinhancça
                }
                break;
            case 4:
                melhorReinsertion(copia,3);
                custoMovimento = getCusto(copia);
                if(custoMovimento < custoInicial){
                    v = copia;      //Movimento melhorou a solucao
                    custoInicial = custoMovimento;
                }
                else{
                    copia = v;      //solucao nao melhorou, volta a estaca zero
                    remover(listaDeVizinhanca, 4); //Remove o movimento da lista de vizinhancça
                }
                break;
        }
    }

}

vector<int> perturbar(vector<int> &v){

    vector<int> vPerturbado;
    int maxSize = ceil(static_cast<double>(dimension)/10);        //Tamanho maximo de um subsequencia. Conversao estática de dimension int para double
    int aux = maxSize - 1;                                        //Garante que a sequencia nao exceda o maxsize
    int ini1, fim1, ini2, fim2;                 //as 4 posicoes que delimitam as subsequencias
    ini1 = gerarRandom(1, dimension - (2 * maxSize) );  //A primeira pode comecar ate, no maximo, no indice que esteja ate duas vezes o tamanho maximo da subsequencia de distancia do final.
    fim1 = gerarRandom(ini1+1, ini1 + aux);              //E precisa terminar de tal forma que nao ultrapasse o tamanho maximo.
    cout << "Ini1: " << ini1 << " Fim1: " << fim1 << endl;

    ini2 = gerarRandom(fim1 + 1, dimension - maxSize);   //A segunda subsequencia precisa comecar ao menos 1 depois do fim da primeira.
    fim2 = gerarRandom(ini2 + 1, ini2 + aux);
    cout << "Ini2: " << ini2 << " Fim2: " << fim2 << endl;

    copy(v.begin(),v.begin() + ini1, back_inserter(vPerturbado));   //Copia o vetor original ate o ponto em que a subsequencia a ser trocada comeca
    copy(v.begin() + ini2, v.begin() + (fim2 + 1), back_inserter(vPerturbado));    //Coloca a segunda subsequencia no lugar da primeira.
    if((ini2 - fim1) != 1) copy(v.begin() + (fim1 + 1), v.begin() + ini2, back_inserter(vPerturbado));   //checa se sao adjacentes. Caso contrario, há uma copia dupla.
    copy(v.begin() + ini1, v.begin() + (fim1 + 1), back_inserter(vPerturbado));
    copy(v.begin() + (fim2 + 1), v.end(), back_inserter(vPerturbado));
    return vPerturbado;

}

int gerarRandom(int x, int y)
{
    int num;
    std::random_device rd; // obtem um aleatorio do hardware
    std::mt19937 seed(rd()); // seeda o gerador
    std::uniform_int_distribution<> gera(x, y); // define o range


    num = gera(seed);
    return num;
}

void printData() {
    cout << "dimension: " << dimension << endl;
    for (size_t i = 1; i <= dimension; i++) {
        for (size_t j = 1; j <= dimension; j++) {
            cout << matrizAdj[i][j] << " ";
        }
        cout << endl;
    }
}

