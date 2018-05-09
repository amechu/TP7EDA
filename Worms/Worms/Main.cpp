#include "Scenario.h"
#include "AllegroTools.h"
#include "Dispatcher.h"
#include "EventGenerator.h"
#include "GameSettings.h"
#include "Drawer.h"
#include "Sender.h"
#include "Network.h"
#include "Parser.h"

//Buscar "&0" en solucion para ver lo que falta hacer
//Falta: Todo lo de networking, desde mandar paquetes hasta armarlos hasta ver que hacer con el evento quit, o crear worm nuevo a partir de una coneccion.
//Considerar: los eventos en EventHandling.h que son refresh+algo son para el worm local y generados por allegro, los que son solo algo son para todos
//los demas worms de networking.


//SACAR RANDOM DE LOS ID, SACAR PACKET DE NETWORK QUE HAY UN packet y un Packet.

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
	
	Scene.registerObserver(&Drawer);
	Scene.registerObserver(&Sender);

	Parser.Read(argc, argv);

	Network.loadOwnIP(Parser);
	Network.loadOtherIP();

	if (AllegroTools.Init()) {

		AllegroTools.drawingInfo.LoadWormImages();
		AllegroTools.drawingInfo.arrangeWormCycle();

		Network.setIfHost(AllegroTools.askIfHost()); //Bloqueante, pantalla con dos botones, un boton para ser host, uno para client. &0
		Network.acceptOrResolve(gameSettings::port);

		if (Network.getIfHost() != QUITTER) 
		{

			if (Network.getIfHost() == HOST) {
				AllegroTools.drawWaitingForSomebody();
				Scene.createNewWorm(0, { gameSettings::LeftWall + 100 , gameSettings::GroundLevel }, WormDirection::Right);
				Network.createLineServer();
			}
			else {
				AllegroTools.drawTryingToConnect();
				Scene.createNewWorm(0, { gameSettings::LeftWall + 400 , gameSettings::GroundLevel }, WormDirection::Right);
				Network.createLineClient(Network.getOtherIP(), gameSettings::port);
			}

				Network.myWormPos = Scene.myWormPos;
				al_flush_event_queue(AllegroTools.Queue);

			while (Event.type != QUIT && (Network.getIfHost() != gameSettings::QUITTER))
			{

				Network.networkProtocol();

				EventGenerator.checkIncomingEvents(&AllegroTools, &Network);

				for (int i = 0; i < 2; i++) {

					if (!(EventGenerator.eventQueue.empty())) {
						Event = EventGenerator.fetchEvent();

						if (Event.type != NOEVENT) {
							Dispatcher.Dispatch(Event, &Scene, &AllegroTools);
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