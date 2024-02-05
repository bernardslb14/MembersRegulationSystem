#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "variaveis.h"

extern void initialize_linked_lists();
extern void insert_member(struct socio **lista, struct socio **aux);
extern void print_default_information(struct socio **lista);
extern void insert_name(struct nomes **lista, struct nomes **aux);
extern void insert_to_payments_lists(struct quotas **lista, struct quotas **aux);
extern void print_results(struct quotas **lista);

#define MAXBUFFER 128
#define MAXNUMERO 6
#define QUOTAS_MES 5
#define YEAR 2020

struct socio *default_list;
struct nomes *names_list;
struct quotas *updated_payments;
struct quotas *outdated_payments;

void clean_string(char *string, int len){
    for(int i=0;i<len;i++){
        string[i]='\0';
    }
}

void write_to_files(){
    FILE *fp;
    struct quotas *aux;
    char nome_ficheiro[32];
    char extensao[] = ".txt";

    printf("Nome do ficheiro que contém as informações das quotas em dia: ");
    scanf("%s",nome_ficheiro);
    strcat(nome_ficheiro,extensao);
    fp=fopen(nome_ficheiro,"w");
    aux=updated_payments;
    while(aux!=NULL){
        fprintf(fp,"%s\t+%.2f€\n",aux->nome,aux->sobras);
        aux=aux->next;
    }
    fclose(fp);
    clean_string(nome_ficheiro, 32);

    printf("Nome do ficheiro que contém as informações das quotas em atraso: ");
    scanf("%s",nome_ficheiro);
    strcat(nome_ficheiro,extensao);
    fp=fopen(nome_ficheiro,"w");
    aux=outdated_payments;
    while(aux!=NULL){
        fprintf(fp,"%s\t%.2f€\n",aux->nome,aux->sobras);
        aux=aux->next;
    }
    fclose(fp);
}

void check_payments(){
    struct socio *aux;
    struct quotas *auxiliar;
    int meses;
    int preco;

    aux = default_list;
    while(aux != NULL){
        meses = 12 - aux->clube.mes;
        preco = meses * QUOTAS_MES;

        auxiliar = (struct quotas *) malloc(sizeof(struct quotas));
        strcpy(auxiliar->nome,aux->nome);
        auxiliar->sobras = aux->quotas-preco;
        if(preco > aux->quotas){
            insert_to_payments_lists(&outdated_payments, &auxiliar);
        }
        else{
            insert_to_payments_lists(&updated_payments, &auxiliar);
        }

        aux = aux->next;
    }
}

void read_binary_file(){
    FILE *fp;
    struct socio *aux;

    fp = fopen("binario.txt","r");

    while(1){
        aux = (struct socio *) malloc(sizeof(struct socio));
        if(fread(aux, sizeof(struct socio), 1, fp)!=1){
            break;
        }
        insert_member(&default_list, &aux);
    }

    fclose(fp);
}

void write_binary_file(struct socio **aux, int *pvazio){
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

int check_digits(char *string, int len){
    int final=1;

    for(int i=0;i<len;i++){
        if(isdigit(string[i])==0){
            final=0;
            break;
        }
    }

    return final;
}

int verify_date(char* digitos, int aux_dia, int aux_mes, char tipo, int aux){
    int numero = atoi(digitos);

    if(check_digits(digitos, (int)strlen(digitos)) == 0)
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
        if(((numero%2==0) && (numero>0 && numero<8)) || ((numero%2!=0) && (numero>=8 && numero <= 12))){
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
        if((aux==1) && (numero<YEAR)){
            return 4;
        }
        if((aux==1) && (numero>YEAR)){
            return 7;
        }
    }
    return 0;
}

int verify_identifier(char *string){
    struct nomes *aux;

    aux=names_list;

    while(aux!=NULL){
        if(strcmp(string,aux->nome)==0){
            return 1;
        }
        aux=aux->next;
    }
    return 0;
}

double check_money(char *string, int len){
    double numero;
    int ind=0, aux=0;
    char unidades[len];
    char decimas[len];

    clean_string(unidades, len);
    clean_string(decimas, len);
    for(int i=0; i<len; i++){
        if(string[i] == '.'){
            ind = i;
            unidades[aux] = '\0';
            break;
        }
        else{
            unidades[aux] = string[i];
            aux++;
        }
    }
    aux=0;
    if(ind == (int)strlen(string)-1){
        return 0;
    }
    for(int i=ind+1; i<len; i++){
        decimas[aux] = string[i];
        aux++;
    }
    decimas[aux] = '\0';

    if(check_digits(decimas, aux) == 0 || check_digits(unidades, (int)strlen(unidades)) == 0){
        return 0;
    }
    else{
        numero = strtof(string,NULL);
        return numero;
    }
}

void read_raw_file(){
    FILE *fp;
    char buffer[MAXBUFFER];
    char *param;
    int count;  //contador dos parametros presentes em cada linha
    int linhas = 0;   //contador das linhas do ficheiro
    char numero[MAXNUMERO]; //variável auxiliar que guarda cada número referente a uma parte de uma data
    int indice, flag, erro, var;    //variáveis auxiliares para tratar da parte do armazenamento das datas
    struct socio *aux;
    int pvazio;   //inteiro a utilizar aquando na escrita no ficheiro binário (verifica se é a primeira vez que estamos a escrever ou não)
    double quotas;  //variável para guardar o valor devolvido pelas função 'check_money'
    struct nomes *auxiliar;

    linhas = 0;
    pvazio = 0;
    fp = fopen("config.txt","r");
    while(fgets(buffer, MAXBUFFER, fp)) {
        linhas++;
        count = 0;
        param = strtok(buffer,"|");
        aux = (struct socio *) malloc(sizeof(struct socio));
        erro=0;
        while(param != NULL){
            count++;
            switch(count){
                case 1:
                    if(verify_identifier(param) == 1){
                        printf("Linha nº %d rejeitada!\nMotivo: já existe no sistema um utilizador com o mesmo identificador!\n\n",linhas);
                        erro=1;
                        break;
                    }
                    else{
                        strcpy(aux->nome,param);

                        auxiliar = (struct nomes *) malloc(sizeof(struct nomes));
                        strcpy(auxiliar->nome,aux->nome);
                        insert_name(&names_list, &auxiliar);
                    }
                    break;
                case 2:
                    indice = 0;
                    flag = 0;
                    clean_string(numero, MAXNUMERO);
                    for(int i=0; i<(int)strlen(param); i++){
                        if((i == (int)strlen(param)-1) || (param[i]==' ')){
                            if(i == (int)strlen(param)-1){
                                numero[indice] = param[i];
                            }
                            if(flag == 0){
                                var = verify_date(numero, 0, 0, 'd', 0);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o dia de nascimento.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 1){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o dia de nascimento não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.dia = atoi(numero);
                            }
                            else if(flag == 1){
                                var= verify_date(numero, 0, 0, 'm', 0);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o mês de nascimento.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 2){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o mês de nascimento não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 3){
                                    printf("Linha nº %d rejeitada!\nMotivo: incoerência entre o mês e o dia referenciado.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.mes = atoi(numero);
                            }
                            else if(flag == 2){
                                var = verify_date(numero, aux->nascimento.dia, aux->nascimento.mes, 'a', 0);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o ano de nascimento.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                else if(var == 5){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foi fornecido o mês de fevereiro e um ano bissexto, pelo que o dia fornecido apenas pode ir até 29.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                else if(var == 6){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foi fornecido o mês fevereiro e um ano não bissexto, pelo que o dia dado apenas pode ir até 28.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->nascimento.ano = atoi(numero);
                            }
                            else{
                                printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foram passados mais parâmetros do que o esperado.\n\n",linhas);
                                erro=1;
                                break;
                            }
                            memset(numero, 0, strlen(numero));
                            indice=0;
                            flag++;
                        }
                        else{
                            numero[indice] = param[i];
                            indice++;
                        }
                    }
                    if((flag!=3) && (erro==0)){
                        printf("Linha nº %d rejeitada!\nMotivo: na data de nascimento foram passados menos parâmetros do que o esperado.\n\n",linhas);
                        erro=1;
                    }
                    break;
                case 3:
                    if(check_digits(param, (int)strlen(param)) == 0){
                        printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o NIF.\n\n",linhas);
                        erro=1;
                        break;
                    }
                    if((int)strlen(param) != 9){
                        printf("Linha nº %d rejeitada!\nMotivo: foram dados %d digitos para o NIF, quando o indicado é serem 9.\n\n",linhas,strlen(param));
                        erro=1;
                        break;
                    }
                    aux->nif = atoi(param);
                    break;
                case 4:
                    if(check_digits(param, (int)strlen(param)) == 0){
                        printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o telefone.\n\n",linhas);
                        erro = 1;
                        break;
                    }
                    if((int)strlen(param) != 9){
                        printf("Linha nº %d rejeitada!\nMotivo: foram dados %d digitos para o telefone, quando o indicado é serem 9.\n\n",linhas,(int)strlen(param));
                        erro = 1;
                        break;
                    }
                    aux->telefone = atoi(param);
                    break;
                case 5:
                    indice = 0;
                    flag = 0;
                    clean_string(numero, MAXNUMERO);
                    for(int i=0; i<(int)strlen(param); i++){
                        if((i == (int)strlen(param)-1) || (param[i]==' ')){
                            if(i == (int)strlen(param)-1){
                                numero[indice] = param[i];
                            }
                            if(flag==0){
                                var = verify_date(numero, 0, 0, 'd', 0);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o dia de entrada no clube.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 1){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o dia de entrada no clube não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->clube.dia = atoi(numero);
                            }
                            else if(flag==1){
                                var = verify_date(numero, 0, 0, 'm', 0);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram fornecidos somente digitos para o mês de entrada no clube.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 2){
                                    printf("Linha nº %d rejeitada!\nMotivo: o número fornecido para o mês de entrada no clube não faz sentido.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                else if(var == 3){
                                    printf("Linha nº %d rejeitada!\nMotivo: incoerência entre o mês e o dia referenciado.\n\n",linhas);
                                    erro=1;
                                    break;
                                }
                                aux->clube.mes = atoi(numero);
                            }
                            else if(flag == 2){
                                var = verify_date(numero, aux->clube.dia, aux->clube.mes, 'a', 1);
                                if(var == -1){
                                    printf("Linha nº %d rejeitada!\nMotivo: não foram somente fornecidos digitos para o ano de entrada no clube.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                if(var == 4){
                                    printf("Linha nº %d rejeitada!\nMotivo: o clube de montanhismo abriu no 1º dia de 2020, pelo que a data de entrada não é válida.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                if(var == 5){
                                    printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foi fornecido o mês de fevereiro e um ano bissexto, pelo que o dia fornecido apenas pode ir até 29.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                if(var == 6){
                                        printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foi fornecido o mês fevereiro e um ano não bissexto, pelo que o dia dado apenas pode ir até 28.\n\n",linhas);
                                        erro = 1;
                                        break;
                                }
                                if(var == 7){
                                    printf("Linha nº %d rejeitada!\nMotivo: pretende-se averiguar as quotas do clube de montanhismo relativo ao ano de 2020, pelo que inscrições posteriores ao ano anteriormente referido não são contabilizadas.\n\n",linhas);
                                    erro = 1;
                                    break;
                                }
                                aux->clube.ano = atoi(numero);
                            }
                            else{
                                printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foram passados mais parâmetros do que o esperado.\n\n",linhas);
                                erro=1;
                                break;
                            }
                            memset(numero, 0, (int)strlen(numero));
                            indice=0;
                            flag++;
                        }
                        else{
                            numero[indice] = param[i];
                            indice++;
                        }
                    }
                    if((flag!=3) && (erro==0)){
                        printf("Linha nº %d rejeitada!\nMotivo: na data de entrada no clube foram passados menos parâmetros do que o esperado.\n\n",linhas);
                        erro=1;
                    }
                    break;
                case 6:
                    quotas = check_money(param, (int)strlen(param));
                    if(quotas == 0){
                        printf("Linha nº %d rejeitada!\nMotivo: no montante disponibilizado para as quotas foram passados caracteres inválidos.\n\n",linhas);
                        erro = 1;
                        break;
                    }
                    aux->quotas = quotas;
                    break;
                default:
                    break;
            }
            if(erro == 1){
                break;
            }
            param = strtok(NULL,"|");
        }

        if(erro == 0){
            if(count == 7)
                write_binary_file(&aux, &pvazio);
            else
                printf("Linha nº %d rejeitada!\nMotivo: Número incorreto de parâmetros.\n\n",linhas);
        }
        free(aux);
    }
    fclose(fp);
}


int main() {
    initialize_linked_lists(&default_list, &names_list, &updated_payments, &outdated_payments);

    read_raw_file();
    read_binary_file();
    //print_default_information(&default_list);

    check_payments();
    //printf("\nSócios com as quotas em dia:\n");
    //print_results(&updated_payments);
    //printf("\nSócios com as quotas em atraso:\n");
    //print_results(&outdated_payments);

    write_to_files();
    return 0;
}