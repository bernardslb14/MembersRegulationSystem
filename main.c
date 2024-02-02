/*
FICHEIRO TEXTO

-> Estrutura de cada linha: Identificador|Data de Nascimento|NIF|Telefone|Data de entrada no clube|Quota|
-> Exemplo: António Santos|29 2 1992|248606060|964751826|1 1 2020|70.25|


NOTAS

-> O identificador de uma determinada linha trata-se de algo que caracteriza um determinado utilizador, pelo que tanto pode ser o nome próprio como um código qualquer (note-se que se trata de algo único pelo que se aparecerem duas ou mais linhas com o mesmo identificador, a partir da 2º serão todas rejeitadas)
-> Considere-se que o clube de montanhismo foi aberto no 1º dia de 2020, pelo que se forem apresentadas datas anteriores a esta aquando do momento de inscrição então essa linha será rejeitada
-> Sobre as quotas, podem ser disponibilizados montantes em double (com casas decimais), podendo o dinheiro que sobra ser utilizado para pagar em anos/meses seguintes
-> Os restantes dias do primeiro mês de entrada no clube são gratuitos. Imaginemos que uma pessoa ingressou na data 1/1/2020, ou 15/1/2020, ou ainda 30/1/2020, então os dias restantes do mês de Janeiro que faltam até Fevereiro não são pagos
-> Considere-se que a quota anual são 60€ para assim termos 5€ por mês
-> Quando pedimos ao utilizador o nome dos ficheiros de saída não é necessário colocar a extensão '.txt', dado que esta é acrescentada depois através da função strcat()


RESUMO DO PROGRAMA

-> Primeiramente inicializa-se todas as listas que vão ser precisas a NULL
-> Leem-se todas as linhas do ficheiro 'config.txt' e aí faz-se a filtragem das mesmas através de mecanismos referente à programação defensiva (os dados das linhas que cumprem todos os requisitos são guardados numa estrutura que depois é acrescentada ao ficheiro binário criado - designado por 'binario.txt')
-> Depois lê-se o ficheiro binário 'binario.txt' e aí sim é que inserimos todos os dados numa lista duplamente ligada que podemos dizer que é a lista mais global do programa dado que contém todos os dados referentes a todas as pessoas corretamente inscritas no clube
-> De seguida faz-se a análise das quotas e separam-se os dois tipos de membros, ou seja, os que têm as quotas em dia que são guardados numa lista em especifico, e os que têm as quotas em atraso são guardados numa outra lista (cada elemento destas listas é apenas composto pelo identificador da pessoa e pelo montante em dívida/em excesso)
-> Por fim, é pedido ao utilizador que insira o nome de dois ficheiros de saída (um que será a listagem dos membros com as quotas em dia e um segundo que será a listagem dos membros com as quotas em atraso) e depois o programa termina
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "variaveis.h"

extern void inicializa_lista();
extern void insere_lista(struct socio **lista, struct socio **aux);
extern void imprime_lista(struct socio **lista);
extern void insere_lista_2(struct nomes **lista, struct nomes **aux);
extern void insere_lista_3(struct quotas **lista, struct quotas **aux);
extern void imprime_lista_2(struct quotas **lista);

#define MAXBUFFER 128
#define MAXNUMERO 6
#define QUOTAS_MES 5

struct socio *lista;
struct nomes *lista_nomes;
struct quotas *em_dia;
struct quotas *em_atraso;

void limpa_string(char string[], int len){
    for(int i=0;i<len;i++){
        string[i]='\0';
    }
}

void escrever_ficheiros(){
    FILE *fp;
    struct quotas *aux;
    char nome_ficheiro[32];
    char extensao[] = ".txt";

    printf("Nome do ficheiro que contém as informações das quotas em dia: ");
    scanf("%s",nome_ficheiro);
    strcat(nome_ficheiro,extensao);
    fp=fopen(nome_ficheiro,"w");
    aux=em_dia;
    while(aux!=NULL){
        fprintf(fp,"%s\t+%.2f€\n",aux->nome,aux->sobras);
        aux=aux->next;
    }
    fclose(fp);
    limpa_string(nome_ficheiro,32);

    printf("Nome do ficheiro que contém as informações das quotas em atraso: ");
    scanf("%s",nome_ficheiro);
    strcat(nome_ficheiro,extensao);
    fp=fopen(nome_ficheiro,"w");
    aux=em_atraso;
    while(aux!=NULL){
        fprintf(fp,"%s\t%.2f€\n",aux->nome,aux->sobras);
        aux=aux->next;
    }
    fclose(fp);
}

void averiguar_quotas(){
    struct socio *aux;
    struct quotas *auxiliar;
    int meses;
    int preco;

    aux=lista;
    while(aux!=NULL){
        meses=12-aux->clube.mes;
        preco=meses*QUOTAS_MES;

        auxiliar = (struct quotas *) malloc(sizeof(struct quotas));
        strcpy(auxiliar->nome,aux->nome);
        auxiliar->sobras=aux->quotas-preco;
        if(preco>aux->quotas){
            insere_lista_3(&em_atraso,&auxiliar);
        }
        else{
            insere_lista_3(&em_dia,&auxiliar);
        }

        aux=aux->next;
    }
}

void ler_binario(){
    FILE *fp;
    struct socio *aux;

    fp=fopen("binario.txt","r");

    while(1){
        aux = (struct socio *) malloc(sizeof(struct socio));
        if(fread(aux, sizeof(struct socio), 1, fp)!=1){
            break;
        }
        insere_lista(&lista,&aux);
    }

    fclose(fp);
}

void escrever_binario(struct socio **aux, int *pvazio){
    FILE *fp;

    if((*pvazio)==0){
        fp=fopen("binario.txt","w");
        *pvazio=1;
    }
    else
        fp=fopen("binario.txt","a");
    fwrite(*aux,sizeof(struct socio),1,fp);
    fclose(fp);
}

int verifica_digitos(char string[], int len){
    int final=1;

    for(int i=0;i<len;i++){
        if(isdigit(string[i])==0){
            final=0;
            break;
        }
    }

    return final;
}

int verifica_data(char* digitos, int aux_dia, int aux_mes, char tipo, int aux){
    int numero=atoi(digitos);

    if(verifica_digitos(digitos,strlen(digitos))==0)
        return -1;
    if(tipo=='d'){
        if(numero>31 || numero<1){
            return 1;
        }
    }
    else if(tipo=='m'){
        if((numero>12) || (numero<1)){
            return 2;
        }
        if(((numero%2==0) && (0<numero<8)) || ((numero%2!=0) && (8<=numero<=12))){
            if(numero>30){
                return 3;
            }
        }
    }
    else if(tipo=='a'){
        if((numero%4==0) && (aux_mes==2)){
            if(aux_dia>29){
                return 5;
            }
        }
        if((numero%4!=0) && (aux_mes==2)){
            if(aux_dia>28){
                return 6;
            }
        }
        if((aux==1) && (numero<2020)){
            return 4;
        }
        if((aux==1) && (numero>2020)){
            return 7;
        }
    }
    return 0;
}

int verifica_identificador(char string[]){
    struct nomes *aux;

    aux=lista_nomes;

    while(aux!=NULL){
        if(strcmp(string,aux->nome)==0){
            return 1;
        }
        aux=aux->next;
    }
    return 0;
}

double verifica_dinheiro(char string[], int len){
    double numero;
    int ind=0, aux=0;
    char unidades[len];
    char decimas[len];

    limpa_string(unidades,len);
    limpa_string(decimas,len);
    for(int i=0;i<len;i++){
        if(string[i]=='.'){
            ind=i;
            unidades[aux]='\0';
            break;
        }
        else{
            unidades[aux]=string[i];
            aux++;
        }
    }
    aux=0;
    if(ind==strlen(string)-1){
        return 0;
    }
    for(int i=ind+1;i<len;i++){
        decimas[aux]=string[i];
        aux++;
    }
    decimas[aux]='\0';

    if(verifica_digitos(decimas,aux)==0 || verifica_digitos(unidades,strlen(unidades))==0){
        return 0;
    }
    else{
        numero=strtof(string,NULL);
        return numero;
    }
}

void ler_ficheiro(){
    FILE *fp;
    char buffer[MAXBUFFER]; //variável onde se armazena uma linha completa
    char *parametro; //variavel para guardar um parametro em especifico de uma linha
    int contador; //contador dos parametros presentes em cada linha
    int linhas=0; //contador das linhas do ficheiro
    char numero[MAXNUMERO]; //variável auxiliar que guarda cada número referente a uma parte de uma data
    int indice, flag, erro, var; //variáveis auxiliares para tratar da parte do armazenamento das datas
    struct socio *aux;
    int pvazio=0; //inteiro a utilizar aquando na escrita no ficheiro binário (verifica se é a primeira vez que estamos a escrever ou não)
    double quotas; //variável para guardar o valor devolvido pelas função 'verifica_dinheiro'
    struct nomes *auxiliar;

    fp=fopen("config.txt","r");
    while(fgets(buffer, MAXBUFFER, fp)) {
        linhas++;
        contador=0;
        parametro=strtok(buffer,"|");
        aux = (struct socio *) malloc(sizeof(struct socio));
        erro=0;
        while(parametro!=NULL){
            contador++;
            switch(contador){
                case 1:
                    if(verifica_identificador(parametro)==1){
                        printf("Linha nº %d rejeitada!\nMotivo: já existe no sistema um utilizador com o mesmo identificador!\n\n",linhas);
                        erro=1;
                        break;
                    }
                    else{
                        strcpy(aux->nome,parametro);

                        auxiliar = (struct nomes *) malloc(sizeof(struct nomes));
                        strcpy(auxiliar->nome,aux->nome);
                        insere_lista_2(&lista_nomes,&auxiliar);
                    }
                    break;
                case 2:
                    indice=0;
                    flag=0;
                    limpa_string(numero,MAXNUMERO);
                    for(int i=0;i<strlen(parametro);i++){
                        if((i==strlen(parametro)-1) || (parametro[i]==' ')){
                            if(i==strlen(parametro)-1){
                                numero[indice]=parametro[i];
                            }
                            if(flag==0){
                                var=verifica_data(numero,0,0,'d',0);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o dia de nascimento.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==1){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o dia de nascimento não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.dia=atoi(numero);
                            }
                            else if(flag==1){
                                var=verifica_data(numero,0,0,'m',0);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o mês de nascimento.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==2){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o mês de nascimento não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==3){
                                    printf("Linha nº %d rejeitada!\nMotivo: incoerência entre o mês e o dia referenciado.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.mes=atoi(numero);
                            }
                            else if(flag==2){
                                var=verifica_data(numero,aux->nascimento.dia,aux->nascimento.mes,'a',0);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o ano de nascimento.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==5){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foi fornecido o mês de fevereiro e um ano bissexto, pelo que o dia fornecido apenas pode ir até 29.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==6){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foi fornecido o mês fevereiro e um ano não bissexto, pelo que o dia dado apenas pode ir até 28.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.ano=atoi(numero);
                            }
                            else{
                                printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foram passados mais parâmetros do que o esperado.\n\n",linhas);
                                erro=1;
                                break;
                            }
                            memset(numero,0,strlen(numero));
                            indice=0;
                            flag++;
                        }
                        else{
                            numero[indice]=parametro[i];
                            indice++;
                        }
                    }
                    if((flag!=3) && (erro==0)){
                        printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foram passados menos parâmetros do que o esperado.\n\n",linhas);
                        erro=1;
                    }
                    break;
                case 3:
                    if(verifica_digitos(parametro,strlen(parametro))==0){
                        printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o NIF.\n\n",linhas);
                        erro=1;
                        break;
                    }
                    if(strlen(parametro)!=9){
                        printf("Linha nº %d rejeitada!\nMotivo: foram dados %d digitos para o NIF, quando o indicado é serem 9.\n\n",linhas,strlen(parametro));
                        erro=1;
                        break;
                    }
                    aux->nif=atoi(parametro);
                    break;
                case 4:
                    if(verifica_digitos(parametro,strlen(parametro))==0){
                        printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o telefone.\n\n",linhas);
                        erro=1;
                        break;
                    }
                    if(strlen(parametro)!=9){
                        printf("Linha nº %d rejeitada!\nMotivo: foram dados %d digitos para o telefone, quando o indicado é serem 9.\n\n",linhas,strlen(parametro));
                        erro=1;
                        break;
                    }
                    aux->telefone=atoi(parametro);
                    break;
                case 5:
                    indice=0;
                    flag=0;
                    limpa_string(numero,MAXNUMERO);
                    for(int i=0;i<strlen(parametro);i++){
                        if((i==strlen(parametro)-1) || (parametro[i]==' ')){
                            if(i==strlen(parametro)-1){
                                numero[indice]=parametro[i];
                            }
                            if(flag==0){
                                var=verifica_data(numero,0,0,'d',0);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o dia de entrada no clube.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==1){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o dia de entrada no clube não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->clube.dia=atoi(numero);
                            }
                            else if(flag==1){
                                var=verifica_data(numero,0,0,'m',0);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o mês de entrada no clube.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==2){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o mês de entrada no clube não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var==3){
                                    printf("Linha nº %d rejeitada!\nMotivo: incoerência entre o mês e o dia referenciado.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->clube.mes=atoi(numero);
                            }
                            else if(flag==2){
                                var=verifica_data(numero,aux->clube.dia,aux->clube.mes,'a',1);
                                if(var==-1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram somente fornecidos digitos para o ano de entrada no clube.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                if(var==4){
                                    printf("Linha nº %d rejeitada!\nMotivo: o clube de montanhismo abriu no 1º dia de 2020, pelo que a data de entrada não é válida.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                if(var==5){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foi fornecido o mês de fevereiro e um ano bissexto, pelo que o dia fornecido apenas pode ir até 29.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                if(var==6){
                                        printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foi fornecido o mês fevereiro e um ano não bissexto, pelo que o dia dado apenas pode ir até 28.\n\n",linhas);
                                        erro=1;
                                        break;
                                }
                                if(var==7){
                                    printf("Linha nº %d rejeitada!\nMotivo: pretende-se averiguar as quotas do clube de montanhismo relativo ao ano de 2020, pelo que inscrições posteriores ao ano anteriormente referido não são contabilizadas.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->clube.ano=atoi(numero);
                            }
                            else{
                                printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foram passados mais parâmetros do que o esperado.\n\n",linhas);
                                erro=1;
                                break;
                            }
                            memset(numero,0,strlen(numero));
                            indice=0;
                            flag++;
                        }
                        else{
                            numero[indice]=parametro[i];
                            indice++;
                        }
                    }
                    if((flag!=3) && (erro==0)){
                        printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foram passados menos parâmetros do que o esperado.\n\n",linhas);
                        erro=1;
                    }
                    break;
                case 6:
                    quotas=verifica_dinheiro(parametro,strlen(parametro));
                    if(quotas==0){
                        printf("Linha nº %d rejeitada!\nMotivo: no montante disponibilizado para as quotas foram passados caracteres inválidos.\n\n",linhas);
                        erro=1;
                        break;
                    }
                    aux->quotas=quotas;
                    break;
                default:
                    break;
            }
            if(erro==1){
                break;
            }
            parametro=strtok(NULL,"|");
        }

        if(erro==0){
            if(contador==7)
                escrever_binario(&aux,&pvazio);
            else
                printf("Linha nº %d rejeitada!\nMotivo: Número incorreto de parâmetros.\n\n",linhas);
        }
        free(aux);
    }
    fclose(fp);
}


int main() {
    inicializa_lista(&lista,&lista_nomes,&em_dia,&em_atraso);
    ler_ficheiro();
    ler_binario();
    //imprime_lista(&lista);

    averiguar_quotas();
    //printf("\nSócios com as quotas em dia:\n");
    //imprime_lista_2(&em_dia);
    //printf("\nSócios com as quotas em atraso:\n");
    //imprime_lista_2(&em_atraso);
    escrever_ficheiros();

    return 0;
}