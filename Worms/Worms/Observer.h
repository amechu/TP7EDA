#pragma once

/*
Clase Observer, posee una funcion virtual pura update, la cual cada clase hija debe de sobreescribir.

El observador es notificado siempre que algun Subject que tenga al observador registrado sufra un cambio,
de manera que cada Subject llama a la funcion update del observador pasando como referencia el propio Subject,
para que el observador pueda mediante una serie de getters que deben de estar definidos en cada clase derivada
de un Subject, observar en que estado esta el Subject y a partir de este estado actuar.

Un ejemplo de Observer seria el observador para dibujar la pantalla. Este observador estaria registrado
en todos los sujetos los cuales tengan elementos que deben ser mostrados en pantalla. Cuando alguno de estos
sujetos cambia, el observador es notificado y a partir del estado de los Subjects dibujara o no en pantalla.

*/

enum class Observers {};

class Observer
{
public:
	Observer();
	virtual ~Observer();
	virtual void update(void* subject, void* tool) = 0;
};

