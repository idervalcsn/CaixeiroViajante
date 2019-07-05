#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>


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

double deltaSwap(vector<int> &, int, int);     //calcula o delta de determinado movimento swap
void movSwap(vector<int> &, int, int);            //executa um movimento swap

double getCusto(vector<int> &);                      //custo total da solucao
vector<CustoInsercao>
listaDeCustos(vector<int> &, vector<int> &);   //cria uma lista de custos baseada nos candidatos restantes
void printData();

void remover(vector<int> &, int);                 //remover item atraves do valor






int main(int argc, char **argv) {


    srand(time(NULL));
    readData(argc, argv, &dimension, &matrizAdj);
    printData();
    vector<int> solution = constructSolution();

    for (int i = 0; i <= dimension; i++) {
        cout << solution[i] << "\t";

    }
    double custo1 = getCusto(solution);
    cout << endl << "Custo: " << custo1 << endl;
    double delta = deltaSwap(solution,2,6);
    movSwap(solution, 2, 6);

    double custo2 = getCusto(solution);
    for (int i = 0; i <= dimension; i++) {
        cout << solution[i] << "\t";

    }
    cout << endl << "Custo: " << getCusto(solution) << endl;

    cout << "Delta Seboso : " << custo2 - custo1 << endl << "Delta Nao seboso: " << delta << endl;



    return 0;


}

bool comparaCusto(CustoInsercao x, CustoInsercao y) {
    return x.custo < y.custo;
}


vector<int> constructSolution() {


    vector<int> solucao = {1, 1};  //Solucao comeca na cidade 1
    vector<int> candidatos;
    int tamanhoSubtour = 4;
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

void movSwap(vector<int> &v, int no1, int no2){
    int temp = v[no1];
    v[no1] = v[no2];
    v[no2] = temp;

}

double deltaSwap(vector<int> &v, int no1, int no2){
    double** m = matrizAdj; //diminui o tamanho da declaraçao ali embaixo
    double delta = 0;
    if(no1 == no2) return delta;
    else if(no2 == no1 + 1){    //nesse caso, duas arestas sao removidas, e duas adicionadas. A que conecta os nos adjacentes permanece.
        delta = (m[v[no1]][v[no2+1]] + m[v[no2]][v[no1 - 1]]) - (m[v[no1]][v[no1 - 1]] + m[v[no2]][v[no2 + 1]]);
    }
    else {
        delta = (m[v[no1]][v[no2 - 1]] + m[v[no1]][v[no2 + 1]] + m[v[no2]][v[no1 - 1]] + m[v[no2]][v[no1 + 1]])
                - (m[v[no1]][v[no1 - 1]] + m[v[no1]][v[no1 + 1]] + m[v[no2]][v[no2 - 1]] + m[v[no2]][v[no2 + 1]]);
    }

    return delta;
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
