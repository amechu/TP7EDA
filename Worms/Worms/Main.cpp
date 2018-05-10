#include "Scenario.h"
#include "AllegroTools.h"
#include "Dispatcher.h"
#include "EventGenerator.h"
#include "GameSettings.h"
#include "Drawer.h"
#include "Sender.h"
#include "Network.h"
#include "Parser.h"

//Notas: Muy poco frecuentemente son los eventos locales no tomados, lo que puede causar que se registre un keydown pero no el keyup
//lo que hace que el worm puede seguir caminando indefinidamente. Con volver a tocar la tecla de mover se arregla. Arreglar esto
//nos llevaría a entregar un día más tarde, por lo que decidimos entregarlo en este estado.

using namespace std;
int main(int argc, char* argv[]) {

	Parser Parser;

	Network Network(gameSettings::port);

	EventGenerator EventGenerator;
	Dispatcher Dispatcher;
	Event Event = { 0,0 };

	AllegroTools AllegroTools;
	Scenario Scene;
	Drawer Drawer;
	Sender Sender(&Network);
	
	Scene.registerObserver(&Drawer); //Registro el observador que dibujará
	Scene.registerObserver(&Sender); //Registro el observador que mandará información

	Parser.Read(argc, argv); //Lee la ip propia. (E.g. -ip 127.0.0.1)

	Network.loadOwnIP(Parser); //Carga datos a network
	Network.loadOtherIP();

	if (AllegroTools.Init()) { //Inicia allegro

		AllegroTools.drawingInfo.LoadWormImages(); //Load bitmaps
		AllegroTools.drawingInfo.arrangeWormCycle(); //Ordena arreglos de punteros a bitmap

		Network.setIfHost(AllegroTools.askIfHost()); //Menu principal, no se puede volver a acceder luego de elegir
		Network.acceptOrResolve(gameSettings::port); //Listen o connect

		if (Network.getIfHost() != QUITTER) //Si no se eligió salir en el menu principal..
		{

			if (Network.getIfHost() == HOST) { //Acciones si es server
				AllegroTools.drawWaitingForSomebody();
				Scene.createNewWorm(0, { gameSettings::LeftWall + 100 , gameSettings::GroundLevel }, WormDirection::Right);
				Network.createLineServer();
			}
			else {
				AllegroTools.drawTryingToConnect(); //Acciones si es client
				Scene.createNewWorm(0, { gameSettings::LeftWall + 400 , gameSettings::GroundLevel }, WormDirection::Right);
				Network.createLineClient(Network.getOtherIP(), gameSettings::port);
			}

				Network.myWormPos = Scene.myWormPos;
				al_flush_event_queue(AllegroTools.Queue); //Para no sobrecargar la cola con eventos de timer pasados

			while (Event.type != QUIT && (Network.getIfHost() != gameSettings::QUITTER))
			{

				Network.networkProtocol(); //Protocolo de networking, aca se esperan y se mandan paquetes.

				EventGenerator.checkIncomingEvents(&AllegroTools, &Network); //Aca se transforma de eventos de allegro / paquetes a eventos propios

				for (int i = 0; i < 2; i++) { //Se meten dos eventos en el caso mas extenso, se dispatchean dos eventos

					if (!(EventGenerator.eventQueue.empty())) {
						Event = EventGenerator.fetchEvent();

						if (Event.type != NOEVENT) {
							Dispatcher.Dispatch(Event, &Scene, &AllegroTools); //Se realizan acciones segun el evento.
						}
						else {
						}
					}
				}
			}
		}
	}
	return 0;
}