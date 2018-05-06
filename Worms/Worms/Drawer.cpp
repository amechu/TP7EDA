#include "Drawer.h"

Drawer::Drawer()
{
}


Drawer::~Drawer()
{
}

void Drawer::update(void* subject) {

}

void Drawer::update(void* subject, void * drawingTool)
{
	Scenario* Scene = (Scenario*)subject;
	AllegroTools* Drawing = (AllegroTools*)drawingTool;



	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.WindowsBackground, 0, 0, NULL);
	al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.Background, 0, 0, NULL);

	for (Worm& worm : (Scene->getWormList())) {

		Point Position = worm.getPosition();
		WormState State = worm.getState();
		WormDirection Direction = worm.getDirection();

		switch (State) {
			case WormState::Iddle: {
				al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.Wormcycle[1], Position.X, Position.Y, (Direction==WormDirection::Left? NULL : ALLEGRO_FLIP_HORIZONTAL));
				break;
			}
			case WormState::Walking: {
				al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.Wormcycle[worm.getTickCount()], Position.X, Position.Y, (Direction == WormDirection::Left ? NULL : ALLEGRO_FLIP_HORIZONTAL));
				break;
			}
			case WormState::Jumping: {
				if (worm.getTickCount() <= 2)
					al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.WormJump[worm.getTickCount() - 1], Position.X, Position.Y, (Direction == WormDirection::Left ? NULL : ALLEGRO_FLIP_HORIZONTAL));
				else if (worm.getTickCount() <= 25)
					al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.WormJump[2], Position.X, Position.Y, (Direction == WormDirection::Left ? NULL : ALLEGRO_FLIP_HORIZONTAL));
				else if (worm.getTickCount() <= 32)
					al_draw_bitmap(((AllegroTools*)drawingTool)->drawingInfo.WormJump[worm.getTickCount() - 23], Position.X, Position.Y, (Direction == WormDirection::Left ? NULL : ALLEGRO_FLIP_HORIZONTAL));
				break;
			}
		}
	}
	al_flip_display();
}
