#include <stdio.h>
#include <string.h>
#include "variaveis.h"


void initialize_linked_lists(struct socio **list1, struct nomes **list2, struct quotas **list3, struct quotas **list4){
    *list1 = NULL;
    *list2 = NULL;
    *list3 = NULL;
    *list4 = NULL;
}

void print_default_information(struct socio **list){
    struct socio *aux;

    aux = *list;
    while(aux != NULL){
        printf("Nome: %s\nData de Nascimento: %d/%d/%d\nNIF: %d\nTelefone: %d\nData de entrada no clube: %d/%d/%d\nMontante para as quotas: %.2lf€\n\n",aux->nome,aux->nascimento.dia,aux->nascimento.mes,aux->nascimento.ano,aux->nif,aux->telefone,aux->clube.dia,aux->clube.mes,aux->clube.ano,aux->quotas);
        aux=aux->next;
    }
}
void print_results(struct quotas **list){
    struct quotas *aux;

    aux = *list;
    while(aux != NULL){
        printf("Nome: %s\nDividendos: %.2f\n",aux->nome,aux->sobras);
        aux=aux->next;
    }
}

void insert_member(struct socio **lista, struct socio **aux){
    struct socio *anterior, *prox;

    if((*lista == NULL) || (strcmp((*aux)->nome,(*lista)->nome)<0)){ //inserir quando a lista está vazia ou inserir no inicio
        (*aux)->next = *lista;
        if(*lista!=NULL){
            (*lista)->previous=*aux; //new
        }
        (*aux)->previous = NULL; //new
        *lista = *aux;
    }
    else{
        anterior=*lista;
        prox=(*lista)->next;
        while((prox!=NULL) && (strcmp(prox->nome ,(*aux)->nome)<0)){
            anterior=prox;
            prox=prox->next;
        }
        //inserir no meio ou inserir no fim
        anterior->next=*aux;
        (*aux)->previous=anterior; //new
        (*aux)->next=prox;
        if(prox!=NULL)
            prox->previous=*aux; //new
    }
}

void insert_name(struct nomes **lista, struct nomes **aux){
    struct nomes *anterior, *prox;

    if((*lista == NULL) || (strcmp((*aux)->nome,(*lista)->nome)<0)){ //inserir quando a lista está vazia ou inserir no inicio
        (*aux)->next = *lista;
        *lista = *aux;
    }
    else{
        anterior=*lista;
        prox=(*lista)->next;
        while((prox!=NULL) && (strcmp(prox->nome ,(*aux)->nome)<0)){
            anterior=prox;
            prox=prox->next;
        }
        //inserir no meio ou inserir no fim
        anterior->next=*aux;
        (*aux)->next=prox;
    }
}

void insert_to_payments_lists(struct quotas **lista, struct quotas **aux){
    struct quotas *anterior, *prox;

    if((*lista == NULL) || (strcmp((*aux)->nome,(*lista)->nome)<0)){ //inserir quando a lista está vazia ou inserir no inicio
        (*aux)->next = *lista;
        *lista = *aux;
    }
    else{
        anterior=*lista;
        prox=(*lista)->next;
        while((prox!=NULL) && (strcmp(prox->nome ,(*aux)->nome)<0)){
            anterior=prox;
            prox=prox->next;
        }
        //inserir no meio ou inserir no fim
        anterior->next=*aux;
        (*aux)->next=prox;
    }
}